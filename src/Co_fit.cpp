/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Every tooth in a man's head is more valuable than a diamond.
 * You don't have to brush your teeth — just the ones you want to keep.
 * 
 * 
 */

/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Every tooth in a man's head is more valuable than a diamond.
 * You don't have to brush your teeth — just the ones you want to keep.
 * 
 * 
 */

#ifndef COFIT

#define COFIT

#include "defines.h"
#include "WaveAnalysis.h"



peak Co_fit(TCanvas* c40, TH1D* h1, std::string savepath, mySetting* st, int PMTid);
void Co_getPeak(const char* src_name, int PMTid, char* wheretosave);
void Co_fitall(const char* src_name);

void Co_fitall(const char* src_name) {
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
        // printf("Mi aspetto LT%d, nchan = %d\n", PMTid, st.Nchan);
        int voltage = st.voltage[j];
        sprintf(temp2, "data/%s_%d.calfish", acqnameFromPath(src_name).c_str(), PMTid);
        printf("\nFilename iniziale %s \n>> Salvato in %s\n", src_name, temp2);
        Co_getPeak(src_name, PMTid, temp2);
    }


    // Ma non alla prima mano
    //Ba_fitsumofchannels(src_name);

}

void Co_getPeak(const char* src_name, int PMTid, char* wheretosave) {
    TFile *sorgente_file = TFile::Open(src_name);
    TCanvas *c41 = new TCanvas();
    peak mypeak;
    mySetting st;

    TTree* tset1 = (TTree*) sorgente_file->Get("tset");
    mySetting_get(tset1, &st);

    //
    char fileOUT[STR_LENGTH];
    strcpy(fileOUT, appendToRootFilename(sorgente_file->GetName(), "cofit").c_str());
    TFile *FOut = new TFile(fileOUT, "UPDATE");

    int CH = PMTtoCH(PMTid, &st);

    // queste variabili servono per salvare il fit sia in adc counts(carica) sia riscalato in kev
    char tname[STR_LENGTH];
    char tnames[2][STR_LENGTH];
    sprintf(tnames[0], "h%d", PMTid);
    sprintf(tnames[1], "hc%d", PMTid);

    char tfitnames[2][STR_LENGTH];
    sprintf(tfitnames[0], "hfit%d", PMTid);
    sprintf(tfitnames[1], "hcfit%d", PMTid);

    char timgnames[2][STR_LENGTH];
    sprintf(timgnames[0], "cofit");
    sprintf(timgnames[1], "ccofit");

    for (int k = 0; k < 2; k++) {
        sprintf(tname, "%s", tnames[k]);
        TH1D *h1 = (TH1D*) sorgente_file->Get(tname);

        if (h1 != nullptr) {
            sprintf(tname, "img/%s_%d_%s.eps", filenameFromPath(src_name).c_str(), PMTid, timgnames[k]);
            printf("Salva in %s\n", tname);

            mypeak = Co_fit(c41, h1, tname, &st, PMTid);
            mypeak.PMTid = PMTid;
            mypeak.voltage = st.voltage[CH];
            mypeak.thresh = st.thresh[CH];
            mypeak.resolution = mypeak.sigma / mypeak.peakpos; //già calcolato nel fit non serve farlo di nuovo
            mypeak.IA = mypeak.resolution / TMath::Sqrt(mypeak.peakvalue * mypeak.sigma);

            if (k == 0) peak_save(wheretosave, &mypeak);
            h1->SetName(tfitnames[k]);
            h1->Write();
        } else {
            printf("\nBOMB! Histofrish doesn't exist!! gamma=%d\n", k);
        }
    }

    TTree* newtree = tset1->CloneTree(0);
    newtree->Fill();
    newtree->Write("", TObject::kOverwrite);
    FOut->Close();


    delete c41;
}

