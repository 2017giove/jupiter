/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Every tooth in a man's head is more valuable than a diamond.
 * You don't have to brush your teeth — just the ones you want to keep.
 * 
 * 
 */

/* 
 * File:   file_plot.cpp
 * Author: sfera
 *
 * Created on June 7, 2017, 3:05 PM
 */

#include <cstdlib>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    

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
    

    return 0;
}

