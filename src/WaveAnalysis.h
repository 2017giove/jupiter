/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Do not modify it unless you know exactly what you are doing.
 * Otherwise you'll be in trouble.
 * 
 * It's Boguliubov!
 */

/* 
 * File:   WaveAnalysis.h
 * Author: leopoldo
 *
 * Created on April 10, 2017, 1:08 AM
 */

/////////////////////////////////////////////////
//                                             //
//        WAVEANALYSIS.H by DeatStar           //
//                                             //
// libreria contenente la classe Waveform con  //
// relative funzioni per l'analisi della       //
// forma d'onda di un singolo segnale          //
//                                             //
/////////////////////////////////////////////////

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TROOT.h"
#include "TGraph.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include "TF1.h"
#include "TMath.h"
#include "defines.h"



class WaveForm {
public:
    std::vector<double> _samples;
    std::vector<double> _times;
    TF1 *gf;
    TH1D *h1;

    /*Riempie il vettore e riscala i samples con un fattore resc_fact*/
    void FillVec(int nSamples, float* times, float* samples, float resc_fact) {

        _samples.resize(nSamples);
        _times.resize(nSamples);
        for (unsigned int i(0); i < nSamples; i++) {

            _samples[i] = (double) resc_fact * samples[i];
            _times[i] = (double) times[i];

        }

    };

    //Sottrae un valore fissato ai samples

    void offset(std::vector<double> samples, const float& value) {

        unsigned int i;

        for (i = 0; i < _samples.size(); ++i)
            _samples[i] -= value;

    };

    /*Sposta tutti i tempi di un valore fissato*/
    void shift_time(std::vector<double> times, const float& time_offset) {

        unsigned int i;

        for (i = 0; i < _times.size(); ++i)
            _times[i] += time_offset;

    };

    /*Trova il punto massimo dei samples (X)*/
    int FindMax() {

        int i, max;

        for (i = 0; i < _samples.size(); i++) {
            if (_samples[i] > _samples[max]) {
                max = i;
            }
        }
        return max;
    };

    /*Trova il valore massimo di _samples (Y)*/
    float GetMax() {

        int i;
        float max = 0;

        for (i = 0; i < _samples.size(); i++) {
            if (_samples[i] > max) {
                max = _samples[i];
            }
        }
        return max;
    };

    /*Restituisce la "media integrale"*/
    float Integral() {

        float Mean = 0;
        int i;

        for (i = 0; i < _samples.size(); i++) {
            Mean += _samples[i];
        }

        return Mean / _samples.size();

    };

    float BoundIntegral(int min, int max) {
        float integral = 0;
        int i;

        for (i = (int) min; i <= (int) max; i++) {
            integral += _samples[i];
        }
        //cout << _samples[min] << "    " << _samples[max] << endl;
        return integral / ((float) (max - min));
    };

    /*Esegue un fit con un esponenziale doppio*
     * Non la faccio e non la chiedo */ 
    void DoubleExpFit(float charge) {
        int Size = _times.size();
        double step = 1. / (double) RATE;

        //TCanvas *c1 = new TCanvas();
        TGraph *g = new TGraph(Size, &(_times[0]), &(_samples[0]));
        gf = new TF1("f1", "[0]*(TMath::Exp(-[1]*(x-[3])) - TMath::Exp(-[2]*(x-[3])))", Size * step - DELAY, Size * step);

        gf->SetParNames("Amplitude", "tau_1", "tau_2", "shift");
        gf->SetParameters(charge * 4.6, 0.005, 0.04, Size * step - DELAY);
        g->Fit("f1", "R", "N");
        //g->Draw("AL");


    };

    //Restituisce il massimo di una funzione convessa con cui si fitta la Waveform

    double Maximum(std::vector<double> x, std::vector<double> y) {
        int max;
        max = FindMax();
        TGraph *mg = new TGraph(x.size(), &(y[0]), &(x[0]));
        TF1 *mF = new TF1("f2", "pol2", (max - (x.size()) / 20), (max + (x.size()) / 20));
        mg->Fit("f2", "R,Q,N");

        return mF->GetMaximum();

    }

    // non lo faccio e non lo chiedo
    bool CheckWave() {
        int i = 0;
        bool flag = false;

        while (flag == false && i < N_SAMPLES) {
            if (_samples[i] > THRESH) {
                flag = true;
            }
            i++;
        }

        while (flag == true && i < N_SAMPLES) {
            if (_samples[i] >= 499.0) {
                flag = false;
            }
            i++;
        }

        return flag;
    }
};

/*Esegue un fit definito dall'utente
 (ma di che stiamo a parla*/  
/*void GeneralFit(std::vector<double> x, std::vector<double> y,char* type, int xmin, int xmax)
{
    
  //TCanvas *c1 = new TCanvas();
  
  TGraph *g = new TGraph(x.size(), &(y[0]), &(x[0]));
  gf = new TF1("f1",type,xmin,xmax);
  g->Fit("f1", "R");
  
  //g->Draw("AL");
  //cout << gf->GetChisquare() << endl;
  //cout << gf->GetParameter(0) << endl;
  //cout << gf->GetMaximum() << endl;
  
};
 */