peak Co_fit(TCanvas* c40, TH1D* h1, std::string savepath, mySetting* st, int PMTid) {


    c40->SetFillColor(0);

    if (PMTid != 0) {
        if (st->voltage[PMTtoCH(PMTid, st)] >= 1650) {
            h1->Rebin(4);
        }
    }

    struct peak mypeak;
    mypeak.anyproblems = 0;

    int nBins;
    float step; //invece di usare QMAX/nBins conviene usare GetBinWidth
    //printf("step %f\n\n", step);
    //int maxBin = GetMaximumBin(h1, 5. / step, nBins);
    std::vector<int> mymaxsbins;

    for (int sniper = 0; sniper < 10; sniper++) {


        nBins = h1->GetSize() - 2;
        step = (float) h1->GetXaxis()->GetBinWidth(0);
        mymaxsbins = GetMaximumBins(h1, 10 / step, nBins);

        if (mymaxsbins[0] == -1) {
            mypeak.anyproblems = NOT_FOUND_MAX;
            printf("WARNING: I did not find any max!");
        } else {
            if (h1->GetBinContent(mymaxsbins.back()) < h1->GetEntries() / 200) {
                printf("Sto scalando l'istograma...\n");
                h1->Rebin();
            } else {
                sniper = 10;
            }
        }

    }

    int maxBin = mymaxsbins.back();

    float Xmax = maxBin*step; //80
    // printf("\nxmax=%f\n", Xmax);

    float Xwindow; // = 3.8; // larghezza su cui eseguire a occhio il fit gaussiano rispetto a xmax rilevato
    // Xwindow = 150;
    //    if (PMTid == 0) {
    //        Xwindow = 150;
    //    }
    Xwindow = Xmax / 5;

    float Ymax = h1->GetBinContent(maxBin);


    //gROOT->SetStyle("Plain");
    gStyle->SetOptFit(1111);


    // prefit gaussiano
    TF1 *Gauss = new TF1("Gauss", "[0]*TMath::Exp(-(x-[1])*(x-[1])/(2*[2]*[2]))", Xmax - Xwindow, Xmax + Xwindow);
    Gauss->SetParNames("Amplitude", "PeakPos", "sigma");

    Gauss->SetParLimits(0, 0.8 * Ymax, 1.5 * Ymax); // ampiezza
    Gauss->SetParLimits(1, Xmax * 0.9, Xmax * 1.1); // xpicco
    Gauss->SetParLimits(2, 0, Xwindow); //sigma

    Gauss->SetParameter(0, Ymax);
    Gauss->SetParameter(1, Xmax);
    Gauss->SetParameter(2, Xwindow / 2);

    h1->Fit("Gauss", "Nq", "", Xmax - Xwindow, Xmax + Xwindow);

    Ymax = Gauss->GetParameter(0);
    Xmax = Gauss->GetParameter(1);

    float xmin = Xmax / ENERGY_COBALTO * ENERGY_COBASSO;



    //Scelta del PUNTO DI PARTENZA del fit
    float sigma = Gauss->GetParameter(2);

    //   printf("Min amp %d\t %f\n", FD2minAmpX, FD2minAmp);
    float startfitpoint = 10; // da cambiare in caso di nuove prese dati
    int intfitpoint = startfitpoint / step;
    //    if (mymaxsbins[0] < intfitpoint) intfitpoint = mymaxsbins[0];
    //
    //    if (mymaxsbins.size() > 2) {
    //
    //        for (int j = 0; j < mymaxsbins.size() - 1; j++) {
    //            //  printf("spiker %d\t%d\n", intfitpoint, mymaxsbins[j]);
    //            float cy = h1->GetBinContent(mymaxsbins[j]);
    //            float oy = h1->GetBinContent(intfitpoint);
    //            //    printf("sanTy %d\t%f\t%d\t%f\t%f\n\n\n\n", mymaxsbins[j], cy, intfitpoint, oy, FDCompton);
    //            if (cy > oy && mymaxsbins[j] < FDCompton / step) {
    //                intfitpoint = mymaxsbins[j];
    //            }
    //            //    printf("nasTy %d\t%f\t%d\t%f\t%f\n\n\n\n", mymaxsbins[j], cy, intfitpoint, oy, FDCompton);
    //        }
    //    }

    startfitpoint = intfitpoint*step;

    float FDCompton = Xmax * (1 - 1 / (1 + 2 * ENERGY_COBALTO / MASS_ELECTRON));
    float FDCompton2 = xmin * (1 - 1 / (1 + 2 * ENERGY_COBASSO / MASS_ELECTRON));

    //    int FD2minAmpX = GetMinimumBin(h1, (int) (FDCompton / step), maxBin);
    //    float FD2minAmp = h1->GetBinContent(FD2minAmpX);



    // FIT 
    h1->GetXaxis()->SetRange(Xmax * 0 / step, Xmax * 1.5 / step);

    TF1 *fsrc = new TF1("fsrc", "[0]*([1]*TMath::Exp((-x/[2]))+  (1-[1])*TMath::Exp((-x/[3])))     + [4]/TMath::Exp((x-[5])*(x-[5])/(2*[6]*[6])) +      [7]/(TMath::Exp((x-[8])*[9])+1)       + [10]/(TMath::Exp((x-[11])*[12])+1)    + [13]/TMath::Exp((x-[14])*(x-[14])/(2*[15]*[15]))", startfitpoint, Xmax * 2);
    //                   0        1           2       3           4           5       6       7       8               9
    //                   0        1           2       3           4           5       6       7       8               9
    fsrc->SetParNames("BGAmp", "BGratio", "tau_1", "tau_2", "GaussAmp", "Peak", "sigma", "FDCAmp", "FDCShift", "FDCBeta");

    fsrc->SetParName(10, "FDCAmp2");
    fsrc->SetParName(11, "FDCShift2");
    fsrc->SetParName(12, "FDCBeta2");

    fsrc->SetParName(13, "GaussAmp4");
    fsrc->SetParName(14, "Peak4");
    fsrc->SetParName(15, "sigma4");

    fsrc->SetParLimits(0, 0, RAND_MAX);
    fsrc->SetParLimits(1, 0.9, 0.99); //OK
    fsrc->SetParLimits(2, Xmax / 20, Xmax/5 );
    fsrc->SetParLimits(3, Xmax / 10, Xmax * 2);



    fsrc->SetParLimits(4, Ymax * 0.7, Ymax * 2); //OK! Valore numerici scelti con cura non cambiare
    fsrc->SetParLimits(5, Xmax *0.9, Xmax * 1.5);
    fsrc->SetParLimits(6, sigma * 0.1, sigma * 2);

    fsrc->SetParLimits(7, Ymax / 10, Ymax);
    fsrc->SetParLimits(8, FDCompton * 0.80, FDCompton * 1.20);
    fsrc->SetParLimits(9, 0.05, 10);

    fsrc->SetParLimits(10, Ymax/ 3,  Ymax);
    fsrc->SetParLimits(11, FDCompton2 * 0.80, FDCompton2 * 1.20);
    fsrc->SetParLimits(12, 0.7, 10);

    fsrc->SetParLimits(13, Ymax * 0.7, Ymax * 2); //OK!
    fsrc->SetParLimits(14, Xmax*0.9 , Xmax * 1.1);
    fsrc->SetParLimits(15, sigma * 0.1, sigma * 2);


    fsrc->SetParameter(0, Ymax);
    fsrc->SetParameter(1, 0.5);
    // fsrc->SetParameter(2, FDCompton);
    // fsrc->SetParameter(3, FDCompton);
    fsrc->SetParameter(4, Ymax * 0.65);
    fsrc->SetParameter(5, xmin);
    fsrc->SetParameter(6, sigma);

    fsrc->SetParameter(7, Ymax / 3);
    fsrc->SetParameter(8, FDCompton);
    fsrc->SetParameter(9, sigma * 0.5296);

    fsrc->SetParameter(10, Ymax / 3);
    fsrc->SetParameter(11, FDCompton2);
    fsrc->SetParameter(12, sigma * 0.5296);

    fsrc->SetParameter(4, Ymax);
    fsrc->SetParameter(5, Xmax);
    fsrc->SetParameter(6, sigma);

    h1->Fit("fsrc", "L", "", startfitpoint, Xmax * 2); // prima la FDCompton * 2 / 3 //vL options
    h1->Draw();

    /*****************************************************/

    //grafici delle funzioni usate per il fit

    //Replot doppio exp
    TF1 *BG = new TF1("doppioexp", "[0]*([1]*TMath::Exp((-x/[2]))+  (1-[1])*TMath::Exp((-x/[3])))", 0, 50);
    BG->FixParameter(0, fsrc->GetParameter(0));
    BG->FixParameter(1, fsrc->GetParameter(1));
    BG->FixParameter(2, fsrc->GetParameter(2));
    BG->FixParameter(3, fsrc->GetParameter(3));

    BG->SetLineColor(2);
    BG->SetLineStyle(2);

    BG->Draw("same");


    //Replot Gauss (Photon Peak)
    TF1 *G1 = new TF1("G1", "[4]/TMath::Exp((x-[5])*(x-[5])/(2*[6]*[6]))", 0, 50);

    G1->FixParameter(4, fsrc->GetParameter(4));
    G1->FixParameter(5, fsrc->GetParameter(5));
    G1->FixParameter(6, fsrc->GetParameter(6));

    G1->SetLineColor(8);
    G1->SetLineStyle(2);

    G1->Draw("same");


    //Replot Gauss (Photon Peak)
    TF1 *G2 = new TF1("G2", "[7]/(TMath::Exp((x-[8])*[9])+1)", 0, 50);

    G2->FixParameter(7, fsrc->GetParameter(7));
    G2->FixParameter(8, fsrc->GetParameter(8));
    G2->FixParameter(9, fsrc->GetParameter(9));

    G2->SetLineColor(9);
    G2->SetLineStyle(2);

    G2->Draw("same");



    //Replot Gauss (Photon Peak)
    TF1 *G3 = new TF1("G3", "[10]/(TMath::Exp((x-[11])*[12])+1)", 0, 50);

    G3->FixParameter(10, fsrc->GetParameter(10));
    G3->FixParameter(11, fsrc->GetParameter(11));
    G3->FixParameter(12, fsrc->GetParameter(12));

    G3->SetLineColor(12);
    G3->SetLineStyle(2);

    G3->Draw("same");


    //Replot Gauss (Photon Peak)
    TF1 *G8 = new TF1("G8", "[13]/TMath::Exp((x-[14])*(x-[14])/(2*[15]*[15]))", 0, 50);

    G8->FixParameter(13, fsrc->GetParameter(13));
    G8->FixParameter(14, fsrc->GetParameter(14));
    G8->FixParameter(15, fsrc->GetParameter(15));

    G8->SetLineColor(11);
    G8->SetLineStyle(2);

    G8->Draw("same");




    h1->SetTitle("Spettro del Co60");
    h1->SetName("Risultati del Fit");
    h1->GetXaxis()->SetTitle("adc Counts");
    h1->GetYaxis()->SetTitle("Eventi");
    gPad->SetGrid();


    //Replot funzione con parametri trovata
    TF1 *fitmax = new TF1("fsrc", "[0]*([1]*TMath::Exp((-x/[2]))+  (1-[1])*TMath::Exp((-x/[3])))     + [4]/TMath::Exp((x-[5])*(x-[5])/(2*[6]*[6])) +      [7]/(TMath::Exp((x-[8])*[9])+1)       + [10]/(TMath::Exp((x-[11])*[12])+1)    + [13]/TMath::Exp((x-[14])*(x-[14])/(2*[15]*[15]))", startfitpoint, Xmax*2);

    fitmax->FixParameter(0, fsrc->GetParameter(0));
    fitmax->FixParameter(1, fsrc->GetParameter(1));
    fitmax->FixParameter(2, fsrc->GetParameter(2));
    fitmax->FixParameter(3, fsrc->GetParameter(3));
    fitmax->FixParameter(4, fsrc->GetParameter(4));
    fitmax->FixParameter(5, fsrc->GetParameter(5));
    fitmax->FixParameter(6, fsrc->GetParameter(6));
    fitmax->FixParameter(7, fsrc->GetParameter(7));
    fitmax->FixParameter(8, fsrc->GetParameter(8));
    fitmax->FixParameter(9, fsrc->GetParameter(9));
    fitmax->FixParameter(10, fsrc->GetParameter(10));
    fitmax->FixParameter(11, fsrc->GetParameter(11));
    fitmax->FixParameter(12, fsrc->GetParameter(12));
    fitmax->FixParameter(13, fsrc->GetParameter(13));
    fitmax->FixParameter(14, fsrc->GetParameter(14));
    fitmax->FixParameter(15, fsrc->GetParameter(15));

    mypeak.sigma = fsrc->GetParameter("sigma");
    mypeak.peakpos = fsrc->GetParameter("Peak");
    mypeak.sigma_err = fsrc->GetParError(6);
    mypeak.peakpos_err = fsrc->GetParError(5);
    mypeak.peakvalue = fitmax->Eval(mypeak.peakpos);

    // lasciato al lettore volenteroso: Ci sono problemi più profondi <<<<<
    float nTOT = h1->Integral(h1->GetXaxis()->FindBin(mypeak.peakpos - mypeak.sigma), h1->GetXaxis()->FindBin(mypeak.peakpos + mypeak.sigma));
    mypeak.nSGN = fitmax ->Integral(mypeak.peakpos - mypeak.sigma, mypeak.peakpos + mypeak.sigma);
    mypeak.nBG = nTOT - mypeak.nSGN;

    mypeak.resolution = mypeak.sigma / mypeak.peakpos;

    printf("RISOLUZIONE = %f\n", mypeak.resolution);



    if (mypeak.resolution < 0.01) {
        mypeak.anyproblems = NOT_CREDIBLE;
        printf("Questo pesce va rigettato in acqua. E' un granchio.\n");
    }

    /*if (fsrc->GetProb() < 0.0001) {
        mypeak.anyproblems = NOT_PROBABLE;
        printf("Questo pesce è una burla. Non ci sto credendo che hai pescato un elefante. Questo essere va rigettato in acqua. E' un granchio?\n");
    }*/

    if (h1->GetEntries() < 2000) {
        mypeak.anyproblems = NOT_GOODFISHERMAN;
        printf("Questo pescatore non è stato esperto ed è tornato a mani vuote: solo %d pesciolini.\n", (int) h1->GetEntries());
    }

    if (mypeak.anyproblems != 0) {
        c40->SetFillColor(46);
        h1->SetFillColor(46);

    }
    fitmax->Draw("same");
    c40->Draw();


    mySetting_histoprint(st, PMTid);
    char temp[STR_LENGTH];
    sprintf(temp, "Risoluzione %f ", mypeak.resolution);
    TText *label1 = new TText();
    label1->SetNDC();
    label1->SetTextSize(0.03);
    label1->DrawText(0.5, 0.00, temp);



    TPaveStats* ps = (TPaveStats *) h1->GetListOfFunctions()->FindObject("stats");
    if (ps != nullptr) {

        ps->SetX1NDC(0.05);
        ps->SetX2NDC(0.25);
        ps->SetY1NDC(0.10);
        ps->SetY2NDC(0.50);
    }

    c40->SaveAs(savepath.c_str());

    return mypeak;
}

#endif