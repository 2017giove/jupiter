/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Every tooth in a man's head is more valuable than a diamond.
 * You don't have to brush your teeth — just the ones you want to keep.
 * 
 * 
 */

#ifndef VOLTFIT
#define VOLTFIT


#include "defines.h"

#include "WaveAnalysis.h"

void bestresolution_find(char * peaksfile, char* wheretosave);

/**
 * This function tests the linearity in energy
 * @param linearfile
 */
void dollar_fit(char* linearfile) {
    printf("Mi chiamo %s", linearfile);

    std::string _final = linearfile;
    std::string _extension = ".txt";
    _final = _final.substr(_final.find_last_of("/") + 1);
    _final = _final.replace(_final.find(_extension), _extension.length(), "");

    std::string animal = _final;

    char temp[STR_LENGTH ];

    sprintf(temp, "data/%s.root", animal.c_str());

    TFile *FOut = new TFile(temp, "UPDATE");

    std::vector<float> TPeakpos;
    std::vector<float> TPeakposErr;
    std::vector<float> TEnergy;


    std::ifstream myfile1;
    std::string myline;
    printf("Energy file read: %s\n", linearfile);
    myfile1.open(linearfile);



    int i = 0;
    std::vector<peak> peaks;

    while (std::getline(myfile1, myline)) {
        peak temp;
        std::istringstream strm(myline);
        if (strm >> temp.peakpos >> temp.sigma >> temp.peakpos_err) {
            std::cout << i << " " << temp.peakpos << " " << temp.sigma << " " << temp.peakpos_err << std::endl;
            i++;
            peaks.push_back(temp);
        } else {
            printf("(riga ignorata)\n");
        }
    }

    for (i = 0; i < peaks.size(); i++) {

        TPeakpos.push_back(peaks[i].peakpos);
        TPeakposErr.push_back(peaks[i].peakpos_err);
        TEnergy.push_back(peaks[i].sigma);



    }

    int PMTid = 505;

    sprintf(temp, "linear_%d", PMTid);
    TCanvas *c40 = new TCanvas(temp, PLOTS_TITLE, 640, 480);
    c40->SetFillColor(10);
    //  c40->SetGrid();
    gStyle->SetOptFit(1111);

    //    TGraphErrors* mygraph1 = new TGraphErrors(TPeakpos.size(), &(TEnergy[0]), &(TPeakpos[0]), 0, &(TPeakposErr[0]));
    TGraph* mygraph1 = new TGraph(TPeakpos.size(), &(TEnergy[0]), &(TPeakpos[0]));
    sprintf(temp, "linear_%d", PMTid);

    mygraph1->SetName(temp);
    //    TGraphErrors* mygraph1 = new TGraphErrors(i, voltage, peakpos, err_voltage, err_peakpos);
    mygraph1->SetTitle("Studio linearita' in energia");
    mygraph1->GetXaxis()->SetTitle("Energia attesa (keV)");
    mygraph1->GetYaxis()->SetTitle("Energia misurata (keV)");
    mygraph1->GetYaxis()->SetTitleOffset(1.4);
    
    mygraph1->SetMarkerColor(4);
    mygraph1->SetMarkerStyle(8);
    mygraph1->SetMarkerSize(0.8);


    TF1* fit_function = new TF1("retta", "[0]*x+[1]", 0, 1500);

    mygraph1->Fit(fit_function, "M");
    fit_function->SetLineColor(2);
    fit_function->SetLineWidth(1);


    mygraph1->Draw("AP");
    fit_function->Draw("Same");

//    for (int ii = 0; ii < peaks.size(); ii++) {
//        TLatex * lbl = new TLatex(peaks[ii].peakpos, peaks[ii].sigma, "DOLL");
//        lbl->SetTextSize(0.040);
//        lbl->SetTextFont(42);
//        lbl->SetTextColor(kGreen);
//        mygraph1->GetListOfFunctions()->Add(lbl);
//    }




    gROOT->SetStyle("Plain");
    gStyle->SetOptFit(1111);
    
    TPaveStats* ps = (TPaveStats *) mygraph1->GetListOfFunctions()->FindObject("stats");
    if (ps != nullptr) {

        ps->SetX1NDC(0.70);
        ps->SetX2NDC(0.90);
        ps->SetY1NDC(0.10);
        ps->SetY2NDC(0.30);
    }
    
    c40->Write();
    sprintf(temp, "img/%s_lineardollar_%d.png", animal.c_str(), PMTid);
    c40->SaveAs(temp);

    FOut->Close();
}

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
    std::vector<float> TPeakposErr;
    std::vector<float> TPeakposLog;
    std::vector<float> TPeakposLogErr;
    std::vector<float> TResolution;
    std::vector<float> TResolutionErr;

    for (i = 0; i < peaks.size(); i++) {


        if (peaks[i].resolution < 0 || peaks[i].anyproblems != 0) {
            printf("Il fit a %.1f volt, trigger %f sarà rigettato in acqua. E' un astropesce.\n%s\n", peaks[i].voltage, peaks[i].thresh, ERROR_FISHERMAN);


        } else {
            TVoltage.push_back(peaks[i].voltage);
            TPeakposLog.push_back(TMath::Log(peaks[i].peakpos));
            TPeakposLogErr.push_back(peaks[i].peakpos_err / peaks[i].peakpos);
            TPeakpos.push_back(peaks[i].peakpos);
            TPeakposErr.push_back(peaks[i].peakpos_err);
            TResolution.push_back(peaks[i].resolution);
            TResolutionErr.push_back(peaks[i].resolution * TMath::Sqrt(((peaks[i].sigma_err) / peaks[i].sigma)*((peaks[i].sigma_err) / peaks[i].sigma) + (peaks[i].sigma / peaks[i].peakpos)*(peaks[i].sigma / peaks[i].peakpos)));
            //   TResolutionErr.push_back(peaks[i].resolution*TMath::Sqrt(((peaks[i].sigma_err)/peaks[i].sigma)*((peaks[i].sigma_err)/peaks[i].sigma) + (peaks[i].peakpos_err/peaks[i].peakpos)*(peaks[i].peakpos_err/peaks[i].peakpos) ));
        }


    }

    sprintf(temp, "linear_%d", peaks[0].PMTid);
    TCanvas *c40 = new TCanvas(temp, PLOTS_TITLE, 640, 480);
    c40->SetFillColor(10);
    //  c40->SetGrid();
    gStyle->SetOptFit(1111);

    TGraph* mygraph1 = new TGraph(TVoltage.size(), &(TVoltage[0]), &(TPeakposLog[0]));

    sprintf(temp, "linear_%d", peaks[0].PMTid);
    mygraph1->SetName(temp);
    //    TGraphErrors* mygraph1 = new TGraphErrors(i, voltage, peakpos, err_voltage, err_peakpos);
    mygraph1->SetTitle("Retta di calibrazione");
    mygraph1->GetXaxis()->SetTitle("Tensione PMT [V]");
    mygraph1->GetYaxis()->SetTitle("Log(posizione picco[adc count]) ");


    mygraph1->SetMarkerColor(4);
    mygraph1->SetMarkerStyle(20);
    mygraph1->SetMarkerSize(0.7);
  






    TF1* fit_function = new TF1("retta", "[0]*x+[1]", 1000, 2000);
    mygraph1->Draw("AP");  
    fit_function->SetLineColor(2);
    fit_function->SetLineWidth(1);
    mygraph1->Fit(fit_function, "M");

   // fit_function->Draw("Same");
 //   gROOT->SetStyle("Plain");
    gStyle->SetOptFit(1111);
    c40->Write();

    TPaveStats* ps = (TPaveStats *) mygraph1->GetListOfFunctions()->FindObject("stats");
    if (ps != nullptr) {

        ps->SetX1NDC(0.75);
        ps->SetX2NDC(0.95);
        ps->SetY1NDC(0.10);
        ps->SetY2NDC(0.30);
    }




    sprintf(temp, "img/%s_linear_%d.png", acqname, peaks[0].PMTid);
    c40->SaveAs(temp);

    sprintf(temp, "expo_%d", peaks[0].PMTid);
    TCanvas *c4001 = new TCanvas(temp, PLOTS_TITLE, 640, 480);
    TGraphErrors* mygraph2 = new TGraphErrors(TVoltage.size(), &(TVoltage[0]), &(TPeakpos[0]), 0, &(TPeakposErr[0]));


    mygraph2->SetTitle("Calibrazione");
    mygraph2->GetXaxis()->SetTitle("Tensione PMT [V]");
    mygraph2->GetYaxis()->SetTitle("Posizione picco[adc count] ");

    mygraph2->SetMarkerColor(4);
    //    mygraph2->SetMarkerStyle(20);
    mygraph2->SetMarkerSize(1);
    mygraph2->Draw("APE");

    TF1* fit_function2 = new TF1("rett2", "TMath::Exp([0]*x+[1])", 1000, 2000);
    fit_function2->SetParameter(0, fit_function->GetParameter(0));
    fit_function2->SetParameter(1, fit_function->GetParameter(1));

    fit_function2->Draw("Same");
    c4001->Write();
    sprintf(temp, "img/%s_expo_%d.eps", acqname, peaks[0].PMTid);
    c4001->SaveAs(temp);

    sprintf(temp, "resol%s_%d", acqname, peaks[0].PMTid);
    TCanvas *c402 = new TCanvas(temp, PLOTS_TITLE, 640, 480);
    TGraphErrors* mygraph3 = new TGraphErrors(TVoltage.size(), &(TVoltage[0]), &(TResolution[0]), 0, &(TResolutionErr[0]));

    mygraph3->SetTitle("Calibrazione - Risoluzione #frac{#sigma}{E #sqrt{N}}");
    mygraph3->SetTitle("Calibrazione - Risoluzione #frac{#sigma}{E}");
    mygraph3->GetXaxis()->SetTitle("Tensione PMT [V]");
    mygraph3->GetYaxis()->SetTitle("Risoluzione [#sigma/E] ");

    mygraph3->SetMarkerColor(4);
    // mygraph3->SetMarkerStyle(20);
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
    c40->Close( );
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

    double minresIA = 5000;
    int minresPOSia = -1;

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

            if (peaks[i].IA < minresIA) {
                minresIA = peaks[i].IA;
                minresPOSia = i;
            }

        }


    }

    char tmpstring[STR_LENGTH];
    sprintf(tmpstring, "%s.ia", wheretosave);

    if (minresPOS != -1) {
        printf("best one is %d\t%f\n\n", minresPOS, peaks[minresPOS].peakpos);
        peak_save(wheretosave, &(peaks[minresPOS]));
        peak_save(tmpstring, &(peaks[minresPOSia]));
    } else {
        printf("Non è stato trovato nessun buon trigger per questa tensione!\n");
    }

}

#endif