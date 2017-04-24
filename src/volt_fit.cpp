/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Every tooth in a man's head is more valuable than a diamond.
 * You don't have to brush your teeth — just the ones you want to keep.
 * 
 * 
 */


#include "WaveAnalysis.h"


/**
 * Esegue il fit lineare di un file nel formato
 * TENSIONE     POSIZIONE_PICCO     ERRORE_TENSIONE     ERRORE_POSIZIONE_PICCO
 * Le righe non contenenti valori in questo formato vengono ignorate
 * 
 * @param nome del file contenente la lista di misure Tensione-PosizionePicco
 */
void volt_fit(char * peaksfile) {

    std::ifstream myfile1;
    std::string myline;

    myfile1.open(peaksfile);

    const int n = 50;
    Double_t voltage[n];
    Double_t peakpos[n];
    Double_t err_voltage[n];
    Double_t err_peakpos[n];
    int i = 0;

    while (std::getline(myfile1, myline)) {
        std::istringstream strm(myline);
        if ((strm >> voltage[i] >> peakpos[i] >> err_voltage[i] >> err_peakpos[i])) {
            std::cout << i << " " << voltage[i] << " " << peakpos[i] << " " << err_voltage[i] << " " << err_peakpos[i] << std::endl;
            peakpos[i] = TMath::Log10(peakpos[i]);
            err_peakpos[i] = TMath::Log10(err_peakpos[i]);
            i++;
            
        } else {
            printf("(riga ignorata)\n");
        }
    }

    TCanvas *c40 = new TCanvas("c40", PLOTS_TITLE, 640, 480);
    c40->SetFillColor(10);
    c40->SetGrid();

    TGraph* mygraph1 = new TGraph(i, voltage, peakpos);
    //    TGraphErrors* mygraph1 = new TGraphErrors(i, voltage, peakpos, err_voltage, err_peakpos);
    mygraph1->SetTitle("Calibrazione");
    mygraph1->GetXaxis()->SetTitle("Tensione PMT [V]");
    mygraph1->GetYaxis()->SetTitle("Log(posizione picco[adc count]) ");

    mygraph1->SetMarkerColor(4);
    mygraph1->SetMarkerStyle(20);
    mygraph1->SetMarkerSize(1);
    mygraph1->Draw("AP");


    TF1* fit_function = new TF1("retta", "[0]*x+[1]", 1000, 2000);

    mygraph1->Fit(fit_function, "M");
    fit_function->SetLineColor(2);
    fit_function->SetLineWidth(1);
    fit_function->Draw("Same");
    //gROOT->SetStyle("Plain");
    gStyle->SetOptFit(1111);

    std::cout << fit_function->GetChisquare() << std::endl;
}