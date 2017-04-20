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

#define PLOTS_TITLE "Fisherman's Plot"
#define TRIGGER_EDGE "neg"
#define THRESH -25

#define NBIN 2000
#define QMAX 200
#define QMIN 0
#define N_SAMPLES 1024
#define MAXCH 4
#define DELAY 1160 //da cambiare se si usa un rate di acquisizione diverso
#define RATE 0.7 //da cambiare se si usa un rate di acquisizione diverso

#define ENERGY_CESIO 663.
#define MASS_ELECTRON 511.

#define CRUCIAL_ERROR "Call a qualified expert: 27th Alpes Ave, view on Caprera Circus and all of Rome."

struct myEvent {
    int trigId;
    int channels;
    int id[MAXCH];
    float time_array[MAXCH][N_SAMPLES];
    float wave_array[MAXCH][N_SAMPLES];
};

typedef struct{
    float resolution;
    float error;
}peak;


#ifndef DEFINES_H
#define DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif /* DEFINES_H */

