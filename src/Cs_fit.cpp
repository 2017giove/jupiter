/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Do not modify it unless you know exactly what you are doing.
 * Otherwise you'll be in trouble.
 * 
 * It's Boguliubov!
 * 
 * Original code written by DeathStar, "Sferici" 2016
 * 
 */

#ifndef CSFIT

#define CSFIT

#include "defines.h"
#include "WaveAnalysis.h"

int GetMaximumBin(TH1D* hist, int from, int to);
std::vector<int> GetMaximumBins(TH1D* hist, int from, int to) ;
int GetMinimumBin(TH1D* hist, int from, int to);
struct peak Cs_fit(TCanvas* c40, TH1D* h1, std::string savepath, mySetting* st, int PMTid);
void Cs_getPeak(const char* src_name, int PMTid, char* wheretosave);
void PMTRangeLT(char * capturename, int PMTid);
void Cs_fitsumofchannels(const char*src_name);
void Cs_fitall(const char* src_name);

/**
 * Massimo boss
 */
int GetMaximumBin(TH1D* hist, int from, int to) {
    int i;
    int max = 0;
    int imax=-1;
    int currentmax;
    for (i = from; i < to; i++) {
        currentmax = hist->GetBinContent(i);
        if (currentmax > max) {
            max = currentmax;
            imax = i;
        }

    }
    return imax;

}

int GetMinimumBin(TH1D* hist, int from, int to) {
    int i;
    int min = RAND_MAX;
    int imin=-1;
    int currentmin;
    //  printf("\n\nfrom %d\t to%d\n", from, to);
    for (i = from; i < to; i++) {
        currentmin = hist->GetBinContent(i);
        if (currentmin < min) {
            min = currentmin;
            imin = i;
            //   printf("%d\t%d\n", imin, currentmin);
        }

    }
    return imin;

}

/**
 * Massimi locali
 */
std::vector<int> GetMaximumBins(TH1D* hist, int from, int to) {
    std::vector<float> myMaxs;
    std::vector<int> myMaxsX;

    int i;
    int max = 0;
    int imax;
    int currentmax;

    float totlength = 0;
    float media = 0;

    for (i = from; i < to; i++) {
        if (hist->GetBinContent(i) != 0) {
            media += (float) hist->GetBinContent(i);
            totlength++;
        }
    }
    media /= totlength;

    float nearmeR[10000] = {0};
    float nearmeL[10000] = {0};

    int nsumR = 0;
    int nsumL = 0;

    int ultimoSopraX = -1;

    for (int i = to; i > from; i--) {
        if (hist->GetBinContent(i) > media && ultimoSopraX == -1) {
            ultimoSopraX = i;
        }
    }

    float windowSize = ultimoSopraX * 0.05;
    //  printf("windowsize %f\n", windowSize);


    for (i = from; i < to; i++) {
        nsumR = 0;
        nearmeR[i] = 0;
        for (int j = i; j < i + windowSize; j++) {
            if (j >= from && j < to && j != i) {
                nearmeR[i] += hist->GetBinContent(j);
                nsumR++;
            }

        }
        nearmeR[i] /= (float) nsumR;

        nsumL = 0;
        nearmeL[i] = 0;

        for (int j = i - windowSize; j < i; j++) {
            if (j >= from && j < to && j != i) {
                nearmeL[i] += hist->GetBinContent(j);
                nsumL++;
            }

        }
        nearmeL[i] /= (float) nsumL;
        //  printf("nearme %d\t%f\t%f\t%f\n", i, hist->GetBinContent(i), nearmeL[i], nearmeR[i]);
    }


    bool foundmax = 0;

    float cmax = 0;
    int cpos = 0;


    for (i = from; i < to; i++) {

        if (foundmax == 0) {
            if (hist->GetBinContent(i) > nearmeR[i] && hist->GetBinContent(i) > nearmeL[i] && hist->GetBinContent(i) > media) {
                foundmax = 1;
                cmax = hist->GetBinContent(i);
                cpos = i;
            }

        } else {

            if (hist->GetBinContent(i) > cmax) {
                cmax = hist->GetBinContent(i);
                cpos = i;
            }

            if ((hist->GetBinContent(i) < nearmeR[i] && hist->GetBinContent(i) < nearmeL[i]) || hist->GetBinContent(i) < media) {
                foundmax = 0;
                myMaxs.push_back(cmax);
                myMaxsX.push_back(cpos);
                cmax = 0;
                cpos = 0;
                //    printf("csono %d\t%d\t%f\t%f\t%f\n", i, cpos, cmax, nearmeL[cpos], nearmeR[cpos]);
            }

        }
    }


    // printf("media %f\n", media);
    for (i = 0; i < myMaxs.size(); i++) {

        //   printf("sono %d\t%d\n", myMaxsX[i], myMaxs[i]);

    }

    if (myMaxsX.size() == 0) {

        myMaxsX.push_back(1);
    }
    return myMaxsX;


}


