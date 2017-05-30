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
struct peak Cs_fit(TH1D* h1, std::string savepath, mySetting* st, int PMTid);
void Cs_getPeak(char* src_name, int PMTid, char* wheretosave);

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
            //  printf("%d\t%d\n", imin, currentmin);
        }

    }
    return imin;

}

/**
 * Massimi locali
 */
std::vector<int> GetMaximumBins(TH1D* hist, int from, int to) {
    std::vector<float> myMins;
    std::vector<int> myMinsX;

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


    int windowSize = (50. * 0.1 / hist->GetXaxis()->GetBinWidth(0));
    printf("%f\n\n", windowSize);
    //   if (windowSize==0) windowSize =1;

    for (i = from; i < to; i++) {
        nsumR = 0;
        nearmeR[i] = 0;
        for (int j = i; j < i + windowSize; j++) {
            if (j >= from && j < to && j != i) {
                nearmeR[i] += hist->GetBinContent(j)*1.1;
            }
            nsumR++;
        }
        nearmeR[i] /= (float) nsumR;

        nsumL = 0;
        nearmeL[i] = 0;

        for (int j = i - windowSize; j < i; j++) {
            if (j >= from && j < to && j != i) {
                nearmeL[i] += hist->GetBinContent(j)*1.1;
            }
            nsumL++;
        }
        nearmeL[i] /= (float) nsumL;
        //  printf("nearme %f\t%f\n", hist->GetBinContent(i), nearmeR[i]);
    }

    bool isabove = 0;

    float cmax = 0;
    int cpos = 0;

    for (i = from; i < to; i++) {
        if (isabove == 0) {//stava sotto soglia

            if (hist->GetBinContent(i) > media && hist->GetBinContent(i) > nearmeR[i] && hist->GetBinContent(i) > nearmeL[i]) { //è andato sopra soglia
                isabove = 1;
                cmax = hist->GetBinContent(i);
                cpos = i;
            }

        } else { //sta sopra soglia
            if (hist->GetBinContent(i) < media && hist->GetBinContent(i) <= nearmeL[i]) { //è sceso sotto soglia

                isabove = 0;
                //                myMins.push_back(cmax);
                //                myMinsX.push_back(cpos);
                //                printf("csono %d\t%d\t%f\t%f\t%f\n", i,cpos, cmax, nearmeL[cpos], nearmeR[cpos]);
                //

            } else {
                if (hist->GetBinContent(i) > cmax) {
                    cmax = hist->GetBinContent(i);
                    cpos = i;
                }

                if (hist->GetBinContent(i) > hist->GetBinContent(i - 1) && hist->GetBinContent(i) >= hist->GetBinContent(i + 1) && hist->GetBinContent(i) > nearmeR[i] && hist->GetBinContent(i) > media) { //condizione ulteriore per massimi, non serve che scenda sempre sotto soglia...


                    myMins.push_back(cmax);
                    myMinsX.push_back(cpos);
                    printf("csono %d\t%d\t%f\t%f\t%f\n", i, cpos, cmax, nearmeL[cpos], nearmeR[cpos]);
                }
            }
        }
    }


    printf("media %f\n", media);
    for (i = 0; i < myMins.size(); i++) {

        printf("sono %d\t%d\n", myMinsX[i], myMins[i]);

    }
    return myMinsX;

}





