/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Do not modify it unless you know exactly what you are doing.
 * Otherwise you'll be in trouble.
 * 
 * It's Boguliubov!
 * 
 * Original code written by DeathStar, Sferici 2016
 */

#include "WaveAnalysis.h"
#include "defines.h"
#include <stdlib.h>
#include <stdio.h> 
#include <iostream>
#include <ostream>
#include "defines.h"

//Questa macro fitta l'istogramma sorgente+fondo a partire da un fit del fondo e infine plotta il fit della sorgente e basta

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

void Cs_fit(TH1F* bg_file, TH1F* src_file) {

}

void Cs_fit(char* bg_file, char* src_name, float MaxGauss) {

    TFile *sorgente_file = TFile::Open(src_name);
    TH1D *h1 = (TH1D*) sorgente_file->Get("h1");

    int nBins = h1->GetSize() - 2;
    float step = (float) QMAX / nBins;

    int maxBin = GetMaximumBin(h1, 5. / step, nBins);
    printf("Lorenzo ha detto %d\n", maxBin);
    float Xmax = maxBin*step;

    float FDCompton = Xmax * (1 - 1 / (1 + 2 * ENERGY_CESIO / MASS_ELECTRON));


    //    h1->GetXaxis()->SetRange((MaxGauss - 10) / step, (MaxGauss + 10) / step);

    //fit della sorgente

    // h1->GetXaxis()->SetRange((MaxGauss - 10) / step, (MaxGauss + 10) / step);

    TF1 *Gauss = new TF1("Gauss", "[0]/TMath::Exp((x-[1])*(x-[1])/(2*[2]*[2]))", Xmax - 4, Xmax + 4);
    Gauss->SetParNames("Amplitude", "PeakPos", "sigma");

    Gauss->SetParLimits(0, 0, 1.1 * h1->GetBinContent(maxBin));
    Gauss->SetParLimits(1, Xmax * 0.9, Xmax * 1.1);
    Gauss->SetParLimits(2, 0, 20);

    Gauss->SetParameter(0, 100);
    Gauss->SetParameter(1, Xmax);
    Gauss->SetParameter(2, 1);

    h1->Fit("Gauss", "N", "", Xmax - 4, Xmax + 4);
    Xmax = Gauss->GetParameter(1);
    float Ymax = h1->GetBinContent(maxBin);
    float sigma = Gauss->GetParameter(2);




    h1->GetXaxis()->SetRange(Xmax * 0.3 / step, Xmax * 1.5 / step);

    TF1 *fsrc = new TF1("fsrc", "[0]*([1]*TMath::Exp((-[2])*x)+  (1-[1])*TMath::Exp((-[3])*x))     + [4]/TMath::Exp((x-[5])*(x-[5])/(2*[6]*[6])) + [7]/(TMath::Exp((x-[8])*[9])+1)        +[10]/([12]*TMath::Exp((x-[5])*[11])+1)", 20, 60);



    //                   0        1           2       3           4           5       6       7       8               9
    fsrc->SetParNames("BGAmp", "BGratio", "tau_1", "tau_2", "GaussAmp", "Peak", "sigma", "FD1Amp", "FD1Shift", "FD1Beta");
    fsrc->SetParName(10, "FD2Amp");
    fsrc->SetParName(11, "FD2Beta");
    fsrc->SetParName(12, "FD2Modulation");


    fsrc->SetParLimits(0, 0, 10000000);
    fsrc->SetParLimits(1, 0, 1); //OK
    //     fsrc->SetParLimits(2, 0.9 * p1, 1.1 * p1);
    //      fsrc->SetParLimits(3, 0.9 * p2, 1.1 * p2);
    fsrc->SetParLimits(4, 0, 20000); //OK?
    fsrc->SetParLimits(5, Xmax * 0.9, Xmax * 1.1);
    fsrc->SetParLimits(7, 0, 10000000);
    fsrc->SetParLimits(8, FDCompton * 0.9, FDCompton * 1.1);
    printf("Lorenzo ha calcolato %f\n", FDCompton);
    fsrc->SetParLimits(10, 0, 1000);

    fsrc->SetParLimits(11, 0, 1);

    //
    ////    Parametri fissati dal fit del rumore
    //        fsrc->FixParameter(1, p3);
    //        fsrc->SetParameter(2, p1);
    //        fsrc->SetParameter(3, p2);
    fsrc->SetParameter(4, 30);
    fsrc->SetParameter(5, Xmax);
    fsrc->SetParameter(6, sigma);
    fsrc->SetParameter(7, 100);
    fsrc->SetParameter(8, FDCompton);
    fsrc->SetParameter(9, sigma * 0.5296);
    fsrc->SetParameter(10, 100);
    fsrc->SetParameter(12, 0.95);


    //    printf("%d\t%f\n", maxBin, Xmax);
    TCanvas *c41 = new TCanvas();
    h1->Fit("fsrc", "", "", 20, 60);
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







    gROOT->SetStyle("Plain");
    gStyle->SetOptFit(1111);
    h1->SetTitle("Spettro del Cs137");
    h1->SetName("Risultati del Fit");
    h1->GetXaxis()->SetTitle("adc Counts");
    h1->GetYaxis()->SetTitle("Eventi");
    gPad->SetGrid();


}

