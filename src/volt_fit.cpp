/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Every tooth in a man's head is more valuable than a diamond.
 * You don't have to brush your teeth — just the ones you want to keep.
 * 
 * 
 */

#ifndef VOLTFIT
#define VOLTFIT

#include "WaveAnalysis.h"

void bestresolution_find(char * peaksfile, char* wheretosave);

/**
 * Esegue il fit lineare di un file nel formato
 * TENSIONE     POSIZIONE_PICCO     SIGMA     VALORE_PICCO    INTEGRALE_FONDO    INTEGRALE_SEGNALE
 * Le righe non contenenti valori in questo formato vengono ignorate
 * 
 * @param nome del file contenente la lista di misure Tensione-PosizionePicco
 */
void volt_fit(char * peaksfile, char* wheretosave, char* acqname) {

    std::vector<peak> peaks = peak_load(peaksfile);

    int i = 0;
    int tresh = 0;

    char temp[STR_LENGTH ];

    sprintf(temp, "%s.root", wheretosave);

    printf("\n\nfile %s\n\n", temp);
    TFile *FOut = new TFile(temp, "UPDATE");

    std::vector<float> TVoltage;
    std::vector<float> TPeakpos;
    std::vector<float> TPeakposLog;
    std::vector<float> TResolution;

    for (i = 0; i < peaks.size(); i++) {

        TVoltage.push_back(peaks[i].voltage);
        TPeakposLog.push_back(TMath::Log(peaks[i].peakpos));
        TPeakpos.push_back(peaks[i].peakpos);
        TResolution.push_back(peaks[i].resolution);

        i++;

        if (peaks[i].resolution < 0 or peaks[i].anyproblems != 0) {
            printf("Il fit a %f volt sarà rigettato in acqua. E' un astropesce.\n%s\n", peaks[i].voltage, ERROR_FISHERMAN);
            i--;
        }

    }

    sprintf(temp, "linear_%d", peaks[0].PMTid);
    TCanvas *c40 = new TCanvas(temp, PLOTS_TITLE, 640, 480);
    c40->SetFillColor(10);
    c40->SetGrid();
        gStyle->SetOptFit(1111);

    TGraph* mygraph1 = new TGraph(TVoltage.size(), &(TVoltage[0]), &(TPeakposLog[0]));

    sprintf(temp, "linear_%d", peaks[0].PMTid);
    mygraph1->SetName(temp);
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
    gROOT->SetStyle("Plain");
    gStyle->SetOptFit(1111);
    c40->Write();
    sprintf(temp, "img/%s_linear_%d.eps", acqname, peaks[0].PMTid);
    c40->SaveAs(temp);

    sprintf(temp, "expo_%d", peaks[0].PMTid);
    TCanvas *c4001 = new TCanvas(temp, PLOTS_TITLE, 640, 480);
    TGraph* mygraph2 = new TGraph(TVoltage.size(), &(TVoltage[0]), &(TPeakpos[0]));


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
    sprintf(temp, "img/%s_expo_%d.eps", acqname, peaks[0].PMTid);
    c4001->SaveAs(temp);

    sprintf(temp, "resol%s_%d", acqname, peaks[0].PMTid);
    TCanvas *c402 = new TCanvas(temp, PLOTS_TITLE, 640, 480);
    TGraph* mygraph3 = new TGraph(TVoltage.size(), &(TVoltage[0]), &(TResolution[0]));

    mygraph3->SetTitle("Calibrazione - Risoluzione #frac{#sigma}{E #sqrt{N}}");
    mygraph3->SetTitle("Calibrazione - Risoluzione #frac{#sigma}{E}");
    mygraph3->GetXaxis()->SetTitle("Tensione PMT [V]");
    mygraph3->GetYaxis()->SetTitle("Risoluzione [#sigma/E] ");

    mygraph3->SetMarkerColor(4);
    mygraph3->SetMarkerStyle(20);
    mygraph3->SetMarkerSize(1);
    mygraph3->Draw("AP");
    c402->Write();
    sprintf(temp, "img/%s_resol_%d.eps", acqname, peaks[0].PMTid);
    c402->SaveAs(temp);


    std::cout << fit_function->GetChisquare() << std::endl;
    bestresolution_find(peaksfile, wheretosave);

    sprintf(temp, "data/%s.calibration", acqname);
    std::ofstream savefile(temp, std::ios_base::app);
    savefile << peaks[0].PMTid << " " << fit_function->GetParameter(0) << " " << fit_function->GetParameter(1) << std::endl;
    FOut->Close();
}

/**
 * Evaluates best resolution for provided file with fixed voltage and many triggers
 * @param peaksfile Path to file with fits
 * @param wheretosave Where to save result of choice 
 */
void bestresolution_find(char * peaksfile, char* wheretosave) {

    std::vector<peak> peaks = peak_load(peaksfile);
    double minres = 5000;
    int minresPOS = -1;

    for (int i = 0; i < peaks.size(); i++) {
        //  printf("\n myres %lf\n\n", peaks[i].resolution);
        if (peaks[i].anyproblems != 0) {
            printf("Il fit a %f volt sarà rigettato in acqua. Codice errore:%d\n%s\n", peaks[i].voltage, peaks[i].anyproblems, ERROR_FISHERMAN);

        } else {
            // printf("resol %f ; minres  =%f \n fatti dire %d\n", peaks[i].resolution, minres,peaks.size());
            if (peaks[i].resolution < minres) {
                minres = peaks[i].resolution;
                minresPOS = i;
            }

        }


    }

    if (minresPOS != -1) {
        printf("best one is %d\t%f\n\n", minresPOS, peaks[minresPOS].peakpos);
        peak_save(wheretosave, &(peaks[minresPOS]));
    } else {
        printf("Non è stato trovato nessun buon trigger per questa tensione!\n");
    }

}

#endif