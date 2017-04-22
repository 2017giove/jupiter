/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Do not modify it unless you know exactly what you are doing.
 * Otherwise you'll be in trouble.
 * 
 * It's Boguliubov!
 * 
 * Original code written by DeathStar, Sferici 2016
 */

#ifndef JUPITER
#include "WaveAnalysis.h"
#endif

//Questa macro fitta l'istogramma sorgente+fondo a partire da un fit del fondo e infine plotta il fit della sorgente e basta
int GetMaximumBin(TH1D* hist, int from, int to);
void Cs_fit();
peak* Cs_fit(TH1D* h1);
void Cs_fit(char* src_name);

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

void Cs_getPeak(char* src_name, char* wheretosave) {
    TFile *sorgente_file = TFile::Open(src_name);
    TH1D *h1 = (TH1D*) sorgente_file->Get("h1");

    float voltage;
    peak mypeak;

    TTree* tset1 = (TTree*) sorgente_file->Get("tset");
    tset1->SetBranchAddress("Voltage", &voltage);
    tset1->GetEntry(0);

    mypeak = Cs_fit(h1)[0];

    std::ofstream savefile(wheretosave, std::ios_base::app);
    savefile << voltage << " " << mypeak.peakpos << " " << 0 << " " << mypeak.err_peakpos << " " << 0 << std::endl;
}

void Cs_fit(char* src_name) {

    TFile *sorgente_file = TFile::Open(src_name);
    TH1D *h1 = (TH1D*) sorgente_file->Get("h1");
    Cs_fit(h1);
}

void Cs_fit() {
    TFile* sorgente_file = (TFile*) gROOT->GetListOfFiles()->First();
    TH1D *h1 = (TH1D*) sorgente_file->Get("h1");
    Cs_fit(h1);
}