//void Calibra_analyze(char* capturename) {
//
//    char capturename_[STR_LENGTH];
//    char temp1[STR_LENGTH];
//    char temp2[STR_LENGTH];
//    char rottentemp[STR_LENGTH];
//    sprintf(capturename_, "%s_", capturename);
//    std::vector<std::string> myfiles = list_files("data/", capturename, "0.root");
//
//
//    std::vector<std::string> myrottenfish = list_files("data/", capturename, ".fish");
//    removeFilesbyExt(myrottenfish);
//
//    std::vector<std::string> myrottenexpert = list_files("data/", capturename, ".expert");
//    removeFilesbyExt(myrottenexpert);
//
//
//    for (int i = 0; i < myfiles.size(); i++) {
//
//        sprintf(capturename_, "data/%s", myfiles[i].c_str());
//        printf("\n\n%d\t%d\t%s\n", i, myfiles.size(), myfiles[i].c_str());
//        ChargeHist(capturename_);
//    }
//
//
//    sprintf(capturename_, "%s_", capturename);
//    std::vector<std::string> myHistfiles = list_files("data/", capturename, "hist.root");
//
//    for (int i = 0; i < myHistfiles.size(); i++) {
//        printf("Bogoliubov\n\n\n");
//        int voltage;
//        int trigger;
//
//        std::string fname = capturename_;
//        std::string vlt = myHistfiles[i].substr(fname.length(), 4);
//        std::string trg = myHistfiles[i].substr(fname.length() + 5, 2);
//        voltage = atoi(vlt.c_str());
//        trigger = atoi(trg.c_str());
//        printf("Bogolobov\n\n\n");
//
//
//        sprintf(temp1, "data/%s", myHistfiles[i].c_str());
//        TFile *sorgente_file = TFile::Open(temp1);
//
//        mySetting st;
//        TTree* tset1 = (TTree*) sorgente_file->Get("tset");
//        mySetting_get(tset1, &st);
//
//        for (int j = 0; j < st.Nchan; j++) {
//            int PMTid = CHtoPMT(j, &st);
//            sprintf(temp2, "data/%s_%d_%d.fish", capturename, voltage, PMTid);
//            printf("\ntemp1 %s \ntemp2%s\n", temp1, temp2);
//            Cs_getPeak(temp1, PMTid, temp2);
//
//        }
//    }
//
//
//    sprintf(capturename_, "%s_", capturename);
//    std::vector<std::string> myFish = list_files("data/", capturename, ".fish");
//    char tempf[STR_LENGTH];
//    for (int f = 0; f < myFish.size(); f++) {
//        sprintf(tempf, "data/%s", myFish[f].c_str());
//        trigger_fit(tempf);
//    }
//
//
//    sprintf(capturename_, "%s_", capturename);
//    std::vector<std::string> myExpert = list_files("data/", capturename, ".expert");
//    char tempe[STR_LENGTH];
//    for (int e = 0; e < myExpert.size(); e++) {
//        sprintf(tempe, "data/%s", myExpert[e].c_str());
//        volt_fit(tempe);
//    }
//
//}

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
            sprintf(temp2, "data/%s_%d.calfish", capturename,  PMTid);
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
 * Salva il picco trovato e la tensione di acquisizione, e i rispettivi errori, in una nuova riga del file specificato
 * @param nome file hist.root contenente l'istogramma
 * @param nome file in cui aggiungere la riga 
 */
void Cs_getPeak(char* src_name, int PMTid, char* wheretosave) {
    TFile *sorgente_file = TFile::Open(src_name);
    //  TCanvas *c41 = new TCanvas();
    peak mypeak;
    mySetting st;

    TTree* tset1 = (TTree*) sorgente_file->Get("tset");
    mySetting_get(tset1, &st);
    mySetting_print(&st);

    int CH = PMTtoCH(PMTid, &st);

    char tname[STR_LENGTH];
    sprintf(tname, "h%d", PMTid);
    TH1D *h1 = (TH1D*) sorgente_file->Get(tname);


    sprintf(tname, "img/%s_%d_csfit.eps", filenameFromPath(src_name).c_str(), PMTid);
    printf("Salva in %s\n", tname);


    mypeak = Cs_fit(h1, tname, &st, PMTid);
    mypeak.PMTid = PMTid;
    mypeak.voltage = st.voltage[CH];
    mypeak.thresh = st.thresh[CH];
    mypeak.resolution = mypeak.sigma / mypeak.peakpos;
    peak_save(wheretosave, &mypeak);

}

// non implementato
//void Cs_fit(char* src_name) {
//
//    TFile *sorgente_file = TFile::Open(src_name);
//    TH1D *h1 = (TH1D*) sorgente_file->Get("h1");
//    Cs_fit(h1);
//}

void Cs_fit() {
    TFile* sorgente_file = (TFile*) gROOT->GetListOfFiles()->First();

    TIter next(sorgente_file->GetListOfKeys());
    TKey *key;
    TH1D *h1;

    char tname[STR_LENGTH];

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
            if (!myname.compare(tname)) {
                //  TCanvas *c41 = new TCanvas();
                printf("\n\n********************************\n");
                printf("Fit per il PMT %d\n", PMTid);

                sprintf(tname, "img/%s_csfit%d.eps", filenameFromPath(sorgente_file->GetName()).c_str(), PMTid);
                mySetting st;
                TTree* tset1 = (TTree*) sorgente_file->Get("tset");
                mySetting_get(tset1, &st);
                //  mySetting_print(&st);


                Cs_fit(h1, tname, &st, PMTid);
                h1->Write();
            }
        }

    }

    // FOut->Close();

}

