/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Every tooth in a man's head is more valuable than a diamond.
 * You don't have to brush your teeth — just the ones you want to keep.
 * 
 * 
 */
/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Do not modify it unless you know exactly what you are doing.
 * Otherwise you'll be in trouble.
 * 
 * It's Boguliubov!
 */

////////////////////////////////////////////
//                                        //
//       CHARGEHIST by DeathStar          //
//                                        //
//  listato di macro che fa l'istogramma  //
//  della carica portata da un segnale    //
//  dall'output di RawIntegral      e lo   //
//  fitta con un esponenziale decrescente //
//                                        //
////////////////////////////////////////////


#include "WaveAnalysis.h"

#include <string.h>
#include <stdio.h>


void MakeChargeHist(const char* fileIN);
void RawIntegral(const char *, const char *, int CH);

void ChargeHist() {
    TFile *f = (TFile*) gROOT->GetListOfFiles()->First();
    MakeChargeHist(f->GetName());

}

void ChargeHist(std::string _fileIN) {
    const char* fileIN = _fileIN.c_str();
    MakeChargeHist(fileIN);
}

void MakeChargeHist(const char* fileIN) {
    int i, Nentries;
    float Integral;
    float BaseIntegral;
    TFile* f;
    TTree *t1;
    TTree *tbase;
    TTree *t2;
    TTree* tset;
    WaveForm Wave;
    struct mySetting st;

    char tname [STR_LENGTH];
    char fileRAWname[STR_LENGTH];
    char histOUT[STR_LENGTH];

    std::strcpy(fileRAWname, appendToRootFilename(fileIN, "RAW").c_str());
    std::strcpy(histOUT, appendToRootFilename(fileIN, "hist").c_str());

    f = TFile::Open(fileRAWname, "read");

    if (!f || f->IsZombie()) {
        printf("The file is being processed. You may go to sleep in the meanwhile\n");


        TFile *fin = TFile::Open(fileIN);

        tset = (TTree*) fin->Get("tset");
        mySetting_get(tset, &st);
        mySetting_print(st);

        int aPMT;
         aPMT = CHtoPMT(CH, &st);
        sprintf(tname, "t%d", aPMT);
             
        Nentries = ((TTree*) fin->Get(tname))->GetEntries();
        printf("This file contains %d events.\n", Nentries);

        int ii;
        for (ii = 0; ii < st.Nchan; ii++) {
            RawIntegral(fileIN, fileRAWname, ii);
        }
        fin->Close();

        f = TFile::Open(fileRAWname, "read");

    }


    tset = (TTree*) f->Get("tset");
    mySetting_get(tset, &st);
    mySetting_print(st);

    int CH;
    int cPMT;
    TFile *hist_file = new TFile(histOUT, "UPDATE");


    TCanvas *c40 = new TCanvas("Fish", PLOTS_TITLE, 640, 480);

    for (CH = 0; CH < st.Nchan; CH++) {
        cPMT = CHtoPMT(CH, &st);
        sprintf(tname, "t%d", cPMT);
        t1 = (TTree*) f->Get(tname);
        sprintf(tname, "tbase%d", cPMT);
        tbase = (TTree*) f->Get(tname);

        //Definisce TTree e crea TBranch del nuovo file

        Nentries = t1->GetEntries();
        t1->SetBranchAddress("Integral", &Integral);
        tbase->SetBranchAddress("Baseline", &BaseIntegral);

        //Crea l'istogramma e lo popola integrando le forme d'onda
        sprintf(tname, "h%d", cPMT);
        TH1D *h1 = new TH1D(tname, "Istogramma energia", NBIN, QMIN, QMAX);

        sprintf(tname, "hbase%d", cPMT);
        TH1D *hbase = new TH1D(tname, "Istogramma baseline", NBIN, -QMAX / 10, QMAX / 10);

        for (i = 0; i < Nentries; i++) {
            t1->GetEntry(i);
            h1->Fill(Integral);

            tbase->GetEntry(i);
            hbase->Fill(BaseIntegral);
        }


        h1->GetXaxis()->SetTitle("Qualcosa proporzionale alla carica");
        h1->GetYaxis()->SetTitle("# eventi");
        h1->Write();
        sprintf(tname, "img/%s_charge%d.eps", filenameFromPath(fileIN).c_str(), cPMT);
        h1->Draw("");
        c40->SaveAs(tname);


        //sprintf(tname, "cbase%d", CH);
        //TCanvas *c42 = new TCanvas(tname, PLOTS_TITLE, 640, 480);
        hbase->GetXaxis()->SetTitle("Qualcosa proporzionale alla carica");
        hbase->GetYaxis()->SetTitle("# eventi");
        hbase->Write();
        sprintf(tname, "img/%s_base%d.eps", filenameFromPath(fileIN).c_str(), cPMT);
        hbase->Draw();
        c40->SaveAs(tname);

    }

    //Salva l'istogramma con fit sovrapposto su file root
    hist_file->cd();
    TTree* newtree = tset->CloneTree(0);
    newtree->Fill();
    newtree->Write("", TObject::kOverwrite);

    hist_file->Close();
    delete c40;
}

