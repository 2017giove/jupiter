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
int GetMinimumBin(TH1D* hist, int from, int to);
void Cs_fit();
struct peak Cs_fit(TCanvas* c40, TH1D* h1, std::string savepath, mySetting* st, int PMTid);
void Cs_getPeak(char* src_name, int PMTid, char* wheretosave);
void PMTRangeLT(char * capturename, int PMTid);
void Cs_FitTot(char*src_name);
//void Cs_fit(char* src_name);

/**
 * Massimo boss
 */
int GetMaximumBin(TH1D* hist, int from, int to) {
    int i;
    int max = 0;
    int imax;
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
    int imin;
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


//Si abilita con 
//  su 
//  setenforce 0
// da terminale, poi si può acquisire dal sito http://lxsfera.roma1.infn.it

void Web_analyze(char* capturename) {
    char capturename_[STR_LENGTH];
    char temp1[STR_LENGTH];
    char temp2[STR_LENGTH];
    char rottentemp[STR_LENGTH];

    // Rimuove i file vecchi eventualmente presenti
    std::vector<std::string> myrottenfish = list_files("data/", capturename, ".RAW.root");
    removeFileList(myrottenfish);

    std::vector<std::string> myrottenhist = list_files("data/", capturename, ".histoweb.root");
    removeFileList(myrottenhist);



    // Cerca tutti i file appartenenti alla presa dati indicata
    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myfiles = list_files("data/", capturename, ".web.root");

    // Crea istogramma carica
    for (int i = 0; i < myfiles.size(); i++) {
        sprintf(capturename_, "data/%s", myfiles[i].c_str());
        printf("\n\n%d\t%d\t%s\n", i, myfiles.size(), myfiles[i].c_str());
        ChargeHist(capturename_, ".histoweb");
    }

    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myHistfiles = list_files("data/", capturename, ".histoweb.root");

    for (int i = 0; i < myHistfiles.size(); i++) {

        sprintf(temp1, "data/%s", myHistfiles[i].c_str());
        TFile *sorgente_file = TFile::Open(temp1);

        mySetting st;
        TTree* tset1 = (TTree*) sorgente_file->Get("tset");
        mySetting_get(tset1, &st);

        for (int j = 0; j < st.Nchan; j++) {
            int PMTid = CHtoPMT(j, &st);
            int voltage = st.voltage[j];
            sprintf(temp2, "data/%s_%d.calfish", capturename, PMTid);
            printf("\nFilename iniziale %s \n>> Salvato in %s\n", temp1, temp2);
            Cs_getPeak(temp1, PMTid, temp2);
        }


    }
    printf("Esecuzione terminata\n");
}

void Calibra_analyze(char* capturename) {
    char capturename_[STR_LENGTH];
    char temp1[STR_LENGTH];
    char temp2[STR_LENGTH];
    char rottentemp[STR_LENGTH];

    // Rimuove i file vecchi eventualmente presenti
    std::vector<std::string> myrottenfish = list_files("data/", capturename, ".calfish");
    removeFileList(myrottenfish);

    std::vector<std::string> myrottenhist = list_files("data/", capturename, ".histcal.root");
    removeFileList(myrottenhist);

    std::vector<std::string> myrottencal = list_files("data/", capturename, ".bestcal");
    removeFileList(myrottencal);

    std::vector<std::string> myrottenia = list_files("data/", capturename, ".bestcal.ia");
    removeFileList(myrottenia);



    // Cerca tutti i file appartenenti alla presa dati indicata
    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myfiles = list_files("data/", capturename, ".cal.root");

    // Crea istogramma carica
    for (int i = 0; i < myfiles.size(); i++) {
        sprintf(capturename_, "data/%s", myfiles[i].c_str());
        printf("\n\n%d\t%d\t%s\n", i, myfiles.size(), myfiles[i].c_str());
        ChargeHist(capturename_, ".histcal");
    }

    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myHistfiles = list_files("data/", capturename, ".histcal.root");

    for (int i = 0; i < myHistfiles.size(); i++) {

        sprintf(temp1, "data/%s", myHistfiles[i].c_str());
        TFile *sorgente_file = TFile::Open(temp1);

        mySetting st;
        TTree* tset1 = (TTree*) sorgente_file->Get("tset");
        mySetting_get(tset1, &st);

        for (int j = 0; j < st.Nchan; j++) {
            int PMTid = CHtoPMT(j, &st);
            int voltage = st.voltage[j];
            sprintf(temp2, "data/%s_%d.calfish", capturename, PMTid);
            printf("\nFilename iniziale %s \n>> Salvato in %s\n", temp1, temp2);
            Cs_getPeak(temp1, PMTid, temp2);
        }


    }

    // Sceglie il valore migliore del trigger per ogni PMT; ipotesi di linearità
    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myFish = list_files("data/", capturename, ".calfish");

    char tempf[STR_LENGTH];
    char tempf2[STR_LENGTH];
    for (int f = 0; f < myFish.size(); f++) {
        sprintf(tempf, "data/%s", myFish[f].c_str());
        sprintf(tempf2, "data/%s.bestcal", capturename);
        volt_fit(tempf, tempf2, capturename);
    }


}

void preCalibra_analyze(char* capturename) {

    char capturename_[STR_LENGTH];
    char temp1[STR_LENGTH];
    char temp2[STR_LENGTH];
    char rottentemp[STR_LENGTH];


    // Rimuove i file vecchi eventualmente presenti
    std::vector<std::string> myrottenfish = list_files("data/", capturename, ".fish");
    removeFileList(myrottenfish);

    std::vector<std::string> myrottenhist = list_files("data/", capturename, ".histprec.root");
    removeFileList(myrottenhist);

    std::vector<std::string> myrottentrigger = list_files("data/", capturename, ".besttrigger");
    removeFileList(myrottentrigger);

    std::vector<std::string> myrottenia = list_files("data/", capturename, ".besttrigger.ia");
    removeFileList(myrottenia);



    // Cerca tutti i file appartenenti alla presa dati indicata
    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myfiles = list_files("data/", capturename, ".th.root");

    // Crea istogramma carica
    for (int i = 0; i < myfiles.size(); i++) {
        sprintf(capturename_, "data/%s", myfiles[i].c_str());
        printf("\n\n%d\t%d\t%s\n", i, myfiles.size(), myfiles[i].c_str());
        ChargeHist(capturename_, ".histprec");
    }

    // Fit con CsFit (CESIO) >> Mette i valori del picco in un file
    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myHistfiles = list_files("data/", capturename, ".histprec.root");


    for (int i = 0; i < myHistfiles.size(); i++) {

        sprintf(temp1, "data/%s", myHistfiles[i].c_str());
        TFile *sorgente_file = TFile::Open(temp1);

        mySetting st;
        TTree* tset1 = (TTree*) sorgente_file->Get("tset");
        mySetting_get(tset1, &st);

        for (int j = 0; j < st.Nchan; j++) {
            int PMTid = CHtoPMT(j, &st);
            int voltage = st.voltage[j];
            sprintf(temp2, "data/%s_%d_%d.fish", capturename, voltage, PMTid);
            printf("\nFilename iniziale %s \n>> Salvato in %s\n", temp1, temp2);
            Cs_getPeak(temp1, PMTid, temp2);
        }



    }

    // Sceglie il valore migliore del trigger per ogni PMT; ipotesi di linearità
    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myFish = list_files("data/", capturename, ".fish");

    char tempf[STR_LENGTH];
    char tempf2[STR_LENGTH];
    for (int f = 0; f < myFish.size(); f++) {
        sprintf(tempf, "data/%s", myFish[f].c_str());
        sprintf(tempf2, "data/%s.besttrigger", capturename);
        bestresolution_find(tempf, tempf2);
    }

}

/**
 * [press a button an dream - don't even say a word]
 * Analizza automaticamente una serie di dati relativa all'acquisizione con nome capturenae
 * @param capturename
 */
void LolFit(char* capturename) {
    char capturename_[STR_LENGTH];
    char temp1[STR_LENGTH];
    char temp2[STR_LENGTH];
    char rottentemp[STR_LENGTH];


    // Rimuove i file vecchi eventualmente presenti
    std::vector<std::string> myrottenfish = list_files("data/", capturename, ".calfish");
    removeFileList(myrottenfish);

    std::vector<std::string> myrottenhist = list_files("data/", capturename, "0hist.root");
    removeFileList(myrottenhist);

    std::vector<std::string> myrottencal = list_files("data/", capturename, ".bestcal");
    removeFileList(myrottencal);


    // Cerca tutti i file appartenenti alla presa dati indicata
    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myfiles = list_files("data/", capturename, "0.root");

    // Crea istogramma carica
    for (int i = 0; i < myfiles.size(); i++) {
        sprintf(capturename_, "data/%s", myfiles[i].c_str());
        printf("\n\n%d\t%d\t%s\n", i, myfiles.size(), myfiles[i].c_str());
        ChargeHist(capturename_, "0hist");
    }


    // sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myHistfiles = list_files("data/", capturename, "0hist.root");

    for (int i = 0; i < myHistfiles.size(); i++) {

        sprintf(temp1, "data/%s", myHistfiles[i].c_str());
        TFile *sorgente_file = TFile::Open(temp1);

        mySetting st;
        TTree* tset1 = (TTree*) sorgente_file->Get("tset");
        mySetting_get(tset1, &st);

        for (int j = 0; j < st.Nchan; j++) {
            int PMTid = CHtoPMT(j, &st);
            int voltage = st.voltage[j];
            sprintf(temp2, "data/%s_%d.calfish", capturename, PMTid);
            printf("\nFilename iniziale %s \n>> Salvato in %s\n", temp1, temp2);
            Cs_getPeak(temp1, PMTid, temp2);
        }
        Cs_FitTot(temp1);

    }


    // Sceglie il valore migliore del trigger per ogni PMT; ipotesi di linearità
    //  sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myFish = list_files("data/", capturename, ".calfish");

    char tempf[STR_LENGTH];
    char tempf2[STR_LENGTH];
    for (int f = 0; f < myFish.size(); f++) {
        sprintf(tempf, "data/%s", myFish[f].c_str());
        sprintf(tempf2, "data/%s.bestcal", capturename);
        volt_fit(tempf, tempf2, capturename);
    }



}

void PMTRangeLT(char * capturename, int PMTid) {

    char capturename_[STR_LENGTH];
    char temp1[STR_LENGTH];
    char temp2[STR_LENGTH];
    char rottentemp[STR_LENGTH];


    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myFish = list_files("data/", capturename, "fish");

    //  gStyle->SetOptFit(1111);
    sprintf(rottentemp, "data/%s_%d.root", capturename, PMTid);
    TFile *FOut = new TFile(rottentemp, "UPDATE");
    TCanvas *c41 = new TCanvas("Fish", PLOTS_TITLE, 640, 480);

    c41->SetLogy();


    TGraph * confPlot = new TGraph();
    TGraph * confPlotProblematic = new TGraph();

    confPlot->GetYaxis()->SetRange(0, 200);
    confPlotProblematic->GetYaxis()->SetRange(0, 200);

    confPlotProblematic->SetMarkerColor(kRed);
    confPlot->SetMarkerStyle(8);
    confPlot->SetMarkerSize(1);
    confPlotProblematic->SetMarkerSize(1);
    confPlotProblematic->SetMarkerStyle(8);




    for (int i = 0; i < myFish.size(); i++) {
        sprintf(temp1, "data/%s", myFish[i].c_str());
        std::vector<peak> peaks = peak_load(temp1);

        for (int j = 0; j < peaks.size(); j++) {
            if (PMTid == peaks[j].PMTid) {
                TLatex * lbl = new TLatex(peaks[j].voltage + 0.2, -peaks[j].thresh + 0.2, Form("%1.5f", peaks[j].resolution));
                lbl->SetTextSize(0.025);
                lbl->SetTextFont(42);


                if (peaks[j].anyproblems == 0) {
                    confPlot->SetPoint((confPlot->GetN()), peaks[j].voltage, -peaks[j].thresh);

                } else {
                    confPlotProblematic->SetPoint((confPlotProblematic->GetN()), peaks[j].voltage, -peaks[j].thresh);
                    lbl->SetTextColor(kRed);
                }


                confPlot->GetListOfFunctions()->Add(lbl);
            }
        }
    }


    confPlot->Draw("ap");
    confPlotProblematic->Draw("psame");

    confPlot->Write("lollo");
    confPlotProblematic->Write("lollo");



    sprintf(rottentemp, "Risoluzione - PMT %d", PMTid);
    confPlot->SetTitle(rottentemp);
    confPlot->GetXaxis()->SetTitle("Tensione (V)");
    confPlot->GetYaxis()->SetTitle("Soglia Trigger (-mV)");

    sprintf(rottentemp, "img/%s_%d.eps", capturename, PMTid);
    c41->SaveAs(rottentemp);
    c41->Write();
    FOut->Close();


}

/**
 * Salva il picco trovato e la tensione di acquisizione, e i rispettivi errori, in una nuova riga del file specificato
 * @param nome file hist.root contenente l'istogramma
 * @param nome file in cui aggiungere la riga 
 */
void Cs_getPeak(char* src_name, int PMTid, char* wheretosave) {
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
            sprintf(tname, "img/%s_%d_%s.eps", filenameFromPath(src_name).c_str(), PMTid, timgnames[k]);
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
            printf("\nBOMB! Histofrish doesn't exist!!\n");
        }
    }

    TTree* newtree = tset1->CloneTree(0);
    newtree->Fill();
    newtree->Write("", TObject::kOverwrite);
    FOut->Close();


    delete c41;
}