/**
 * Salva il picco trovato e la tensione di acquisizione, e i rispettivi errori, in una nuova riga del file specificato
 * @param nome file hist.root contenente l'istogramma
 * @param nome file in cui aggiungere la riga 
 */
void Cs_getPeak(const char* src_name, int PMTid, char* wheretosave) {
    TFile *sorgente_file = TFile::Open(src_name);
    TCanvas *c41 = new TCanvas();
    peak mypeak;
    mySetting st;

    TTree* tset1 = (TTree*) sorgente_file->Get("tset");
    mySetting_get(tset1, &st);

    //
    char fileOUT[STR_LENGTH];
    strcpy(fileOUT, appendToRootFilename(sorgente_file->GetName(), "csfit").c_str());
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
    sprintf(timgnames[0], "csfit");
    sprintf(timgnames[1], "ccsfit");

    for (int k = 0; k < 2; k++) {
        sprintf(tname, "%s", tnames[k]);
        TH1D *h1 = (TH1D*) sorgente_file->Get(tname);

        if (h1 != nullptr) {
            sprintf(tname, "img/%s_%d_%s.png", filenameFromPath(src_name).c_str(), PMTid, timgnames[k]);
            printf("Salva in %s\n", tname);

            mypeak = Cs_fit(c41, h1, tname, &st, PMTid);
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

void Cs_fitsumofchannels(const char*src_name) {
    printf("Good People mi chiamano così!\n\n");
    TCanvas *c41 = new TCanvas();
    TFile *sorgente_file = TFile::Open(src_name);
    peak mypeak;
    mySetting st;
    TTree* tset1 = (TTree*) sorgente_file->Get("tset");
    mySetting_get(tset1, &st);


    char fileOUT[STR_LENGTH];
    strcpy(fileOUT, appendToRootFilename(sorgente_file->GetName(), "csfit").c_str());
    TFile *FOut = new TFile(fileOUT, "UPDATE");

    char tname[STR_LENGTH];
    sprintf(tname, "hctot");
    TH1D *h1 = (TH1D*) sorgente_file->Get(tname);

    if (h1 != nullptr) {
        sprintf(tname, "img/%s_tot_csfit.png", filenameFromPath(src_name).c_str());
        printf("Salva in %s\n", tname);
        mypeak = Cs_fit(c41, h1, tname, &st, 0);
        h1->Write();
    } else {
        printf("\nBOMB! Big Histofrish doesn't exist!!\n");
    }

    delete c41;
}

void Cs_fitall(const char* src_name) {
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
        Cs_getPeak(src_name, PMTid, temp2);
    }    
        
    Cs_fitsumofchannels(src_name);

}

// PER FARE FIT SCALATO (CIOE IN KEV) BISOGNA PASSARGLI hc%d e non h%d e semplicamente riscalare di almeno 4 i bin

/**
 * This is the function that fits a histograms and gives back information about Caesium peak
 * @param h1    Histogram object to be fitted
 * @param savepath  Where to save fitted plot image
 * @param st    Settings of that data acquisition
 * @param PMTid ID of current PMT that is being fitted
 * @return Returns properties of Caesium peak, and fit results
 */
struct peak Cs_fit(TCanvas* c40, TH1D* h1, std::string savepath, mySetting* st, int PMTid) {


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
        mymaxsbins = GetMaximumBins(h1, 0.5 / step, nBins);

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


    //Scelta del PUNTO DI PARTENZA del fit
    float sigma = Gauss->GetParameter(2);
    float FDCompton = Xmax * (1 - 1 / (1 + 2 * ENERGY_CESIO / MASS_ELECTRON));

    int FD2minAmpX = GetMinimumBin(h1, (int) (FDCompton / step), maxBin);
    float FD2minAmp = h1->GetBinContent(FD2minAmpX);
    //   printf("Min amp %d\t %f\n", FD2minAmpX, FD2minAmp);
    float startfitpoint = FDCompton;
    int intfitpoint = startfitpoint / step;
    if (mymaxsbins[0] < intfitpoint) intfitpoint = mymaxsbins[0];


    if (mymaxsbins.size() == 1) {
        intfitpoint = (FDCompton * 3 / 4 / step);
        mypeak.anyproblems = NOT_COMPTON;
        printf("%s\nThere is just one big spike in your plot. Are you cutting some good signal with this trigger? This fish will be thrown overboard.\n", ERROR_DEEPER);
    }

    if (mymaxsbins.size() > 2) {

        for (int j = 0; j < mymaxsbins.size() - 1; j++) {
            //  printf("spiker %d\t%d\n", intfitpoint, mymaxsbins[j]);
            float cy = h1->GetBinContent(mymaxsbins[j]);
            float oy = h1->GetBinContent(intfitpoint);
            //    printf("sanTy %d\t%f\t%d\t%f\t%f\n\n\n\n", mymaxsbins[j], cy, intfitpoint, oy, FDCompton);
            if (cy > oy && mymaxsbins[j] < FDCompton / step) {
                intfitpoint = mymaxsbins[j];
            }
            //    printf("nasTy %d\t%f\t%d\t%f\t%f\n\n\n\n", mymaxsbins[j], cy, intfitpoint, oy, FDCompton);
        }
    }

    startfitpoint = intfitpoint*step;


    // FIT 
    h1->GetXaxis()->SetRange(Xmax * 0 / step, Xmax * 1.5 / step);

    TF1 *fsrc = new TF1("fsrc", "[0]*([1]*TMath::Exp((-x/[2]))+  (1-[1])*TMath::Exp((-x/[3])))     + [4]/TMath::Exp((x-[5])*(x-[5])/(2*[6]*[6])) +      [7]/(TMath::Exp((x-[8])*[9])+1)        +[10]/([12]*TMath::Exp((x-[5])*[11])+1)", startfitpoint, Xmax * 2);
    //                   0        1           2       3           4           5       6       7       8               9
    fsrc->SetParNames("BGAmp", "BGratio", "tau_1", "tau_2", "GaussAmp", "Peak", "sigma", "FDCAmp", "FDCShift", "FDCBeta");
    fsrc->SetParName(10, "FD2Amp");
    fsrc->SetParName(11, "FD2Beta");
    fsrc->SetParName(12, "FD2Modulation");

    fsrc->SetParLimits(0, FD2minAmp * 5, Ymax * 10);
    fsrc->SetParLimits(1, 0.01, 0.99); //OK
    fsrc->SetParLimits(2, Xmax / 10, Xmax * 2);
    fsrc->SetParLimits(3, Xmax / 10, Xmax * 2);
    fsrc->SetParLimits(4, Ymax * 0.8, Ymax * 1.2); //OK!
    fsrc->SetParLimits(5, Xmax * 0.9, Xmax * 1.1);
    fsrc->SetParLimits(6, sigma * 0.7, sigma * 1.3);
    fsrc->SetParLimits(7, 0, Ymax * 10);
    fsrc->SetParLimits(8, FDCompton * 0.90, FDCompton * 1.10);
    fsrc->SetParLimits(9, 0.1, 2);
    fsrc->SetParLimits(10, FD2minAmp, 2 * FD2minAmp);
    fsrc->SetParLimits(11, 0.1, 1);
    fsrc->SetParLimits(12, 0.01, 20);
    //    
    // se il minimo tra i due picchi è zero
    if (FD2minAmp == 0) {
        FD2minAmp = 1;
        fsrc->SetParLimits(10, FD2minAmp, 5 * FD2minAmp);
    }


    fsrc->SetParameter(0, Ymax);
    fsrc->SetParameter(1, 0.5);
    fsrc->SetParameter(2, FDCompton);
    fsrc->SetParameter(3, FDCompton);
    fsrc->SetParameter(4, Ymax);
    fsrc->SetParameter(5, Xmax);
    fsrc->SetParameter(6, sigma);
    fsrc->SetParameter(7, Ymax / 3);
    fsrc->SetParameter(8, FDCompton);
    fsrc->SetParameter(9, sigma * 0.5296);
    fsrc->SetParameter(10, FD2minAmp * 1.5);
    fsrc->SetParameter(11, 0.6); //basta che parte
    fsrc->SetParameter(12, 0.95);
    //fsrc->FixParameter(12, 1);

    h1->Fit("fsrc", "L", "", startfitpoint, Xmax * 2); // prima la FDCompton * 2 / 3 //vL options
    h1->Draw();

    /*****************************************************/

    //grafici delle funzioni usate per il fit

    //Replot senza doppio esponenziale
    TF1 *wow = new TF1("nonlovogliamo", "[4]/TMath::Exp((x-[5])*(x-[5])/(2*[6]*[6])) + [7]/(TMath::Exp((x-[8])*[9])+1)   +[12]/(TMath::Exp((x-[5])*[13])*[14]+1)  ", Xmax * 0.3, Xmax * 1.6);

    wow->FixParameter(4, fsrc->GetParameter(4));
    wow->FixParameter(5, fsrc->GetParameter(5));
    wow->FixParameter(6, fsrc->GetParameter(6));
    wow->FixParameter(7, fsrc->GetParameter(7));
    wow->FixParameter(8, fsrc->GetParameter(8));
    wow->FixParameter(9, fsrc->GetParameter(9));
    wow->FixParameter(12, fsrc->GetParameter(12));
    wow->FixParameter(13, fsrc->GetParameter(13));
    wow->FixParameter(14, fsrc->GetParameter(14));

    wow->SetLineColor(30);
    wow->SetLineStyle(3);
    //   wow->Draw("same");


    //Replot doppio exp
    TF1 *BG = new TF1("doppioexp", "[0]*([1]*TMath::Exp((-x/[2]))+  (1-[1])*TMath::Exp((-x/[3])))  ", Xmax * 0.3, Xmax * 1.6);
    BG->FixParameter(0, fsrc->GetParameter(0));
    BG->FixParameter(1, fsrc->GetParameter(1));
    BG->FixParameter(2, fsrc->GetParameter(2));
    BG->FixParameter(3, fsrc->GetParameter(3));

    BG->SetLineColor(2);
    BG->SetLineStyle(2);

    BG->Draw("same");


    //Replot Fermi-Dirac  (Compton Edge)
    TF1 *FD2 = new TF1("FDCompton", "[7]/(TMath::Exp((x-[8])*[9])+1) ", Xmax * 0.3, Xmax * 1.6);
    FD2->FixParameter(7, fsrc->GetParameter(7));
    FD2->FixParameter(8, fsrc->GetParameter(8));
    FD2->FixParameter(9, fsrc->GetParameter(9));

    FD2->SetLineColor(45);
    FD2->SetLineStyle(2);

    FD2->Draw("same");

    //Replot Fermi-Dirac Multipla  (Multiple Compton)
    TF1 *FD1 = new TF1("FDMultiplo", "[10]/([12]*TMath::Exp((x-[5])*[11])+1) ", Xmax * 0.3, Xmax * 1.6);

    FD1->FixParameter(10, fsrc->GetParameter(10));
    FD1->FixParameter(11, fsrc->GetParameter(11));
    FD1->FixParameter(12, fsrc->GetParameter(12));
    FD1->FixParameter(5, fsrc->GetParameter(5));

    FD1->SetLineColor(41);
    FD1->SetLineStyle(2);

    FD1->Draw("same");

    //Replot Gauss (Photon Peak)
    TF1 *G1 = new TF1("G1", " [4]/TMath::Exp((x-[5])*(x-[5])/(2*[6]*[6])) ", Xmax * 0.3, Xmax * 1.6);

    G1->FixParameter(4, fsrc->GetParameter(4));
    G1->FixParameter(5, fsrc->GetParameter(5));
    G1->FixParameter(6, fsrc->GetParameter(6));

    G1->SetLineColor(8);
    G1->SetLineStyle(2);

    G1->Draw("same");


    h1->SetTitle("Spettro del Cs137");
    h1->SetName("Risultati del Fit");
    h1->GetXaxis()->SetTitle("adc Counts");
    h1->GetYaxis()->SetTitle("Eventi");
    gPad->SetGrid();


    //Replot funzione con parametri trovata
    TF1 *fitmax = new TF1("fsrc", "[0]*([1]*TMath::Exp((-[2])*x)+  (1-[1])*TMath::Exp((-[3])*x))     + [4]/TMath::Exp((x-[5])*(x-[5])/(2*[6]*[6])) + [7]/(TMath::Exp((x-[8])*[9])+1)        +[10]/([12]*TMath::Exp((x-[5])*[11])+1)", 20, 60);

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

    // Disegna una linea sulla regione in cui è stato fatto il fit
    //  TF1 *fitregionF = new TF1("G1", " [0] ", Xmax * 0.3, Xmax * 1.6);
    //
    //    fitregionF->FixParameter(0, Ymax*1.5);
    //
    //    fitregionF->SetLineColor(3);
    //    fitregionF->SetLineStyle(2);
    //
    //    fitregionF->Draw("same");


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


    float comptonAMP = fsrc->GetParameter("FDCAmp");
    if (comptonAMP > h1->GetBinContent(intfitpoint)) {
        printf("Questo pesce è senza spalla. Da operare urgentemente. %f %d \n %s\n", h1->GetBinContent(intfitpoint), intfitpoint, ERROR_FISHERMAN);
        mypeak.anyproblems = NOT_COMPTON;
    }

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
