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
#include "TGraph2D.h"
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
#define ERROR_FISHERMAN "We did not find what you were looking for: the fish escaped the net."
#define NOT_FOUND_INT -241
#define NOT_CARING_INT 433
#define NOT_CARING_STRING "Blessed are they who hold lively conversations with the helplessly mute, for they shall be called dentists."
#define EXT_ROOT ".root"
#define STR_LENGTH 300
#define BASE_SPAGO 70

#define TRIGGER_EDGE "neg"
//#define THRESH -25 // non ha senso definirlo perche è utile cambiarlo

#define NBIN 2000
#define QMAX 200
#define QMIN 0
#define N_SAMPLES 1024
#define SANTA_MAX 50

#define MAXCH 4
#define DELAY 1160 //da cambiare se si usa un rate di acquisizione diverso
#define RATE 0.7 //da cambiare se si usa un rate di acquisizione diverso

#define ENERGY_CESIO 663.
#define MASS_ELECTRON 511.

struct myEvent {
    /*
     * Che altre informazioni servono?
     */
    int eventID;
    int trigCH;
    float time_array[SANTA_MAX][N_SAMPLES];
    float wave_array[SANTA_MAX][N_SAMPLES];
};

struct mySetting {
    char date[STR_LENGTH];
    int Nchan;
    int deltaT;
    float voltage[SANTA_MAX];
    int PmtID[SANTA_MAX];
    float thresh[SANTA_MAX];
    int triggerSetting;
    int delayns;
    char description[10 * STR_LENGTH];
    //Aggiungere altri parametri rilevanti come...

};

struct peak {
    float sigma;
    float peakpos;
    float peakvalue;
    float nBG;
    float nSGN;
};



void allocateSetting(struct mySetting* st, int NCHAN) {
    int i;
    for (i = 0; i < SANTA_MAX; i++) {
        st->voltage[i] = NOT_CARING_INT;
        st->PmtID[i] = NOT_CARING_INT;
        st->thresh[i] = NOT_CARING_INT;
        sprintf(st->description, "%s", NOT_CARING_STRING);
    }
    //
    //    st->voltage = (float*) malloc(NCHAN * sizeof (st->voltage));
    //    st->PmtID = (int*) malloc(NCHAN * sizeof (st->PmtID));
    //    st->thresh = (float*) malloc(NCHAN * sizeof (st->thresh));
    //
    //    if (st->voltage == NULL || st->PmtID == NULL || st->thresh == NULL) {
    //        printf("Errore allocazione memoria setting. %s\n", ERROR_CRUCIAL);
    //        exit(-1);
    //    }
    // Santizzato


}

void allocateEvent(struct myEvent* ev, int NCHAN) {

    int i, j;
    for (i = 0; i < SANTA_MAX; i++) {
        for (j = 0; j < N_SAMPLES; j++) {
            ev->time_array[i][j] = NOT_CARING_INT;
            ev->wave_array[i][j] = NOT_CARING_INT;
        }
    }

    //    ev->time_array = (float**) malloc(NCHAN * sizeof (float*));
    //    ev->wave_array = (float**) malloc(NCHAN * sizeof (float*));
    //
    //    int i;
    //    for (i = 0; i < NCHAN; i++) {
    //        ev->time_array[i] = (float*) malloc(N_SAMPLES * sizeof (float));
    //        ev->wave_array[i] = (float*) malloc(N_SAMPLES * sizeof (float));
    //    }
    //
    //    if (ev->time_array == NULL || ev->wave_array == NULL) {
    //        printf("Errore allocazione memoria event. %s\n", ERROR_CRUCIAL);
    //        exit(-1);
    //    }
    //    //    int fotodilollo[4][1024];
    //    //        int fotodilollo2[4][1024];
    //    //    ev->time_array = (float**)fotodilollo;
    //    //    ev->wave_array =  (float**)fotodilollo2;
    //    Santizzato
}

std::string appendToRootFilename(const char* filename, const char* suffix) {
    std::string _extension = EXT_ROOT;
    std::string _filename = filename;
    std::string _suffix = suffix + _extension;
    std::string _final = _filename;
    _final.replace(_filename.find(_extension), _extension.length(), _suffix);
    return _final;
}