void Cs_fitLOLLATA(char* bg_file, char* src_file, float MaxGauss) {

    //fit del fondo

    TFile *f = TFile::Open(bg_file);
    TH1D* h1 = (TH1D*) f->Get("h1");
    int nBins = h1->GetSize() - 2;
    printf("%d\n", nBins);
    float step = (float) QMAX / nBins;
    int bin1 = 10;
    h1->GetXaxis()->SetRange(bin1, nBins);
    int maxBin = h1->GetMaximumBin();
    int begin = maxBin*step;
    begin = 20;
    double ratio;
    printf("Lorenzo conta %d\n", nBins);
    TF1 *fBg = new TF1("fBackground", "[0]*([3]*TMath::Exp((-[1])*x)+       (1-[3])*TMath::Exp((-[2])*x))", begin, QMAX);
    //TF1 *fBg = new TF1("fBackground", "[0]*TMath::Exp((-[1])*(x))",maxBin*step, QMAX);
    fBg->SetParNames("Amplitude", "tau_1", "tau_2", "ratio");
    //fBg->SetParNames("Amplitude", "tau_1");
    fBg->SetParameter(0, h1->GetMaximum());
    fBg->SetParameter(1, 0.01);
    fBg->SetParameter(2, 0.07);
    //fBg->FixParameter(2, 0);
    fBg->SetParameter(3, 0.06);

    fBg->SetParLimits(0, 0, 10000);
    fBg->SetParLimits(1, 0, 10);
    fBg->SetParLimits(2, 0, 10);
    fBg->SetParLimits(3, 0, 1);

    h1->Fit("fBackground", "N", "", begin, QMAX);

    float p1 = fBg->GetParameter(1);
    float p2 = fBg->GetParameter(2);
    float p3 = fBg->GetParameter(3);

    //fit preliminare del picco		

    TFile *g = TFile::Open(src_file);
    h1 = (TH1D*) g->Get("h1");
    nBins = h1->GetSize() - 2;
    step = (float) QMAX / nBins;
    h1->GetXaxis()->SetRange((MaxGauss - 10) / step, (MaxGauss + 10) / step);
    maxBin = h1->GetMaximumBin();
    float Xmax = maxBin*step;
    printf("%d\t%f\n", maxBin, Xmax);


    TF1 *Gauss = new TF1("Gauss", "[0]/TMath::Exp((x-[1])*(x-[1])/(2*[2]*[2]))", Xmax - 4, Xmax + 4);
    Gauss->SetParNames("Amplitude", "PeakPos", "sigma");

    Gauss->SetParLimits(0, 0, 1.1 * h1->GetBinContent(maxBin));
    Gauss->SetParLimits(1, Xmax * 0.9, Xmax * 1.1);
    Gauss->SetParLimits(2, 0, 20);

    Gauss->SetParameter(0, 100);
    Gauss->SetParameter(1, Xmax);
    Gauss->SetParameter(2, 1);

    h1->Fit("Gauss", "N", "", Xmax - 4, Xmax + 4);
    Xmax = Gauss->GetParameter(1);
    float Ymax = h1->GetBinContent(maxBin);
    float FDCompton = Xmax * (1 - 1 / (1 + 2 * 663 / 511.));
    float sigma = Gauss->GetParameter(2);
    printf("Xmax=%f\tFDCompton%f\n", Xmax, FDCompton);



    //fit della sorgente
    h1->GetXaxis()->SetRange(Xmax * 0.3 / step, Xmax * 1.5 / step);

    TF1 *fsrc = new TF1("fsrc", "[0]*([1]*TMath::Exp((-[2])*x)+(1-[1])*TMath::Exp((-[3])*x)) + [4]/TMath::Exp((x-[5])*(x-[5])/(2*[6]*[6])) + [7]/(TMath::Exp((x-[8])*[9])+1) +[12]/([14]*TMath::Exp((x-[5])*[13])+1)", Xmax * 0.3, QMAX);



    fsrc->SetParNames("BGAmp", "BGratio", "tau_1", "tau_2", "GaussAmp", "Peak", "sigma", "FD1Amp", "FD1Shift", "FD1Beta");

    //fsrc->SetParName(10, "ExpAmp");
    //fsrc->SetParName(11, "ExpTau");
    fsrc->SetParName(12, "FD2Amp");
    fsrc->SetParName(13, "FD2Beta");




    fsrc->SetParLimits(0, 0, 10000000);
    fsrc->SetParLimits(1, 0, 1); //OK
    fsrc->SetParLimits(2, 0.9 * p1, 1.1 * p1);
    fsrc->SetParLimits(3, 0.9 * p2, 1.1 * p2);
    fsrc->SetParLimits(4, 0, 20000); //OK?
    fsrc->SetParLimits(5, Xmax * 0.9, Xmax * 1.1);

    fsrc->SetParLimits(7, 0, 10000000);
    fsrc->SetParLimits(8, FDCompton * 0.9, FDCompton * 1.1);
    //   fsrc->SetParLimits(10, 0, 10000);
    //   fsrc->SetParLimits(11, 0, 5);

    fsrc->SetParLimits(12, 0, 200);

    fsrc->SetParLimits(13, 0, 1);


    //Parametri fissati dal fit del rumore
    fsrc->FixParameter(1, p3);
    fsrc->SetParameter(2, p1);
    fsrc->SetParameter(3, p2);


    fsrc->SetParameter(4, 30);
    fsrc->SetParameter(5, Xmax);
    fsrc->SetParameter(6, sigma);
    fsrc->SetParameter(7, 100);
    fsrc->SetParameter(8, FDCompton);
    fsrc->SetParameter(9, sigma * 0.5296);
    // fsrc->SetParameter(10, 2);
    //  fsrc->SetParameter(11, 3.21876e-01);
    fsrc->SetParameter(12, 100);

    fsrc->SetParameter(14, 0.95);

    //   fsrc->FixParameter(10, 800);
    //   fsrc->FixParameter(11, 0.1);

    TCanvas *c41 = new TCanvas();
    // h1->Fit("fsrc", "", "", Xmax * 0.3, Xmax * 1.6);
    h1->Fit("fsrc", "", "", 20, 60);
    h1->Draw();

    gROOT->SetStyle("Plain");
    gStyle->SetOptFit(1111);
    h1->SetTitle("Spettro del Cs137");
    h1->SetName("Risultati del Fit");
    h1->GetXaxis()->SetTitle("adc Counts");
    h1->GetYaxis()->SetTitle("Eventi");
    gPad->SetGrid();

    //Replot senza doppio esponenziale
    TF1 *wow = new TF1("wow", "[4]/TMath::Exp((x-[5])*(x-[5])/(2*[6]*[6])) + [7]/(TMath::Exp((x-[8])*[9])+1)+[10]*TMath::Exp((-[11])*x)+[12]/(TMath::Exp((x-[5])*[13])*[14]+1)  ", Xmax * 0.3, Xmax * 1.6);

    wow->FixParameter(4, fsrc->GetParameter(4));
    wow->FixParameter(5, fsrc->GetParameter(5));
    wow->FixParameter(6, fsrc->GetParameter(6));
    wow->FixParameter(7, fsrc->GetParameter(7));
    wow->FixParameter(8, fsrc->GetParameter(8));
    wow->FixParameter(9, fsrc->GetParameter(9));
    wow->FixParameter(10, fsrc->GetParameter(10));
    wow->FixParameter(11, fsrc->GetParameter(11));
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

    FD1->FixParameter(12, fsrc->GetParameter(12));
    FD1->FixParameter(13, fsrc->GetParameter(13));
    FD1->FixParameter(14, fsrc->GetParameter(14));
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

    //Replot exp (beta)

    TF1 *E = new TF1("G1", " [10]*TMath::Exp((-[11])*x) ", Xmax * 0.3, Xmax * 1.6);

    E->FixParameter(10, fsrc->GetParameter(10));
    E->FixParameter(11, fsrc->GetParameter(11));

    E->SetLineColor(48);
    E->SetLineStyle(2);

    E->Draw("same");

    // Scuola russa - sottrae i due istogrammi (sorgente - fondo)
    TH1D* hBackground = (TH1D*) f->Get("h1");
    TH1D* hStella = (TH1D*) g->Get("h1");
    TCanvas *c40 = new TCanvas("c40", PLOTS_TITLE, 640, 480);
    TH1D * hist_russianSTD = new TH1D("h1tildato", "Istogramma russo", NBIN, QMIN, QMAX);

    int i;
    for (i = 0; i < nBins; i++) {

        hist_russianSTD->SetBinContent(i, hStella->GetBinContent(i));
    }
    hist_russianSTD->Draw();
}



