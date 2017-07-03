/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Every tooth in a man's head is more valuable than a diamond.
 * You don't have to brush your teeth — just the ones you want to keep.
 * 
 * 
 */

#ifndef J_WAVEFORM

#define J_WAVEFORM 

#include <TView3D.h>
#include "WaveAnalysis.h"


#include "TProfile2D.h"
#include <string.h>
#include <stdio.h>

int FittingStartBin(float threshold, TH1F * hist);
//void Waveform(int PMTid);
void MakeWaveform(const char* fileIN, int PMTid = -1);
//void Waveform(std::string _fileIN);
void WaveProfile(const char* src_name);
void RawWave(const char * fileIN, const char *fileOUT, int PMTid);
void DrawWaveSplot(const char * fileIN, const char *fileOUT, int PMTid);
TH1F* plotWaveFromCharge(const char * fileIN, int PMTid, float charge);

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

void WaveformAll(const char* src_name) {
    char temp2[STR_LENGTH];
    // printf("i am reading %s\n", acqnameFromPath(src_name).c_str());
    TFile *sorgente_file = TFile::Open(src_name);

    if (sorgente_file == nullptr) {
        printf("BOMB! No file found!");
    }

    mySetting st;
    TTree* tset1 = (TTree*) sorgente_file->Get("tset");
    mySetting_get(tset1, &st);

    for (int j = 0; j < st.Nchan; j++) {
        int PMTid = CHtoPMT(j, &st);
        MakeWaveform(src_name, PMTid);
    }

}


//void Waveform(int PMTid) {
//    TFile *f = (TFile*) gROOT->GetListOfFiles()->First();
//    MakeWaveform(f->GetName(), PMTid);
//
//}

//void Waveform(std::string _fileIN) {
//    const char* fileIN = _fileIN.c_str();
//    MakeWaveform(fileIN);
//}

