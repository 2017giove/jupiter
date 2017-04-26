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


void MakeChargeHist(const char* fileIN, int CH);
void RawIntegral(const char *, const char *);





void ChargeHist() {
    TFile *f = (TFile*) gROOT->GetListOfFiles()->First();
    MakeChargeHist(f->GetName(), 0);

}

void ChargeHist(std::string _fileIN, int CH) {
    const char* fileIN = _fileIN.c_str();
    MakeChargeHist(fileIN, CH);
}

void MakeChargeHist(const char* fileIN, int CH) {
    int i, Nentries;
    float Integral;
    float BaseIntegral;
    TFile* f;
    TFile *g;
    TTree *t1;
    TTree *tbase;
    TTree *t2;
    TTree* tset;
    WaveForm Wave;
    mySetting st;

    char fileRAWname[STR_LENGTH];
    char histOUT[STR_LENGTH];

    std::strcpy(fileRAWname, appendToRootFilename(fileIN, "RAW").c_str());
    std::strcpy(histOUT, appendToRootFilename(fileIN, "hist").c_str());

    f = TFile::Open(fileRAWname);

    if (!f || f->IsZombie()) {
        printf("The file is being processed. You may go to sleep in the meanwhile\n");
        RawIntegral(fileIN, fileRAWname);

        g = TFile::Open(fileRAWname);
        t1 = (TTree*) g->Get("t1");
        tbase = (TTree*) g->Get("tbase");
        tset = (TTree*) g->Get("tset");
    } else {
        t1 = (TTree*) f->Get("t1");
        tbase = (TTree*) f->Get("tbase");
        tset = (TTree*) f->Get("tset");
    }

//        tset->SetBranchAddress("PMT_ID", &st.PmtID);
//    tset->SetBranchAddress("Voltage", &st.voltage);
//    tset->SetBranchAddress("threshold", &st.thresh);
//    tset->SetBranchAddress("Delay_ns", &st.delayns);
//    tset->SetBranchAddress("Date", st.date);
//    tset->GetEntry(0);
//
//    printf("Data captured on %s", st.date);
//    printf("PMT_ID \t\t=\t%d\n", st.PmtID);
//    printf("Voltage(V)\t=\t%f\n", st.voltage);
//    printf("Threshold(mV)\t=\t%f\n", st.thresh);
//    printf("Delay(ns)\t=\t%d\n", st.delayns);
//    printf("\n");

    mySetting_get(tset, &st);
    mySetting_print(st);

    Nentries = t1->GetEntries();
    t1->SetBranchAddress("Integral", &Integral);
    tbase->SetBranchAddress("Baseline", &BaseIntegral);

    //Crea l'istogramma e lo popola integrando le forme d'onda
    TH1D *h1 = new TH1D("h1", "Istogramma energia", NBIN, QMIN, QMAX);
    TH1D *hbase = new TH1D("hbase", "Istogramma baseline", NBIN, QMIN, QMAX);

    for (i = 0; i < Nentries; i++) {
        t1->GetEntry(i);
        h1->Fill(Integral);

        tbase->GetEntry(i);
        hbase->Fill(BaseIntegral);
    }

    TCanvas *c40 = new TCanvas("c40", PLOTS_TITLE, 640, 480);
    h1->GetXaxis()->SetTitle("Qualcosa proporzionale alla carica");
    h1->GetYaxis()->SetTitle("# eventi");
    h1->Draw();


    TCanvas *c42 = new TCanvas("c42", PLOTS_TITLE, 640, 480);
    hbase->GetXaxis()->SetTitle("Qualcosa proporzionale alla carica");
    hbase->GetYaxis()->SetTitle("# eventi");
    hbase->Draw();


    //Salva l'istogramma con fit sovrapposto su file root
    f = TFile::Open(fileIN);
    TFile *hist_file = new TFile(histOUT, "RECREATE");
    tset->CloneTree();
    h1->Write();
    hbase->Write();
    hist_file->Write();
}

void RawIntegral(const char * fileIN, const char *fileOUT) {
    int i, j, Nentries;
    WaveForm Wave; //definizione di WaveForm e InputData in WaveAnalysis.h
    myEvent temp;
    float Integral, BaseIntegral, Max;

    //Apre il file di dati in input
    TFile *f = TFile::Open(fileIN);

    // AddBranchtoCache , SetCacheSize to speed up

    if (!f || f->IsZombie()) {
        printf("%s\nCannot write in %s\n.", ERROR_DEEPER, fileIN);
    }

    TTree* t1 = (TTree*) f->Get("t1");
    Nentries = t1->GetEntries();

    printf("This file contains %d events.\n", Nentries);

    t1->SetBranchAddress("wave_array", temp.wave_array);
    t1->SetBranchAddress("time_array", temp.time_array);

    TTree* tset1 = (TTree*) f->Get("tset");
    mySetting st;
    mySetting_get(tset1, &st);
    mySetting_print(st);
    
//        tset1->SetBranchAddress("PMT_ID", &st.PmtID);
//    tset1->SetBranchAddress("Voltage", &st.voltage);
//    tset1->SetBranchAddress("threshold", &st.thresh);
//    tset1->SetBranchAddress("Delay_ns", &st.delayns);
//    tset1->SetBranchAddress("Date", st.date);
//    tset1->GetEntry(0);
//
//    printf("Data captured on %s", st.date);
//    printf("PMT_ID \t\t=\t%d\n", st.PmtID);
//    printf("Voltage(V)\t=\t%f\n", st.voltage);
//    printf("Threshold(mV)\t=\t%f\n", st.thresh);
//    printf("Delay(ns)\t=\t%d\n", st.delayns);
//    printf("\n");
//    
    TFile *FOut = new TFile(fileOUT, "RECREATE");

    //Definisce TTree e crea TBranch del nuovo file
    TTree *Tspectrum = new TTree("t1", "spectrum");
    TTree *Tbaseline = new TTree("tbase", "baseline");

    TBranch *b_integral = Tspectrum->Branch("Integral", &Integral, "Integral/F");
    TBranch *b_baseline = Tbaseline->Branch("Baseline", &BaseIntegral, "Baseline/F");

    //Integra le forme d'onda, stima il valore massimo dell'array e li stampa sul file in output
    for (i = 0; i < Nentries; i++) {
        t1->GetEntry(i);
        Wave.FillVec(N_SAMPLES, temp.time_array, temp.wave_array, -1);
        Integral = Wave.Integral();
        BaseIntegral = Wave.BoundIntegral(0, (N_SAMPLES - (int) (st.delayns * RATE)));
      //  printf("delay %d\n",st.delayns);
        Integral -= BaseIntegral;
        Tspectrum->Fill();
        Tbaseline->Fill();
        printf("%d/%d\t", i, Nentries);
        printStatus((float) i / (float) Nentries);

    }



   // Tspectrum->Write();
  //  Tbaseline->Write();
    tset1->CloneTree();
    FOut->Write();

}