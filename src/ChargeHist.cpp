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
//  dall'output di RawIntegral      e lo  //
//  fitta con un esponenziale decrescente //
//                                        //
////////////////////////////////////////////

#ifndef CHARGEHIST

#define CHARGEHIST

#include "WaveAnalysis.h"
#include <string.h>
#include <stdio.h>


void RawIntegral(const char *, const char *, int CH);

void MakeChargeHist(const char* fileIN, std::string fileext = "hist") {
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


    strcpy(fileRAWname, appendToRootFilename(fileIN, "RAW").c_str());
    strcpy(histOUT, appendToRootFilename(fileIN, fileext.c_str()).c_str());

    f = TFile::Open(fileRAWname, "read");

    if (!f || f->IsZombie()) {
        printf("The file is being processed. You may go to sleep in the meanwhile\n");


        TFile *fin = TFile::Open(fileIN);

        tset = (TTree*) fin->Get("tset");
        mySetting_get(tset, &st);
        mySetting_print(&st);

        Nentries = ((TTree*) fin->Get("t1"))->GetEntries();
        printf("This file contains %d events.\n", Nentries);

        int ii;
        Exotourbillion* Guggy = new Exotourbillion();
        for (ii = 0; ii < st.Nchan; ii++) {
            Guggy->WakeFromBed();
            RawIntegral(fileIN, fileRAWname, ii);
            printf("Ho impiegato %lf tempo cpu.\n\n", Guggy->TotalUsedInk());
        }
        fin->Close();

        f = TFile::Open(fileRAWname, "read");

    }


    tset = (TTree*) f->Get("tset");
    mySetting_get(tset, &st);
    mySetting_print(&st);

    int CH;
    int cPMT;
    TFile *hist_file = new TFile(histOUT, "UPDATE");


    TCanvas *c40 = new TCanvas("Fish", PLOTS_TITLE, 640, 480);

    //Codice per istogramma in kev
    bool amIcalibratedFish = isthisfilepublic(CALIB_FILE);
    std::vector<myPMTcalib> calibs;
    if (amIcalibratedFish) {
        calibs = calib_load(CALIB_FILE);
    }
    sprintf(tname, "hctot");
    TH1D *htot = new TH1D(tname, "Istogramma energia tot", NBIN / 4, QMIN, 3 * CAESIUM_PEAK);


    for (CH = 0; CH < st.Nchan; CH++) {
        cPMT = CHtoPMT(CH, &st);
        int calibIndex = -1;
        double scalingAnanas = -1;

        if (amIcalibratedFish) {
            calibIndex = PMTidToCalibIndex(cPMT, calibs);
            scalingAnanas = CAESIUM_PEAK / TMath::Exp(calibs[calibIndex].A * st.voltage[CH] + calibs[calibIndex].B);
        }

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

        //Istogramma calibrato
        sprintf(tname, "hc%d", cPMT);
        TH1D *h2 = new TH1D(tname, "Istogramma energia 661", NBIN, QMIN, 3 * CAESIUM_PEAK);


        for (i = 0; i < Nentries; i++) {
            t1->GetEntry(i);
            h1->Fill(Integral);

            if (amIcalibratedFish && (calibIndex != NOT_FOUND_INT)) {
                h2->Fill(Integral * scalingAnanas);
                htot->Fill(Integral * scalingAnanas);
            }

            tbase->GetEntry(i);
            hbase->Fill(BaseIntegral);
        }


        h1->GetXaxis()->SetTitle("Qualcosa proporzionale alla carica");
        h1->GetYaxis()->SetTitle("# eventi");
        h1->Write();
        sprintf(tname, "img/%s_charge%d.eps", filenameFromPath(fileIN).c_str(), cPMT);
        h1->Draw("");
        mySetting_histoprint(&st, cPMT);
        c40->SaveAs(tname);

        if (amIcalibratedFish && (calibIndex != NOT_FOUND_INT)) {
            h2->Rebin(4);
            h2->GetXaxis()->SetTitle("keV");
            h2->GetYaxis()->SetTitle("# eventi");
            h2->Write();
            sprintf(tname, "img/%s_charge_calib%d.eps", filenameFromPath(fileIN).c_str(), cPMT);
            h2->Draw("");
            mySetting_histoprint(&st, cPMT);
            c40->SaveAs(tname);

        }



        //sprintf(tname, "cbase%d", CH);
        //TCanvas *c42 = new TCanvas(tname, PLOTS_TITLE, 640, 480);
        hbase->GetXaxis()->SetTitle("Qualcosa proporzionale alla carica");
        hbase->GetYaxis()->SetTitle("# eventi");
        hbase->Write();
        sprintf(tname, "img/%s_base%d.eps", filenameFromPath(fileIN).c_str(), cPMT);
        hbase->Draw();
        c40->SaveAs(tname);

    }

    if (amIcalibratedFish) {

        htot->GetXaxis()->SetTitle("keV");
        htot->GetYaxis()->SetTitle("# eventi");
        htot->Write();
        sprintf(tname, "img/%s_charge_calibTOT.eps", filenameFromPath(fileIN).c_str());
        htot->Draw("");
        mySetting_histoprint(&st, 0);
        c40->SaveAs(tname);

    }

    //Salva l'istogramma con fit sovrapposto su file root
    hist_file->cd();
    TTree* newtree = tset->CloneTree(0);
    newtree->Fill();
    newtree->Write("", TObject::kOverwrite);

    hist_file->Close();
    //  delete c40;
}