struct peak Cs_fit(TH1D* h1, std::string savepath, mySetting* st, int PMTid) {

    TCanvas *c40 = new TCanvas();



    c40->SetFillColor(0);

    h1->Rebin(5);

    int nBins = h1->GetSize() - 2;
    float step = (float) h1->GetXaxis()->GetBinWidth(0); //invece di usare QMAX/nBins conviene usare GetBinWidth
    printf("step %f\n\n", step);
    //int maxBin = GetMaximumBin(h1, 5. / step, nBins);
    std::vector<int> mymaxsbins = GetMaximumBins(h1, 5. / step, nBins);
    int maxBin = mymaxsbins.back();

    float Xmax = maxBin*step; //80
    // printf("\nxmax=%f\n", Xmax);

    float Xwindow = 3.8; // larghezza su cui eseguire a occhio il fit gaussiano rispetto a xmax rilevato
    float Ymax = h1->GetBinContent(maxBin);


    //gROOT->SetStyle("Plain");
    //    gStyle->SetOptFit(1111);


    struct peak mypeak;
    mypeak.anyproblems = 0;



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



    //punto di partenza del fit
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
    fsrc->SetParLimits(12, 0.01, 10000);
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

    h1->Fit("fsrc", "Lq", "", startfitpoint, Xmax * 2); // prima la FDCompton * 2 / 3 //vL options
    h1->Draw();


    //Quando la seconda spalla è più alta della prima
    if (fsrc->GetParameter(7) < fsrc->GetParameter(10)) {
        printf("Il fit è tildato\n");

        //        float oldFDc = fsrc->GetParameter(7);
        //        fsrc->SetParameter(7, fsrc->GetParameter(10));
        //        fsrc->SetParameter(10, oldFDc);
        //        h1->Fit("fsrc", "q", "", startfitpoint, Xmax * 2);
    }


    //   h1->Draw("same");
    //   fsrc->Draw("same");

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
    TF1 *FD1 = new TF1("FDMultiplo", "[12]/(TMath::Exp((x-[5])*[13]*[14])+1) ", Xmax * 0.3, Xmax * 1.6);

    FD1->FixParameter(12, fsrc->GetParameter(10));
    FD1->FixParameter(13, fsrc->GetParameter(11));
    FD1->FixParameter(14, fsrc->GetParameter(12));
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

    c40->SaveAs(savepath.c_str());

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
    mypeak.peakvalue = fitmax->Eval(mypeak.peakpos);
    float nTOT = h1->Integral(h1->GetXaxis()->FindBin(mypeak.peakpos - mypeak.sigma), h1->GetXaxis()->FindBin(mypeak.peakpos + mypeak.sigma));
    mypeak.nSGN = fitmax ->Integral(mypeak.peakpos - mypeak.sigma, mypeak.peakpos + mypeak.sigma);
    mypeak.nBG = nTOT - fitmax ->Integral(mypeak.peakpos - mypeak.sigma, mypeak.peakpos + mypeak.sigma);

    float resolution = mypeak.sigma / mypeak.peakpos;
    printf("RISOLUZIONE = %f\n", resolution);


    float comptonAMP = fsrc->GetParameter("FDCAmp");
    if (comptonAMP > h1->GetBinContent(intfitpoint)) {
        printf("Questo pesce è senza spalla. Da operare urgentemente. %f %d \n %s\n", h1->GetBinContent(intfitpoint), intfitpoint, ERROR_FISHERMAN);
        mypeak.anyproblems = NOT_COMPTON;
    }


    if (resolution < 0.01) {
        mypeak.anyproblems = NOT_CREDIBLE;
        printf("Questo pesce va rigettato in acqua. E' un granchio.\n");
    }

    // printf("santaprob: %f\n", fsrc->GetProb());
    if (fsrc->GetProb() < 0.0001) {
        mypeak.anyproblems = NOT_PROBABLE;
        printf("Questo pesce è una burla. Non ci sto credendo che hai pescato un elefante. Questo essere va rigettato in acqua. E' un granchio?\n");
    }


    if (mypeak.anyproblems != 0) {
        printf("col\n");
        c40->SetFillColor(46);
        h1->SetFillColor(46);
        c40->Draw();
    }


    mySetting_histoprint(st, PMTid);

    return mypeak;

}

#endif