void MakeWaveform(const char* fileIN, int PMTid) {

    char histOUT[STR_LENGTH];
    strcpy(histOUT, appendToRootFilename(fileIN, "wave").c_str());
    TFile *f = TFile::Open(histOUT);

    if (!f || f->IsZombie()) {
        printf("The file is being processed. You may go to sleep in the meanwhile\n");
        RawWave(fileIN, histOUT, PMTid);

        // DrawWaveSplot(fileIN, histOUT, PMTid);
        //f = TFile::Open(histOUT);
    } else {
        printf("Gli istogrammi già esistono in %s\n", histOUT);
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

void WaveProfile(const char* src_name) {



    //TFile *f = (TFile*) gROOT->GetListOfFiles()->First();
    TFile *f = TFile::Open(src_name);

    char fileOUT[STR_LENGTH];
    strcpy(fileOUT, appendToRootFilename(f->GetName(), "wave").c_str());
    TFile *FOut = new TFile(fileOUT, "UPDATE");
    int i, j;

    TH1F *histo_ch1;
    TTree* tset = (TTree*) f->Get("tset");
    char tname [STR_LENGTH];
    struct mySetting st;
    mySetting_get(tset, &st);
    mySetting_print(&st);

    int CH;


    Exotourbillion* Rieussec = new Exotourbillion();
    for (i = 0; i < st.Nchan; i++) {
        TCanvas *c41 = new TCanvas("Fish", PLOTS_TITLE, 640, 480);
        //     for (i = 0; i < 1; i++) {
        CH = i;


        TTree* t1 = (TTree*) f->Get("t1");
        TTree* tset = (TTree*) f->Get("tset");

        int jentry;
        struct myEvent temp;
        int nentries = t1->GetEntries();

        t1->SetBranchAddress("trigCH", &temp.trigCH);
        t1->SetBranchAddress("wave_array", temp.wave_array);
        t1->SetBranchAddress("time_array", temp.time_array);

        float Integral, BaseIntegral, Max;
        WaveForm Wave;

        sprintf(tname, "wp%d", st.PmtID[i]);
        TProfile2D * sprofh = new TProfile2D(tname, "Profile della waveform", N_SAMPLES / 5, 0, N_SAMPLES, 200 + 1, 0, 200);
        // sprofh->SetBinEntries(200,1);
        int totEventiQ[100] = {0};


        //Ciclo sugli eventi
        for (jentry = 0; jentry < nentries; jentry++) {
            t1->GetEntry(jentry);

            Wave.FillVec(N_SAMPLES, temp.time_array[CH], temp.wave_array[CH], -1);
            Integral = Wave.Integral();
            BaseIntegral = Wave.BoundIntegral(0, (N_SAMPLES - (int) ((st.delayns + BASE_SPAGO) * RATE)));
            Integral -= BaseIntegral;

            for (j = 0; j < N_SAMPLES; j++) {
                int cQ = (int) (Integral / sprofh->GetYaxis()->GetBinWidth((j)));

                if (cQ < 0 || cQ > 99) {
                    //printf("\n\n%f\t%d\n\n\n",Integral,cQ);
                    cQ = 99;
                }
                //float oldBinContent = sprofh->GetBinContent(j, cQ);
                //  sprofh->SetBinContent(j, cQ, oldBinContent + (-temp.wave_array[CH][j] / Integral));
                //sprofh->Fill(j, cQ, (-temp.wave_array[CH][j] / Integral));
                //printf("\n%d\t%d\t%f\n\n", j, cQ, oldBinContent + (-temp.wave_array[CH][j] / Integral));

                //  sprofh->Fill(j,Integral / sprofh->GetYaxis()->GetBinWidth(j),  temp.wave_array[CH][j] / Integral);
                sprofh->Fill(j, Integral, temp.wave_array[CH][j] / Integral);
                totEventiQ[cQ]++;

            }



            printf("%d/%d ", jentry, nentries);
            printStatus((float) jentry / (float) nentries);

        }
        printf("\nHo impiegato %lf\n\n", Rieussec->TotalUsedInk());

        // ananas inutile
        //        int jj = sprofh->GetNbinsY();
        //        float z;
        //        for (j = 0; j < jj; j++) {
        //            for (int k = 0; k < N_SAMPLES; k++) {
        //                if (totEventiQ[j] != 0) {
        //                    z = sprofh->GetBinContent(k, j) / (float) totEventiQ[j];
        //                    sprofh->SetBinContent(k, j, z);
        //
        //                }
        //            }
        //
        //        }


        int col = 1;
        //Qual è l'intervallo che ci interessa???
        for (int ii = 10; ii < 40; ii += 5) {
            sprintf(tname, "x%d", ii);
            TProfile *a = sprofh->ProfileX(tname, ii, ii + 5);
            //a->SetLineColor(ii + 1);
            // a->SetLineWidth(7);
            a->SetMarkerStyle(8);
            a->SetMarkerSize(0.4);
            a->SetMarkerColor(col++);
            a->Draw("same");
        }



        //        TH1D* aa = sprofh->ProjectionX("px", 0, 10);
        //        aa->SetLineColor(2);
        //        aa->Draw( "same"     );


        //sprofh->Draw("surf3"  );

        c41->Write();
        char tname22 [STR_LENGTH];
        std::string myname = filenameFromPath((f->GetName()));

        sprintf(tname22, "img/%s_wp%d.eps", myname.c_str(), st.PmtID[i]);

        printf("nome: %s\n\n", tname22);
        c41->SaveAs(tname22);
        delete c41;
    }


    // FOut->Close();
    // delete c41;
}

void plotWaveStepCharge() {
    TFile *f = (TFile*) gROOT->GetListOfFiles()->First();
    char fileOUT[STR_LENGTH];
    strcpy(fileOUT, appendToRootFilename(f->GetName(), "wave").c_str());
    TFile *FOut = new TFile(fileOUT, "UPDATE");
    int i, j;

    TH1F *histo_ch1;
    TTree* tset = (TTree*) f->Get("tset");
    char tname [STR_LENGTH];
    struct mySetting st;
    mySetting_get(tset, &st);
    mySetting_print(&st);
    TCanvas *c40 = new TCanvas("Fish", PLOTS_TITLE, 640, 480);
    FILE * plot3d = fopen("plot3d.jpt", "w");


    int p = 0;

    for (j = 0; j < st.Nchan; j++) {

        sprintf(tname, "p3d%d", st.PmtID[j]);
        TCanvas *c41 = new TCanvas(tname, PLOTS_TITLE, 640, 480);
        TGraph2D* splot3d = new TGraph2D();


        for (i = 5; i < 150; i += 10) {
            histo_ch1 = plotWaveFromCharge(f->GetName(), st.PmtID[j], i);
            if (histo_ch1 != NULL) {
                sprintf(tname, "cwave%d_%d", st.PmtID[j], i);
                FOut->cd();
                c40->cd();
                histo_ch1->SetName(tname);
                histo_ch1->Write();

                sprintf(tname, "img/%s_cwave%d_%d.eps", filenameFromPath(f->GetName()).c_str(), st.PmtID[j], i);
                histo_ch1->Draw();
                c40->SaveAs(tname);

                sprintf(tname, "cfit%d_%d", st.PmtID[j], i);
                TH1F *temp = (TH1F*) histo_ch1->Clone(tname);
                temp ->Rebin(8);
                for (int k = 0; k < N_SAMPLES / 8; k++) {
                    temp->SetBinContent(k, temp->GetBinContent(k) / 8);
                }

                TF1 *fitfunct = new TF1("f1", "([0]*TMath::Exp(-[1]*(x-[3])) - [4]*TMath::Exp(-[2]*(x-[5])))", 0, N_SAMPLES);

                fitfunct->SetParameter(0, 1620);
                fitfunct->FixParameter(1, 0.00867);
                fitfunct->FixParameter(2, 0.0992);
                fitfunct->SetParameter(3, 220);
                fitfunct->SetParameter(4, 1620);
                fitfunct->SetParameter(5, 220);



                // se vuoi velocizzare parti da start=(N_SAMPLES - (int) (delay * RATE))
                temp->Fit(fitfunct, "Q", "", FittingStartBin(st.thresh[j], histo_ch1), N_SAMPLES);

                for (int k = (N_SAMPLES - (int) (st.delayns * RATE)) + 10; k < N_SAMPLES - 300; k++) {
                    fprintf(plot3d, "%d\t%d\t%f\n", i, k, fitfunct->Eval(k));
                    splot3d->SetPoint(p++, k, i, fitfunct->Eval(k));
                }

                temp->Write();
            } else {
                printf("%s. We could not fish this fish.\n", ERROR_DEEPER);
            }

        }
        c41->cd();
        //gStyle->SetPalette(53);

        TView3D *view = (TView3D*) TView::CreateView(1);
        view->RotateView(-75, 65);


        splot3d->SetName(tname);
        splot3d->SetTitle("Waveform - Charge");
        splot3d->GetXaxis()->SetBinLabel(1, "time ");
        splot3d->GetYaxis()->SetBinLabel(1, "ADC counts");
        splot3d->GetZaxis()->SetBinLabel(1, "charge");
        splot3d->GetXaxis()->CenterTitle();
        splot3d->GetYaxis()->CenterTitle();
        splot3d->GetZaxis()->CenterTitle();
        splot3d->Draw("surf");
        gPad->Modified();


        c41->Write();
        sprintf(tname, "img/%s_p3d%d.eps", filenameFromPath(f->GetName()).c_str(), st.PmtID[j]);
        c41->SaveAs(tname);
    }


    fclose(plot3d);
    FOut->Close();
    delete c40;
}

void plotWaveFromCharge_show(const char * fileIN, int PMTid, float charge) {
    TCanvas *c = new TCanvas("cA", PLOTS_TITLE, 640, 480);
    TH1F *histo_ch1 = plotWaveFromCharge(fileIN, PMTid, charge);
    histo_ch1->Draw();
}

TH1F * plotWaveFromCharge(const char * fileIN, int PMTid, float charge) {

    TFile* f = TFile::Open(fileIN);

    TTree* t1 = (TTree*) f->Get("t1");
    TTree* tset = (TTree*) f->Get("tset");

    struct mySetting st;
    mySetting_get(tset, &st);
    mySetting_print(&st);

    int CH = PMTtoCH(PMTid, &st);
    if (CH == NOT_FOUND_INT) {
        printf("PMT non nel file. %s\n", ERROR_FISHERMAN);
    }


    int jentry;
    struct myEvent temp;
    int nentries = t1->GetEntries();

    t1->SetBranchAddress("trigCH", &temp.trigCH);
    t1->SetBranchAddress("wave_array", temp.wave_array);
    t1->SetBranchAddress("time_array", temp.time_array);

    float Integral, BaseIntegral, Max;
    WaveForm Wave;

    //TCanvas *c = new TCanvas("cA", PLOTS_TITLE, 640, 480);
    TH1F *histo_ch1 = new TH1F("histo_ch1", "Forma del segnale", N_SAMPLES, 0, N_SAMPLES);

    for (jentry = 0; jentry < nentries; jentry++) {
        t1->GetEntry(jentry);
        Wave.FillVec(N_SAMPLES, temp.time_array[CH], temp.wave_array[CH], -1);
        Integral = Wave.Integral();
        BaseIntegral = Wave.BoundIntegral(0, (N_SAMPLES - (int) (st.delayns * RATE)));
        Integral -= BaseIntegral;
        if (Integral > charge * 0.95 && Integral < charge * 1.05) {
            for (int k = 0; k < 1024; k++) {
                histo_ch1->SetBinContent(k, temp.wave_array[CH][k]);
            }

            //histo_ch1->Draw();

            printf("Do you want some more imgurs?\n");
            return histo_ch1;
        }

    }
    printf("I'm sorry but i could not find any waveform with such a charge.\n%s", ERROR_FISHERMAN);
    return histo_ch1;
}

void DrawWaveSplot(const char * fileIN, const char *fileOUT, int PMTid) {
    TFile* f = TFile::Open(fileIN);

    TTree* t1 = (TTree*) f->Get("t1");
    TTree* tset = (TTree*) f->Get("tset");

    struct mySetting st;
    mySetting_get(tset, &st);
    mySetting_print(&st);

    int CH = PMTtoCH(PMTid, &st);
    if (CH == NOT_FOUND_INT) {
        printf("PMT non nel file. %s\n", ERROR_FISHERMAN);
    }

    struct myEvent ev;
    t1->SetBranchAddress("trigCH", &ev.trigCH);
    t1->SetBranchAddress("wave_array", &ev.wave_array[0][0]);

    int nentries = t1->GetEntries();
    int jentry;

    TFile *FOut = new TFile(fileOUT, "UPDATE");

    TCanvas *c = new TCanvas("cA", PLOTS_TITLE, 640, 480);
    TGraph2D *splot = new TGraph2D(nentries * N_SAMPLES); //"histo_ch1", "Forma del segnale", N_SAMPLES, 50, 500, nentries/1000, 50, 500
    splot->GetXaxis()->SetRange(0, 1024);
    splot->GetYaxis()->SetRange(0, nentries);
    splot ->GetZaxis()->SetRange(2000, -2000);
    printf("Scrittura in corso\n");
    int s = 0;
    FILE* santanas = fopen("santanas.jpt", "w");
    for (jentry = 0; jentry < nentries; jentry++) {
        t1->GetEntry(jentry);
        for (int k = 0; k < N_SAMPLES; k++) {
            splot->SetPoint(s++, k, jentry, ev.wave_array[CH][k]);
            fprintf(santanas, "%d\t%d\t%f\n", k, jentry, ev.wave_array[CH][k]);
            //printf("%f\n", ev.wave_array[CH][k]);
        }
        printf("%d/%d\n", jentry, nentries);
    }
    fclose(santanas);
    gStyle->SetPalette(1);
    splot->Print();
    // splot->Draw("");

    splot->Write();

    FOut->Close();

}

void RawWave(const char * fileIN, const char *fileOUT, int PMTid) {

    char tname[STR_LENGTH];

    TFile* f = TFile::Open(fileIN);

    TTree* t1 = (TTree*) f->Get("t1");
    TTree* tset = (TTree*) f->Get("tset");

    struct mySetting st;
    mySetting_get(tset, &st);
    //mySetting_print(st);

    int CH = PMTtoCH(PMTid, &st);
    if (CH == NOT_FOUND_INT) {
        printf("PMT non nel file. %s\n", ERROR_FISHERMAN);
    }


    struct myEvent ev;
    // allocateEvent(&ev,st.Nchan );
    t1->SetBranchAddress("trigCH", &ev.trigCH);
    t1->SetBranchAddress("wave_array", &ev.wave_array[0][0]);

    TCanvas *c = new TCanvas("cA", PLOTS_TITLE, 640, 480);
    TCanvas *c2 = new TCanvas("cB", PLOTS_TITLE, 640, 480);


    TH1F *histo_ch1 = new TH1F("histo_ch1", "Forma del segnale", N_SAMPLES, 0, N_SAMPLES);
    TH1F *histo_max = new TH1F("histo_max", "Istogramma dei massimi", 500, 0, 5024);

    TH1F *showHist;
    TF1 *showFit;

    TFile *FOut = new TFile(fileOUT, "RECREATE");

    int nentries = t1->GetEntries();
    int minimo;

    for (int jentry = 0; jentry < nentries; jentry++) {

        t1->GetEntry(jentry);
        TF1 *fitfunct = new TF1("f1", "([0]*TMath::Exp(-[1]*(x-[3])) - [4]*TMath::Exp(-[2]*(x-[5])))", 0, N_SAMPLES);

        fitfunct->SetParameter(0, 1620);
        fitfunct->FixParameter(1, 0.00867);
        fitfunct->FixParameter(2, 0.0992);
        fitfunct->SetParameter(3, 220);
        fitfunct->SetParameter(4, 1620);
        fitfunct->SetParameter(5, 220);

        for (int k = 0; k < 1024; k++) {
            histo_ch1->SetBinContent(k, ev.wave_array[CH][k]);
        }

        TH1F *temp = (TH1F*) histo_ch1->Clone("GrongoHist");
        temp ->Rebin(16);
        for (int k = 0; k < 64; k++) {
            temp->SetBinContent(k, temp->GetBinContent(k) / 16);
        }

        // se vuoi velocizzare parti da start=(N_SAMPLES - (int) (delay * RATE))
        temp->Fit(fitfunct, "Q", "", FittingStartBin(st.thresh[CH], histo_ch1), N_SAMPLES);

        if (jentry % 500 == 0) {
            showHist = (TH1F*) histo_ch1->Clone("GrongoWave");
            showFit = (TF1*) fitfunct->Clone("GrongoCurve");
            printf("Pesco un granchio...\n");
            showHist->GetXaxis()->SetTitle("tempo (samples)");
            showHist->GetYaxis()->SetTitle("Segnale (mV)");

            //showFit->DrawF1(0, N_SAMPLES, "pl same");
            // showHist->(0, -800, N_SAMPLES, 50);
            //showFit->SetRange(0, -800, N_SAMPLES, 50);

            showHist->Draw();
            showFit->Draw("same");
        }

        minimo = -fitfunct->GetMinimum(0, N_SAMPLES);
        histo_max->Fill(minimo, 1);

        printf("%d/%d\t", jentry, nentries);
        printStatus((float(jentry) / (float) nentries));

    }

    histo_max ->Write();
    FOut->Write();
}


#endif