void ChargeHist(std::string _fileIN, std::string fileext2 = "hist") {
    const char* fileIN = _fileIN.c_str();
    MakeChargeHist(fileIN, fileext2);
}

void RawIntegral(const char * fileIN, const char *fileOUT, int CH) {
    int i, j, Nentries;
    int N_spikes = 0;
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
    //  mySetting_print(&st);
    int cPMT = CHtoPMT(CH, &st);


    struct myEvent temp;
    //allocateEvent(&temp,st.Nchan);
    TTree* t1 = (TTree*) f->Get("t1");
    Nentries = t1->GetEntries();

    t1->SetBranchAddress("trigCH", &temp.trigCH);
    t1->SetBranchAddress("wave_array", temp.wave_array);
    t1->SetBranchAddress("time_array", temp.time_array);

    sprintf(tname, "t%d", cPMT);
    TTree *Tspectrum = new TTree(tname, "spectrum");
    sprintf(tname, "tbase%d", cPMT);
    TTree *Tbaseline = new TTree(tname, "baseline");

    Tspectrum->Branch("Integral", &Integral, "Integral/F");
    Tbaseline->Branch("Baseline", &BaseIntegral, "Baseline/F");
    TH1F *histo_spike = new TH1F("histo_spike", "Forma delle spike", N_SAMPLES, 0, N_SAMPLES);




    TCanvas *c400 = new TCanvas("SpikeFish", PLOTS_TITLE, 640, 480);
    // TCanvas *c401 = new TCanvas("SaturatedFish", PLOTS_TITLE, 640, 480);
    int nsat = 0;

    //Integra le forme d'onda, stima il valore massimo dell'array e li stampa sul file in output
    for (i = 0; i < Nentries; i++) {
        t1->GetEntry(i);
        //(printf("\n%d\n\n", temp.trigCH);
        if (temp.trigCH == CH || st.triggerSetting == 768) {
            Wave.FillVec(N_SAMPLES, temp.time_array[CH], temp.wave_array[CH], -1);
            BaseIntegral = Wave.BoundIntegral(0, (N_SAMPLES - (int) ((st.delayns + BASE_SPAGO) * RATE)));

            int trigpos = triggerbin(st.thresh[CH], temp.wave_array[CH]);

            if (isaspike(temp.wave_array[CH], trigpos, BaseIntegral) == 0) {

                Integral = Wave.Integral();
                Integral -= BaseIntegral;
                Tspectrum->Fill();
                Tbaseline->Fill();


                //        get_minimum_pos(temp.wave_array[CH], trigpos,N_SAMPLES);

                if (issaturated(temp.wave_array[CH], trigpos) == 1) {
                    nsat++;

                    //                    sprintf(tname, "sat%d", N_spikes);
                    //                    TH1F *histo_ch2 = new TH1F(tname, "Saturation", N_SAMPLES, 0, N_SAMPLES);
                    //                    int k;
                    //                    for (k = 0; k < N_SAMPLES; k++) {
                    //                        histo_ch2->SetBinContent(k, temp.wave_array[CH][k]);
                    //                    }
                    //                    //     FOut->cd();
                    //                    //    c401->cd();
                    //                    histo_ch2->Draw();
                    //                    histo_ch2->Write();
                    //                    //  sprintf(tname, "img/sat%d.jpg", N_spikes);
                    //                    //   c401->SaveAs(tname);

                }




            } else {

                N_spikes++;


                //fare funzione Spikes 
                //                for (int k = 0; k < 1024; k++) {
                //                    histo_spike->SetBinContent(k, histo_spike->GetBinContent(k) + temp.wave_array[CH][k]);
                //                }
                //
                //
                //
                //
                //
                //                if (N_spikes % 500 == 0) {
                //                    sprintf(tname, "sp%d", N_spikes);
                //                    TH1F *histo_ch1 = new TH1F(tname, "Spikes", N_SAMPLES, 0, N_SAMPLES);
                //                    int k;
                //                    for (k = 0; k < N_SAMPLES; k++) {
                //                        histo_ch1->SetBinContent(k, temp.wave_array[CH][k]);
                //                    }
                //                    //  FOut->cd();
                //                    // c400->cd();
                //                    histo_ch1->Draw();
                //                    histo_ch1->Write();
                //                    //    sprintf(tname, "img/sp%d.jpg", N_spikes);
                //                    //    c400->SaveAs(tname);
                //
                //                }
            }

        }

        //printf("CH %d (PMT %d) ", CH, cPMT);
        printf("%d/%d ", i, Nentries);
        printStatus((float) i / (float) Nentries);

    }

    //    for (int k = 0; k < 1024; k++) {
    //        histo_spike->SetBinContent(k, histo_spike->GetBinContent(k) / (float) N_spikes);
    //    }

    // c400->cd();
    // histo_spike->Draw();
    //   sprintf(tname, "img/avgSP%d.jpg", CH);
    //  c400->SaveAs(tname);



    printf("\nCH %d (PMT %d) completed. Lollo ha avuto %d erezioni spiked e %d fellatio saturated.\n", CH, cPMT, N_spikes, nsat);

    FOut->cd();
    Tspectrum->Write();
    Tbaseline->Write();
    // histo_spike->Write();

    TTree* newtree = tset1->CloneTree(0);
    newtree->Fill();
    newtree->Write("", TObject::kOverwrite);

    FOut->Close();
}

#endif