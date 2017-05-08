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

void ChargeHist(std::string _fileIN, int CH) {
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

    char fileRAWname[STR_LENGTH];
    char histOUT[STR_LENGTH];

    std::strcpy(fileRAWname, appendToRootFilename(fileIN, "RAW").c_str());
    std::strcpy(histOUT, appendToRootFilename(fileIN, "hist").c_str());

    f = TFile::Open(fileRAWname);

    if (!f || f->IsZombie()) {
        printf("The file is being processed. You may go to sleep in the meanwhile\n");

        TFile *fin = TFile::Open(fileIN);
        tset = (TTree*) fin->Get("tset");
        mySetting_get(tset, &st);
        mySetting_print(st);
        int ii;
        for (ii = 0; ii < st.Nchan; ii++) {
            RawIntegral(fileIN, fileRAWname, ii);
        }
    }



    f = TFile::Open(fileRAWname);
    tset = (TTree*) f->Get("tset");

    mySetting_get(tset, &st);
    mySetting_print(st);
    int CH;

    char tname [STR_LENGTH];

    TFile *hist_file = new TFile(histOUT, "UPDATE");


    for (CH = 0; CH < st.Nchan; CH++) {
        sprintf(tname, "t%d", CH);
        t1 = (TTree*) f->Get(tname);
        sprintf(tname, "tbase%d", CH);
        tbase = (TTree*) f->Get(tname);


        //Definisce TTree e crea TBranch del nuovo file

        Nentries = t1->GetEntries();
        t1->SetBranchAddress("Integral", &Integral);
        tbase->SetBranchAddress("Baseline", &BaseIntegral);

        //Crea l'istogramma e lo popola integrando le forme d'onda
        sprintf(tname, "h%d", CH);
        TH1D *h1 = new TH1D(tname, "Istogramma energia", NBIN, QMIN, QMAX);
        
        sprintf(tname, "hbase%d", CH);
        TH1D *hbase = new TH1D(tname, "Istogramma baseline", NBIN, QMIN, QMAX);

        for (i = 0; i < Nentries; i++) {
            t1->GetEntry(i);
            h1->Fill(Integral);

            tbase->GetEntry(i);
            hbase->Fill(BaseIntegral);
        }




                TCanvas *c40 = new TCanvas("c40", PLOTS_TITLE, 640, 480);
                h1->GetXaxis()->SetTitle("Qualcosa proporzionale alla carica");
                h1->GetYaxis()->SetTitle("# eventi");
                h1->Write();


                TCanvas *c42 = new TCanvas("c42", PLOTS_TITLE, 640, 480);
                hbase->GetXaxis()->SetTitle("Qualcosa proporzionale alla carica");
                hbase->GetYaxis()->SetTitle("# eventi");
                hbase->Write();


        //Salva l'istogramma con fit sovrapposto su file root
        if (hist_file->Get("tset") == NULL) {
            tset->CloneTree();
        }
       // h1->Write();
       // hbase->Write();
        hist_file->Write();

    }
}

void RawIntegral(const char * fileIN, const char *fileOUT, int CH) {
    int i, j, Nentries;
    WaveForm Wave; //definizione di WaveForm e InputData in WaveAnalysis.h

    float Integral, BaseIntegral, Max;
    char branchDef[STR_LENGTH];
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
    mySetting_print(st);

    struct myEvent temp;
    //allocateEvent(&temp,st.Nchan);




    TTree* t1 = (TTree*) f->Get("t1");
    Nentries = t1->GetEntries();


    //>>> mettere loop sui canali
    t1->SetBranchAddress("wave_array", temp.wave_array);
    t1->SetBranchAddress("time_array", temp.time_array);

    printf("This file contains %d events.\n", Nentries);
    char tname [STR_LENGTH];
    sprintf(tname, "t%d", CH);
    //Definisce TTree e crea TBranch del nuovo file
    TTree *Tspectrum = new TTree(tname, "spectrum");
    sprintf(tname, "tbase%d", CH);
    TTree *Tbaseline = new TTree(tname, "baseline");

    TBranch *b_integral = Tspectrum->Branch("Integral", &Integral, "Integral/F");
    TBranch *b_baseline = Tbaseline->Branch("Baseline", &BaseIntegral, "Baseline/F");



    //Integra le forme d'onda, stima il valore massimo dell'array e li stampa sul file in output
    for (i = 0; i < Nentries; i++) {
        t1->GetEntry(i);
        printf("Lorenzo ha detto %d\n", CH);
        Wave.FillVec(N_SAMPLES, temp.time_array[CH], temp.wave_array[CH], -1);
        Integral = Wave.Integral();
        BaseIntegral = Wave.BoundIntegral(0, (N_SAMPLES - (int) (st.delayns * RATE)));
        //  printf("delay %d\n",st.delayns);
        Integral -= BaseIntegral;
        Tspectrum->Fill();
        Tbaseline->Fill();
        printf("%d/%d\t", i, Nentries);
        printStatus((float) i / (float) Nentries);

    }



    //Tspectrum->Write();
    //  Tbaseline->Write();
    if (FOut->Get("tset") == NULL) {
        tset1->CloneTree();
    }
    FOut->Write();
    FOut->Close();
}