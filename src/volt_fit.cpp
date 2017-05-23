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
 * TENSIONE     POSIZIONE_PICCO     SIGMA     VALORE_PICCO    INTEGRALE_FONDO    INTEGRALE_SEGNALE
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
    Double_t esfpeakpos[n];
    Double_t peakpos[n];
    Double_t sigma[n];
    Double_t peakval[n];
    Double_t resolution[n];
    Double_t nBG[n];
    Double_t nSGN[n];

    int i = 0;
    int tresh = 0;

    char temp[STR_LENGTH ];

    std::string fout = peaksfile;
    std::string whattolook = ".expert";
    std::string whattoput = "calib.root";
    fout = fout.replace(fout.find(whattolook), whattolook.length(), whattoput);

    printf("\n\nfile %s\n\n", fout.c_str());
    TFile *FOut = new TFile(fout.c_str(), "UPDATE");


    while (std::getline(myfile1, myline)) {
        std::istringstream strm(myline);
        if (strm >> voltage[i] >> tresh >> peakpos[i] >> sigma[i] >> peakval[i] >> nSGN[i] >> nBG[i]) {
            std::cout << i << " " << voltage[i] << " " << tresh << " " << peakpos[i] << " " << sigma[i] << " " << peakval[i] << " " << nSGN[i] << " " << nBG[i] << std::endl;
            esfpeakpos[i] = peakpos[i];
     

            resolution[i] = sigma[i] / peakpos[i];// / TMath::Sqrt(nSGN[i]);
            
                   peakpos[i] = TMath::Log(peakpos[i]);
            i++;

            if (resolution[i] < 0) {
                printf("Il fit a %f volt sarà rigettato in acqua.\n%s\n", voltage[i], ERROR_FISHERMAN);
                i--;
            }


        } else {
            printf("(riga ignorata)\n");
        }
    }


    TCanvas *c40 = new TCanvas("linear", PLOTS_TITLE, 640, 480);
    c40->SetFillColor(10);
    c40->SetGrid();
    gStyle->SetOptFit(1111);

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
    //gStyle->SetOptFit(1111);
    c40->Write();




    TCanvas *c4001 = new TCanvas("expo", PLOTS_TITLE, 640, 480);
    TGraph* mygraph2 = new TGraph(i, voltage, esfpeakpos);


    mygraph2->SetTitle("Calibrazione");
    mygraph2->GetXaxis()->SetTitle("Tensione PMT [V]");
    mygraph2->GetYaxis()->SetTitle("Posizione picco[adc count] ");

    mygraph2->SetMarkerColor(4);
    mygraph2->SetMarkerStyle(20);
    mygraph2->SetMarkerSize(1);
    mygraph2->Draw("AP");


    TF1* fit_function2 = new TF1("rett2", "TMath::Exp([0]*x+[1])", 1000, 2000);
    fit_function2->SetParameter(0, fit_function->GetParameter(0));
    fit_function2->SetParameter(1, fit_function->GetParameter(1));

    fit_function2->Draw("Same");
    c4001->Write();



    TCanvas *c402 = new TCanvas("resol", PLOTS_TITLE, 640, 480);
    TGraph* mygraph3 = new TGraph(i, voltage, resolution);

    mygraph3->SetTitle("Calibrazione - Risoluzione #frac{#sigma}{E #sqrt{N}}");
     mygraph3->SetTitle("Calibrazione - Risoluzione #frac{#sigma}{E}");
    mygraph3->GetXaxis()->SetTitle("Tensione PMT [V]");
    mygraph3->GetYaxis()->SetTitle("Risoluzione [#sigma/E] ");

    mygraph3->SetMarkerColor(4);
    mygraph3->SetMarkerStyle(20);
    mygraph3->SetMarkerSize(1);
    mygraph3->Draw("AP");
    c402->Write();

    std::cout << fit_function->GetChisquare() << std::endl;

    //  FOut->Close();
}