std::string filenameFromPath(const char*filename) {
    std::string _final = filename;
    std::string _extension = EXT_ROOT;
    _final = _final.substr(_final.find_last_of("/") + 1);
    _final = _final.replace(_final.find(_extension), _extension.length(), "");
    return _final;

}

void printStatus(float progress) {
    int barWidth = 40;

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

int get_minimum_pos  (float* test, int start, int end) {
    
    int minpos = 0;
    float min = 0;
    
    for (int i = start; i < end; i++) {
        
        if (test[i] < min) {
            
            min = test[i];
            minpos = i;
            
        }
        
    }
    
    return minpos;
    
}

int triggerbin(int threshold, float* test) {
    int i;
    if (threshold < 0) {
        for (i = 100; i < N_SAMPLES; i++) {
            if (test[i] < threshold) return i - 1;
        }
        return 0;
    } else if (threshold >= 0) {
        for (i = 100; i < N_SAMPLES; i++) {
            if (test[i] > threshold) return i - 1;
        }
        return 0;
    }
}

float BoundIntegral(float* test, int min, int max) {
    float integral = 0;
    int i;

    for (i = min; i < max; i++) {
        integral += test[i];
    }
    //cout << _samples[min] << "    " << _samples[max] << endl;
    return integral / ((float) (max - min));
}

bool isnotaspike (float* test, int triggerpos, float baseline) {
    
    float testint =  - BoundIntegral (test, triggerpos, triggerpos + 100);
    int minpos = get_minimum_pos (test, triggerpos, triggerpos + 100);
    float maxspike = - test[minpos];
    float troppospike = (testint - baseline)/(maxspike - baseline);
    if (troppospike < 0.1) {
        printf("\nBrava gente: bomba %f\ntestint %f\nminpos %f\nmaxspike %f\ntroppospike %f\n\n", test[0], testint,minpos, maxspike,troppospike);
        return 0;
    }
    return 1;
}

//
//struct myEvent {
//    /*
//     * Che altre informazioni servono?
//     */
//    int eventID;
//    int trigCH;
//    float time_array[SANTA_MAX][N_SAMPLES];
//    float wave_array[SANTA_MAX][N_SAMPLES];
//};
//
//struct mySetting {
//    char date[STR_LENGTH];
//    int Nchan;
//    int deltaT;
//    float voltage[SANTA_MAX];
//    int PmtID[SANTA_MAX];
//    float thresh[SANTA_MAX];
//    int triggerSetting;
//    int delayns;
//    char description[10 * STR_LENGTH];
//    //Aggiungere altri parametri rilevanti come...
//
//};

void mySetting_print(mySetting *st) {

    printf("Data captured on %s for %d secs in configuration %d with %d channels.\n", st->date, st->deltaT, st->triggerSetting, st->Nchan);
    printf("Delay ns = %d\n", st->delayns);
    int i;
    for (i = 0; i < st->Nchan; i++) {
        printf("*****\tCH #%d\t*****\n", i);
        printf("PMT_ID\t=\t%d\n", st->PmtID[i]);
        printf("Voltage(V)\t=\t%f\n", st->voltage[i]);
        printf("Threshold(mV)\t=\t%f\n", st->thresh[i]);
        printf("\n");
    }

}

void mySetting_get(TTree* tset1, mySetting* st) {

    char branchDef[STR_LENGTH];

    tset1->SetBranchAddress("Nchan", &st->Nchan);
    tset1->SetBranchAddress("deltaT", &st->deltaT);
    tset1->GetEntry(0);

    allocateSetting(st, st->Nchan);

    tset1->SetBranchAddress("Voltage", &st->voltage);
    tset1->SetBranchAddress("PMT_ID", &st->PmtID);
    tset1->SetBranchAddress("threshold", &st->thresh);
    tset1->SetBranchAddress("Delay_ns", &st->delayns);
    tset1->SetBranchAddress("Date", st->date);
    tset1->SetBranchAddress("description", st->description);

    tset1->GetEntry(0);
}

int PMTtoCH(int PMT, mySetting* st) {
    int CH;
    int i;
    for (i = 0; i < st->Nchan; i++) {
        if (st->PmtID[i] == PMT) return i;
    }
    return NOT_FOUND_INT;
}

int CHtoPMT(int CH, mySetting* st) {
    return st->PmtID[CH];
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