peak* Cs_fit(TH1D* h1) {

    int nBins = h1->GetSize() - 2;
    float step = (float) QMAX / nBins;

    int maxBin = GetMaximumBin(h1, 5. / step, nBins);
    float Xmax = maxBin*step;
    float Xwindow = 3.8; // larghezza su cui eseguire il fit gaussiano rispetto a xmax rilevato
    float Ymax = h1->GetBinContent(maxBin);

    TCanvas *c41 = new TCanvas();
    gROOT->SetStyle("Plain");
    gStyle->SetOptFit(1111);

    TF1 *Gauss = new TF1("Gauss", "[0]*TMath::Exp(-(x-[1])*(x-[1])/(2*[2]*[2]))", Xmax - Xwindow, Xmax + Xwindow);
    Gauss->SetParNames("Amplitude", "PeakPos", "sigma");

    Gauss->SetParLimits(0, 0.8 * Ymax, 1.1 * Ymax);
    Gauss->SetParLimits(1, Xmax * 0.9, Xmax * 1.1);
    Gauss->SetParLimits(2, 0, Xwindow);

    Gauss->SetParameter(0, Ymax);
    Gauss->SetParameter(1, Xmax);
    Gauss->SetParameter(2, Xwindow / 2);

    h1->Fit("Gauss", "N", "", Xmax - Xwindow, Xmax + Xwindow);

    Ymax = Gauss->GetParameter(0);
    Xmax = Gauss->GetParameter(1);
    float sigma = Gauss->GetParameter(2);


    h1->GetXaxis()->SetRange(Xmax * 0.3 / step, Xmax * 1.5 / step);

    TF1 *fsrc = new TF1("fsrc", "[0]*([1]*TMath::Exp((-[2])*x)+  (1-[1])*TMath::Exp((-[3])*x))     + [4]/TMath::Exp((x-[5])*(x-[5])/(2*[6]*[6])) + [7]/(TMath::Exp((x-[8])*[9])+1)        +[10]/([12]*TMath::Exp((x-[5])*[11])+1)", 20, 60);

    float FDCompton = Xmax * (1 - 1 / (1 + 2 * ENERGY_CESIO / MASS_ELECTRON));


    //                   0        1           2       3           4           5       6       7       8               9
    fsrc->SetParNames("BGAmp", "BGratio", "tau_1", "tau_2", "GaussAmp", "Peak", "sigma", "FDCAmp", "FDCShift", "FDCBeta");
    fsrc->SetParName(10, "FD2Amp");
    fsrc->SetParName(11, "FD2Beta");
    fsrc->SetParName(12, "FD2Modulation");


    fsrc->SetParLimits(0, 0, 10000000);
    fsrc->SetParLimits(1, 0, 1); //OK
    //     fsrc->SetParLimits(2, 0.9 * p1, 1.1 * p1);
    //      fsrc->SetParLimits(3, 0.9 * p2, 1.1 * p2);
    fsrc->SetParLimits(4, Ymax * 0.8, Ymax * 1.2); //OK!
    fsrc->SetParLimits(5, Xmax * 0.9, Xmax * 1.1);
    fsrc->SetParLimits(7, 0, 10000000);
    fsrc->SetParLimits(8, FDCompton * 0.9, FDCompton * 1.1);
    fsrc->SetParLimits(10, 0, 1000);
    fsrc->SetParLimits(11, 0, 1);

    //
    ////    Parametri fissati dal fit del rumore
    // fsrc->FixParameter(1, p3);
    // fsrc->SetParameter(2, p1);
    // fsrc->SetParameter(3, p2);
    fsrc->SetParameter(4, Ymax);
    fsrc->SetParameter(5, Xmax);
    fsrc->SetParameter(6, sigma);
    fsrc->SetParameter(7, Ymax / 3);
    fsrc->SetParameter(8, FDCompton);
    fsrc->SetParameter(9, sigma * 0.5296);
    fsrc->SetParameter(10, 100);
    fsrc->SetParameter(12, 0.95);


    //  h1->Fit("fsrc", "", "", 20, 60);
    h1->Fit("fsrc", "", "", FDCompton * 2 / 3, Xmax * 2);
    h1->Draw();





    //Replot senza doppio esponenziale
    TF1 *wow = new TF1("wow", "[4]/TMath::Exp((x-[5])*(x-[5])/(2*[6]*[6])) + [7]/(TMath::Exp((x-[8])*[9])+1)   +[12]/(TMath::Exp((x-[5])*[13])*[14]+1)  ", Xmax * 0.3, Xmax * 1.6);

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

    wow->Draw("same");


    //Replot doppio exp

    TF1 *BG = new TF1("BG", "[0]*([1]*TMath::Exp((-[2])*x)+(1-[1])*TMath::Exp((-[3])*x)) ", Xmax * 0.35, Xmax * 1.6);
    BG->FixParameter(0, fsrc->GetParameter(0));
    BG->FixParameter(1, fsrc->GetParameter(1));
    BG->FixParameter(2, fsrc->GetParameter(2));
    BG->FixParameter(3, fsrc->GetParameter(3));

    BG->SetLineColor(40);
    BG->SetLineStyle(2);

    BG->Draw("same");





    //Replot Fermi-Dirac2  (Compton Edge)

    TF1 *FD2 = new TF1("FD2", "[7]/(TMath::Exp((x-[8])*[9])+1) ", Xmax * 0.3, Xmax * 1.6);


    FD2->FixParameter(7, fsrc->GetParameter(7));
    FD2->FixParameter(8, fsrc->GetParameter(8));
    FD2->FixParameter(9, fsrc->GetParameter(9));

    FD2->SetLineColor(46);
    FD2->SetLineStyle(2);

    FD2->Draw("same");

    //Replot Fermi-Dirac1  (Multiple Compton)

    TF1 *FD1 = new TF1("FD1", "[12]/(TMath::Exp((x-[5])*[13]*[14])+1) ", Xmax * 0.3, Xmax * 1.6);

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






    peak peaks [10];
    peak mypeak;
    mypeak.resolution = fsrc->GetParameter("Peak") / fsrc->GetParameter("sigma");
    mypeak.peakpos = fsrc->GetParameter("Peak");

    peaks[0] = mypeak;

    printf("RISOLUZIONE = %f\n", peaks[0].resolution);
    return peaks;

}




