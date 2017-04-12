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

#include "defines.h"

#include <string.h>
#include <stdio.h>
void Make(TFile* f, const char* fileIN, int CH);
void RawIntegral(const char *, const char *, int);

void ChargeHist() {
    TFile *f = (TFile*) gROOT->GetListOfFiles()->First();
    Make(f, f->GetName(), 0); //Allarme lollo

}

void ChargeHist(std::string _fileIN, int CH) {
    TFile *f;
    const char* fileIN = _fileIN.c_str();
    Make(f, fileIN, CH);
}

void Make(TFile* f, const char* fileIN, int CH) {
    int i, Nentries;
    float Integral;
    TFile *g;
    TTree *t1;
    TTree *t2;
    WaveForm Wave;

    const char* fileRAWname;
    const char *histOUT;

    std::string _extension = ".root";
    std::string _fileIN = fileIN;
    std::string _fileRAWname;
    std::string _histOUT;

    _fileRAWname = _fileIN;
    _fileRAWname.replace(_fileIN.find(_extension), _extension.length(), "RAW.root");

    _histOUT = _fileIN;
    _histOUT.replace(_fileIN.find(_extension), _extension.length(), "hist.root");


    fileRAWname = _fileRAWname.c_str();
    histOUT = _histOUT.c_str();

    f = TFile::Open(fileRAWname);

    if (!f || f->IsZombie()) {
        printf("Il file %s non e' stato ancora analizzato.\nInizio creazione del file a partire da %s \n", fileRAWname, fileIN);

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

    int i,j, Nentries;
    WaveForm Wave; //definizione di WaveForm e InputData in WaveAnalysis.h
    myEvent temp;
    float Integral, BaseIntegral, Max;
    int delay;
    //Apre il file di dati in input
    TFile *f = TFile::Open(fileIN);

    if (!f || f->IsZombie()) {
        printf("Errore, mancata apertura del file %s\n. E' piu profondo.", fileIN);
    }

    TTree* t1 = (TTree*) f->Get("t1");
    TTree* tset1 = (TTree*) f->Get("tset");
    Nentries = t1->GetEntries();
    printf("Lorenzo dice che sono %d\n\n\n\n",Nentries);
    
    t1->SetBranchAddress("wave_array", temp.wave_Array);
    t1->SetBranchAddress("time_array", temp.time_Array);
    tset1->SetBranchAddress("Delay_ns", &delay);
    tset1->GetEntry(0);
    TFile *FOut = new TFile(fileOUT, "RECREATE");

    //Definisce TTree e TBranch del nuovo file
    TTree *TOut = new TTree("t1", "title");
    TBranch *b_integral = TOut->Branch("Integral", &Integral, "Integral/F");


    //Integra le forme d'onda, stima il valore massimo dell'array e li stampa sul file in output
    for (i = 0; i < Nentries; i++) {
        t1->GetEntry(i);
        Wave.FillVec(N_SAMPLES, temp.time_Array[CH], temp.wave_Array[CH], -1);
        Integral = Wave.Integral();
        BaseIntegral = Wave.BoundIntegral(0, (N_SAMPLES - (int) (delay * RATE)));
        Integral -= BaseIntegral;
        TOut->Fill();
    }



    TOut->Write();
    FOut->Write();
}