void RawIntegral(const char * fileIN, const char *fileOUT, int CH) {
    int i, j, Nentries;
    WaveForm Wave; //definizione di WaveForm in WaveAnalysis.h
    float Integral, BaseIntegral, Max;
    char branchDef[STR_LENGTH];
    char tname [STR_LENGTH];
    //Apre il file di dati in input
    TFile *f = TFile::Open(fileIN);
    TFile *FOut = new TFile(fileOUT, "UPDATE");
    // AddBranchtoCache , SetCacheSize to speed up

    if (!f || f->IsZombie()) {
        printf("%s\nCannot write in %s\n.", ERROR_DEEPER, fileIN);
    }

    TTree* tset1 = (TTree*) f->Get("tset");
    mySetting st;
    mySetting_get(tset1, &st);

    int cPMT = CHtoPMT(CH, &st);


    struct myEvent temp;
    //allocateEvent(&temp,st.Nchan);
    TTree* t1 = (TTree*) f->Get("t1");
    Nentries = t1->GetEntries();

    t1->SetBranchAddress("wave_array", temp.wave_array);
    t1->SetBranchAddress("time_array", temp.time_array);

    sprintf(tname, "t%d", cPMT);
    TTree *Tspectrum = new TTree(tname, "spectrum");
    sprintf(tname, "tbase%d", cPMT);
    TTree *Tbaseline = new TTree(tname, "baseline");

    Tspectrum->Branch("Integral", &Integral, "Integral/F");
    Tbaseline->Branch("Baseline", &BaseIntegral, "Baseline/F");

    //Integra le forme d'onda, stima il valore massimo dell'array e li stampa sul file in output
    for (i = 0; i < Nentries; i++) {
        t1->GetEntry(i);

        if (temp.trigCH = i) {

            Wave.FillVec(N_SAMPLES, temp.time_array[CH], temp.wave_array[CH], -1);
            Integral = Wave.Integral();
            BaseIntegral = Wave.BoundIntegral(0, (N_SAMPLES - (int) (st.delayns * RATE)));
            Integral -= BaseIntegral;

            Tspectrum->Fill();
            Tbaseline->Fill();
            printf("CH %d (PMT %d) ", CH, cPMT);
            printf("%d/%d ", i, Nentries);
            printStatus((float) i / (float) Nentries);

        }

    }
    printf("CH %d (PMT %d) completed\n", CH, cPMT);

    FOut->cd();
    Tspectrum->Write();
    Tbaseline->Write();

    TTree* newtree = tset1->CloneTree(0);
    newtree->Fill();
    newtree->Write("", TObject::kOverwrite);

    FOut->Close();
}