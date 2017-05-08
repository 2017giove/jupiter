/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Every tooth in a man's head is more valuable than a diamond.
 * You don't have to brush your teeth — just the ones you want to keep.
 * 
 * 
 */

#include "WaveAnalysis.h"

#include <string.h>
#include <stdio.h>

int FittingStartBin(float threshold, TH1F * hist);
void MakeWaveform(const char* fileIN);
void Waveform();
void MakeWaveform(const char* fileIN);
void Waveform(std::string _fileIN);
void RawWave(const char * fileIN, const char *fileOUT);

int FittingStartBin(float threshold, TH1F * hist) {
    int i;
    if (threshold < 0) {
        for (i = 0; i < hist->GetSize(); i++) {
            if (hist->GetBinContent(i) < threshold) return i - 1;
        }
        return 0;
    } else if (threshold >= 0) {
        for (i = 0; i < hist->GetSize(); i++) {
            if (hist->GetBinContent(i) > threshold) return i - 1;
        }
        return 0;
    }
}

void Waveform() {
    TFile *f = (TFile*) gROOT->GetListOfFiles()->First();
    MakeWaveform(f->GetName());

}

void Waveform(std::string _fileIN) {
    const char* fileIN = _fileIN.c_str();
    MakeWaveform(fileIN);
}

void MakeWaveform(const char* fileIN) {

    char histOUT[STR_LENGTH];
    std::strcpy(histOUT, appendToRootFilename(fileIN, "wave").c_str());
    TFile *f = TFile::Open(histOUT);

    if (!f || f->IsZombie()) {
        printf("The file is being processed. You may go to sleep in the meanwhile\n");
        RawWave(fileIN, histOUT);
        f = TFile::Open(histOUT);
    }

    //    TCanvas *c = new TCanvas("cA", PLOTS_TITLE, 640, 480);
    //    TCanvas *c2 = new TCanvas("cB", PLOTS_TITLE, 640, 480);
    //    
    //    TH1F *histo_ch1 = new TH1F("histo_ch1", "Forma del segnale", 1024, 0, N_SAMPLES);
    //    TH1F *histo_max = new TH1F("histo_max", "Istogramma dei massimi", 500, 0, 5024);
    //
    //    TH1F *showHist;
    //    TF1 *showFit;


    printf("Gli istogrammi sono stati salvati ma non verrano visualizzati ora.\nVedi come cambia l'ottica?\n");


    //
    //    c2->cd();
    //    showHist->Draw();
    //    showFit->Draw("pl same");
    //
    //    c->cd();
    //    histo_max->GetXaxis()->SetTitle("Qualcosa proporzionale alla corrente");
    //    histo_max->GetYaxis()->SetTitle("# eventi");
    //    histo_max->Draw("Hist");
    //    c2->Update();



}

void RawWave(const char * fileIN, const char *fileOUT) {

    TFile* f = TFile::Open(fileIN);
    TTree* t1 = (TTree*) f->Get("t1");
    TTree* tset = (TTree*) f->Get("tset");

    struct mySetting st;
    mySetting_get(tset, &st);
    mySetting_print(st);


    struct myEvent ev;
   // allocateEvent(&ev,st.Nchan );
    t1->SetBranchAddress("wave_array", &ev.wave_array[0][0]);

    TCanvas *c = new TCanvas("cA", PLOTS_TITLE, 640, 480);
    TCanvas *c2 = new TCanvas("cB", PLOTS_TITLE, 640, 480);


    TH1F *histo_ch1 = new TH1F("histo_ch1", "Forma del segnale", 1024, 0, N_SAMPLES);
    TH1F *histo_max = new TH1F("histo_max", "Istogramma dei massimi", 500, 0, 5024);

    TH1F *showHist;
    TF1 *showFit;

    TFile *FOut = new TFile(fileOUT, "RECREATE");


    int nentries = t1->GetEntries();
    int minimo;

    for (int jentry = 0; jentry < nentries; jentry++) {

        t1->GetEntry(jentry);
        TF1 *gf = new TF1("f1", "([0]*TMath::Exp(-[1]*(x-[3])) - [4]*TMath::Exp(-[2]*(x-[5])))", 0, N_SAMPLES);

        gf->SetParameter(0, 1620);
        gf->FixParameter(1, 0.00867);
        gf->FixParameter(2, 0.0992);
        gf->SetParameter(3, 220);
        gf->SetParameter(4, 1620);
        gf->SetParameter(5, 220);

        
        for (int k = 0; k < 1024; k++) {
            histo_ch1->SetBinContent(k,  ev.wave_array[0][k]);
        }

        TH1F *temp = (TH1F*) histo_ch1->Clone("GrongoHist");
        temp ->Rebin(16);
        for (int k = 0; k < 64; k++) {
            temp->SetBinContent(k, temp->GetBinContent(k) / 16);
        }

        // se vuoi velocizzare parti da start=(N_SAMPLES - (int) (delay * RATE))
        temp->Fit(gf, "Q", "", FittingStartBin(st.thresh[0], histo_ch1), N_SAMPLES);

        if (jentry % 5000 == 0) {
            showHist = (TH1F*) histo_ch1->Clone("GrongoWave");
            showFit = (TF1*) gf->Clone("GrongoCurve");
            printf("Pesco un granchio...\n");
            showHist->GetXaxis()->SetTitle("tempo (samples)");
            showHist->GetYaxis()->SetTitle("Segnale (mV)");

            //showFit->DrawF1(0, N_SAMPLES, "pl same");
            // showHist->(0, -800, N_SAMPLES, 50);
            //showFit->SetRange(0, -800, N_SAMPLES, 50);

            showHist->Draw();
            showFit->Draw("same");
        }

        minimo = -gf->GetMinimum(0, N_SAMPLES);
        histo_max->Fill(minimo, 1);

        printf("%d/%d\t", jentry, nentries);
        printStatus((float(jentry) / (float) nentries));

    }

    histo_max ->Write();
    FOut->Write();
}