void Cs_FitTot(char*src_name) {
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
        sprintf(tname, "img/%s_tot_csfit.eps", filenameFromPath(src_name).c_str());
        printf("Salva in %s\n", tname);
        mypeak = Cs_fit(c41, h1, tname, &st, 0);
        h1->Write();
    } else {
        printf("\nBOMB! Big Histofrish doesn't exist!!\n");
    }

    delete c41;
}


// non implementato
void Cs_fit(const char* src_name) {

    TFile *sorgente_file = TFile::Open(src_name);
   // TH1D *h1 = (TH1D*) sorgente_file->Get("h1");
    Cs_fit();
}

void Cs_fit() {
    TCanvas *c41 = new TCanvas();
    TFile* sorgente_file = (TFile*) gROOT->GetListOfFiles()->First();

    TIter next(sorgente_file->GetListOfKeys());
    TKey *key;
    TH1D *h1;

    char tname[STR_LENGTH];
    char tname2[STR_LENGTH];

    char fileOUT[STR_LENGTH];
    strcpy(fileOUT, appendToRootFilename(sorgente_file->GetName(), "csfit").c_str());
    TFile *FOut = new TFile(fileOUT, "UPDATE");

    gStyle->SetOptFit(1111);
    // gROOT->SetStyle("Plain");

    int i = 0;
    // esegue il fit per tutti gli istogrammi presenti nel file hPMT
    while ((key = (TKey*) next())) {


        TClass *cl = gROOT->GetClass(key->GetClassName());
        if (cl -> InheritsFrom("TH1D")) {
            h1 = (TH1D*) key->ReadObj();
            std::string myname = h1->GetName();
            std::string pmtname = myname.substr(myname.length() - 3);
            int PMTid = atoi(pmtname.c_str());
            sprintf(tname, "h%d", PMTid);
            sprintf(tname2, "hc%d", PMTid);

            if ((!myname.compare(tname))&&(!myname.compare(tname2))) {
                //  TCanvas *c41 = new TCanvas();
                printf("\n\n********************************\n");
                printf("Fit per il PMT %d\n", PMTid);

                sprintf(tname, "img/%s_%s.eps", filenameFromPath(sorgente_file->GetName()).c_str(), myname.c_str());
                mySetting st;
                TTree* tset1 = (TTree*) sorgente_file->Get("tset");
                mySetting_get(tset1, &st);

                Cs_fit(c41, h1, tname, &st, PMTid);
                h1->SetName(myname.c_str());
                h1->Write();
            }
        }

    }


    sprintf(tname, "%s", sorgente_file->GetName());
    Cs_FitTot(tname);

    delete c41;
    FOut->Close();

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
    TF1 *BG = new TF1("doppioexp", "[0]*([1]*TMath::Exp((-x/[2]))+  (1-[1])*TMath::Exp((-x/[3])))  ", 0, 200);
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

    if (fsrc->GetProb() < 0.0001) {
        mypeak.anyproblems = NOT_PROBABLE;
        printf("Questo pesce è una burla. Non ci sto credendo che hai pescato un elefante. Questo essere va rigettato in acqua. E' un granchio?\n");
    }

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
