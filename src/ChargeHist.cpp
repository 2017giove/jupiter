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


void Make(const char* fileIN, int CH);
void RawIntegral(const char *, const char *, int);

void ChargeHist() {
    TFile *f = (TFile*) gROOT->GetListOfFiles()->First();
    Make(f->GetName(), 0);

}

void ChargeHist(std::string _fileIN, int CH) {
    const char* fileIN = _fileIN.c_str();
    Make(fileIN, CH);
}

void Make(const char* fileIN, int CH) {
    int i, Nentries;
    float Integral;
    TFile* f;
    TFile *g;
    TTree *t1;
    TTree *t2;
    WaveForm Wave;

    char fileRAWname[STR_LENGTH];
    char histOUT[STR_LENGTH];

    std::strcpy(fileRAWname, appendToRootFilename(fileIN, "RAW").c_str());
    std::strcpy(histOUT, appendToRootFilename(fileIN, "hist").c_str());

    f = TFile::Open(fileRAWname);

    if (!f || f->IsZombie()) {
        printf("The file is being processed. You may go to sleep in the meanwhile\n");
        RawIntegral(fileIN, fileRAWname, CH);

        g = TFile::Open(fileRAWname);
        t1 = (TTree*) g->Get("t1");
    } else {
        t1 = (TTree*) f->Get("t1");
    }

    Nentries = t1->GetEntries();
    t1->SetBranchAddress("Integral", &Integral);

    //Crea l'istogramma e lo popola integrando le forme d'onda
    TH1D *h1 = new TH1D("h1", "Istogramma energia", NBIN, QMIN, QMAX);

    TCanvas *c40 = new TCanvas("c40", PLOTS_TITLE, 640, 480);


    for (i = 0; i < Nentries; i++) {
        t1->GetEntry(i);
        h1->Fill(Integral);
    }

    h1->GetXaxis()->SetTitle("Qualcosa proporzionale alla carica");
    h1->GetYaxis()->SetTitle("# eventi");

    h1->Draw();

    //Salva l'istogramma con fit sovrapposto su file root
    f = TFile::Open(fileIN);
    t2 = (TTree*) f->Get("tset");
    TFile *hist_file = new TFile(histOUT, "RECREATE");
    t2->CloneTree();
    h1->Write();
    hist_file->Write();
}

void RawIntegral(const char * fileIN, const char *fileOUT, int CH) {

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

    TFile *FOut = new TFile(fileOUT, "RECREATE");

    //Definisce TTree e TBranch del nuovo file
    TTree *TOut = new TTree("t1", "title");
    TBranch *b_integral = TOut->Branch("Integral", &Integral, "Integral/F");


    //Integra le forme d'onda, stima il valore massimo dell'array e li stampa sul file in output
    for (i = 0; i < Nentries; i++) {
        t1->GetEntry(i);
        Wave.FillVec(N_SAMPLES, temp.time_array[CH], temp.wave_array[CH], -1);
        Integral = Wave.Integral();
        BaseIntegral = Wave.BoundIntegral(0, (N_SAMPLES - (int) (st.delayns * RATE)));
        Integral -= BaseIntegral;
        TOut->Fill();
        
        printf("%d/%d\t", i, Nentries);
        printStatus((float) i / (float) Nentries);
        
    }



    TOut->Write();
    FOut->Write();

}