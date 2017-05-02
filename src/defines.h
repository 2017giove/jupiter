/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Do not modify it unless you know exactly what you are doing.
 * Otherwise you'll be in trouble.
 * 
 * It's Boguliubov!
 */

/* 
 * File:   defines.h
 * Author: leopoldo
 *
 * Created on April 9, 2017, 11:43 PM
 */

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TROOT.h"
#include "TF1.h"
#include "TMath.h"
#include "TGraph.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <stdio.h> 
#include <iostream>
#include <ostream>



#define JUPITER "Esperienza GioVe"
#define PLOTS_TITLE "Fisherman's Plot"
#define ERROR_CRUCIAL "Call a qualified expert: 27th Alpes Ave, view on Caprera Circus and all of Rome."
#define ERROR_DEEPER "THERE IS A PROBLEM AND IT IS DEEPER."


#define EXT_ROOT ".root"
#define STR_LENGTH 300


#define TRIGGER_EDGE "neg"
//#define THRESH -25 // non ha senso definirlo perche è utile cambiarlo

#define NBIN 2000
#define QMAX 200
#define QMIN 0
#define N_SAMPLES 1024
#define MAXCH 4
#define DELAY 1160 //da cambiare se si usa un rate di acquisizione diverso
#define RATE 0.7 //da cambiare se si usa un rate di acquisizione diverso

#define ENERGY_CESIO 663.
#define MASS_ELECTRON 511.

struct myEvent {
    /*
     * Questa va modificata togliendo l'array in [MAXCH] e id[MAXCH] e channels, perchè non servono a nulla
     * Che altre informazioni servono?
     */
    int eventID;
    int trigCH;
    float** time_array;
    float** wave_array;
};

struct mySetting {
    char date[STR_LENGTH];
    int Nchan;
    int deltaT;
    float * voltage;
    int * PmtID;
    float * thresh;
    int triggerSetting;
    int delayns;
    int description[10 * STR_LENGTH];
    //Aggiungere altri parametri rilevanti come deltaT, descrizione ...

};

struct peak {
    float resolution;
    float err_resolution;
    float peakpos;
    float err_peakpos;
};

void allocateSetting(struct mySetting* st, int NCHAN) {

    st->voltage = (float*) malloc(NCHAN * sizeof (st->voltage));
    st->PmtID = (int*) malloc(NCHAN * sizeof (st->PmtID));
    st->thresh = (float*) malloc(NCHAN * sizeof (st->thresh));

    if (st->voltage == NULL || st->PmtID == NULL || st->thresh == NULL) {
        printf("Errore allocazione memoria setting. %s\n", ERROR_CRUCIAL);
        exit(-1);
    }

}

void allocateEvent(struct myEvent* ev, int NCHAN) {

    ev->time_array = (float**) malloc(NCHAN * sizeof (float*));
    ev->wave_array = (float**) malloc(NCHAN * sizeof (float*));

    int i;
    for (i = 0; i < NCHAN; i++) {
        ev->time_array[i] = (float*) malloc(N_SAMPLES * sizeof (float));
        ev->wave_array[i] = (float*) malloc(N_SAMPLES * sizeof (float));
    }

    if (ev->time_array == NULL || ev->wave_array == NULL) {
        printf("Errore allocazione memoria event. %s\n", ERROR_CRUCIAL);
        exit(-1);
    }
//    int fotodilollo[4][1024];
//        int fotodilollo2[4][1024];
//    ev->time_array = (float**)fotodilollo;
//    ev->wave_array =  (float**)fotodilollo2;
    
}

std::string appendToRootFilename(const char* filename, const char* suffix) {
    std::string _extension = EXT_ROOT;
    std::string _filename = filename;
    std::string _suffix = suffix + _extension;
    std::string _final = _filename;
    _final.replace(_filename.find(_extension), _extension.length(), _suffix);
    return _final;
}

void printStatus(float progress) {
    int barWidth = 50;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "#";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();

}

void mySetting_print(mySetting st) {

//    printf("Data captured on %s", st.date);
//    printf("PMT_ID \t\t=\t%d\n", st.PmtID);
//    printf("Voltage(V)\t=\t%f\n", st.voltage);
//    printf("Threshold(mV)\t=\t%f\n", st.thresh);
//    printf("Delay(ns)\t=\t%d\n", st.delayns);
//    printf("\n");
}

void mySetting_get(TTree* tset1, mySetting* st) {
//    tset1->ResetBranchAddresses();
//    tset1->SetBranchAddress("PMT_ID", &st->PmtID);
//    tset1->SetBranchAddress("Voltage", &st->voltage);
//    tset1->SetBranchAddress("threshold", &st->thresh);
//    tset1->SetBranchAddress("Delay_ns", &st->delayns);
//    tset1->SetBranchAddress("Date", st->date);
//    tset1->GetEntry(0);
//    tset1->ResetBranchAddresses();
}




#ifndef DEFINES_H
#define DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif /* DEFINES_H */

