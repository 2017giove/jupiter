/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Every tooth in a man's head is more valuable than a diamond.
 * You don't have to brush your teeth — just the ones you want to keep.
 * 
 * 
 */

/********************************************************************\
 
  Name:         drs_jupiter.cpp
  Created by:   Stefan Ritt
  Modified by:  Death Star Team
  Corrected by: JupiTer
  
 
  Contents:     Simple example application to read out a DRS4 
                evaluation board and 
                write a ROOT output file on JupiTer
                 
 
  $Id: drs_sub.cpp $
 
\********************************************************************/


#ifdef _MSC_VER

#include <windows.h>

#elif defined(OS_LINUX)

#define O_BINARY 0

#include <unistd.h>
#include <ctype.h>
#include <ctime>
#include <sys/ioctl.h>
#include <errno.h>

#define DIR_SEPARATOR '/'

#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <sstream>      // std::ifstream
#include <ctime>
#include "time.h" 

#include <chrono>

#include <math.h>
#include "TCanvas.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TSystem.h"
#include "TBranch.h"
#include "TTree.h"
#include "TFile.h"
#include "TRint.h"
#include <TApplication.h>

#include "strlcpy.h"
#include "DRS.h"


#include "defines.h"

/** -------------------- Rieussec Class -------------------- 
 This class is used to analyze code performance using an exclusive Montblanc Rieussec Cronograph.
 * The automatic chronograph in the Nicolas Rieussec Collection is one of Montblanc's tributes to the inventor of the first patented chronograph. 
 * Its innovative minute display, along with the modern design of its rotating discs for elapsed seconds and minutes, creates purity and clarity 
 * that consistently continue into luminous hands for the time in the primary zone and a skeletonized hand for the hour in the second time zone. 
 * Equipped with a monopusher mechanism, a column-wheel and vertical disc coupling, the self-winding manufacture caliber MB R200 features several 
 * additional complications that complement its classical appearance. 
 * These include a day-night display for the second time zone and a rapid-reset mechanism for the hours-hand.
 * 
 * For information and documentation: 
 * http://www.montblanc.com/en-us/collection/watches/montblanc-nicolas-rieussec-collection/109996-Montblanc-Nicolas-Rieussec-Chronograph-Automatic.html
 * 
 * Usage: just declare a variable 
 *          rieussec mynewmbclock;
 *          [... the piece of code you want to measure ...]
 *          double timeelapsed = mynewmbclock.elapsed();
 * 
 */
class rieussec {
public:
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;

    rieussec() : lastTime(std::chrono::high_resolution_clock::now()) {
    }

    inline double elapsed() {
        std::chrono::time_point<std::chrono::high_resolution_clock> thisTime = std::chrono::high_resolution_clock::now();
        double deltaTime = std::chrono::duration<double>(thisTime - lastTime).count();
        lastTime = thisTime;
        return deltaTime;
    }
};

/*-------------------- end of Rieussec Class ----------------------------------------------*/




using namespace std;

/*------------------------------------------------------------------*/
float getTriggerSource(myEvent *ev, mySetting *st);

int main(int argc, char* argv[]) {

    if (argc < 8) {
        cout << argv[0] << ": Usage" << endl;
        cout << argv[0] << " filename " << endl;
        cout << "          deltaT   : time of the experiment in s" << endl;
        cout << "          nchan : number of channels to acquire [1-4]" << endl;
        cout << "          delay    : Trigger delay in ns" << endl; //<<--- ATTENZIONE: è veramente il tempo morto, o forse il ritardo del trigger (da quando far partire l'acquisizione???
        cout << "          thresh   : Threshold in mV" << endl;
        cout << "          source   : trigger source 1=ch1, 2=ch2, 3=ch1 or ch2" << endl;
        cout << "          Voltage  : Voltage of PMT" << endl;
        cout << "          PMT      : ID number of the crystal, put one for each channel specified in nchan" << endl;
        //                              nome    DT  # del thr src V    pmtIDs
        cout << " Example: drs_jupiter filename 300 1 900  25  1  1500 500" << endl;
        //                                 1    2   3  4   5   6   7   8 -9-10-11
        return 0;
    }

    printf("Loading settings...\n");

    int i;

    int maxchan = atoi(argv[3]);
    /* Lettura impostazioni     */
    mySetting cset;
    allocateSetting(&cset, maxchan);

    char *fileName = argv[1];

    cset.deltaT = atoi(argv[2]);

    cset.Nchan = maxchan;
    cset.delayns = atoi(argv[4]);
    int triggerSource = atoi(argv[6]);

    printf("Canali da acquisire\n");

    for (i = 0; i < maxchan; i++) {
        cset.thresh[i] = -atof(argv[5]);
        //cset.thresh = -100.; //2*Voltage*THRESH/1200;
        cset.voltage[i] = atof(argv[7]);
        cset.PmtID[i] = atoi(argv[i + 8]);
        printf("%d\n", cset.PmtID[i]);
    }


    bool triggerEdge;
    if (strncmp(TRIGGER_EDGE, "neg", 3) == 0) triggerEdge = true;
    else if (strncmp(TRIGGER_EDGE, "pos", 3) == 0) triggerEdge = false;


    time_t t0 = time(0);
    time_t Current_Time;
    time(&Current_Time);
    sprintf(cset.date, "%s", asctime(localtime(&Current_Time)));
    cout << cset.date << endl;




    /* Apertura files e creazione albero */
    TFile * f1;
    Int_t comp = 4;

    myEvent ev;
    allocateEvent(&ev, maxchan);


    char rootFileName[130];
    sprintf(rootFileName, "data/%s.root", fileName);
    f1 = (TFile*) new TFile(rootFileName, "RECREATE");
    f1->SetCompressionLevel(comp);


    TTree * tree;
    tree = (TTree*) new TTree("t1", "title");

    /*
    >tset
     *  Delay_ns
     *  Date
     *  Voltage
     *  PMT_ID
     *  threshold
     */

    char branchDef[STR_LENGTH];


    TTree * Tset = new TTree("tset", "Acquire Settings");
    Tset->Branch("Nchan", &cset.Nchan, "Nchan/I");

    Tset->Branch("Delay_ns", &cset.delayns, "Delay_ns/I");
    Tset->Branch("Date", cset.date, "date/C");

    Tset->Branch("deltaT", &cset.deltaT, "deltaT/I");

    Tset->Branch("description", cset.description, "description/C");

    sprintf(branchDef, "Voltage[%d]/F", SANTA_MAX);
    Tset->Branch("Voltage", cset.voltage, branchDef);

    sprintf(branchDef, "PMT_ID[%d]/I", SANTA_MAX);
    Tset->Branch("PMT_ID", cset.PmtID, branchDef);

    sprintf(branchDef, "threshold[%d]/F", SANTA_MAX);
    Tset->Branch("threshold", cset.thresh, branchDef);

    // f1->cd();
    Tset->Fill();
    //  Tset->Write();
    /*
    >t1
     *  trigID
     *  channels
     *  id[4]
     *  time_array[4][N_SAMPLES]
     *  wave_array[4][N_SAMPLES]
          
    trigID significa in realtà ID univoco evento 
    channels il numero di canali attivi
    id contiene quali canali hanno effettivamente registrato l'evento
     
     * è memorizzata nella struttura myevent
     */





    TBranch * b_eventId = tree->Branch("eventID", &ev.eventID, "eventID/I");



    sprintf(branchDef, "time_array[%d][1024]/F", SANTA_MAX);
    TBranch * b_time_array = tree->Branch("time_array", &ev.time_array[0][0], branchDef);

    sprintf(branchDef, "wave_array[%d][1024]/F", SANTA_MAX);
    TBranch * b_wave_array = tree->Branch("wave_array", &ev.wave_array[0][0], branchDef);






    /*  Inizializzazione scheda*/
    int nBoards;
    DRS *drs;
    DRSBoard *b;


    /* do initial scan */
    drs = new DRS();

    /* show any found board(s) */
    for (i = 0; i < drs->GetNumberOfBoards(); i++) {
        b = drs->GetBoard(i);
        printf("Found DRS4 evaluation board, serial #%d, firmware revision %d\n", b->GetBoardSerialNumber(), b->GetFirmwareVersion());
    }

    /* exit if no board found */
    nBoards = drs->GetNumberOfBoards();
    if (nBoards == 0) {
        printf("No DRS4 evaluation board found. %s\n", ERROR_CRUCIAL);
        return 0;
    }

    /* continue working with first board only */
    b = drs->GetBoard(0); //ciambo

    /* initialize board */
 
    b->Init();

    /* set sampling frequency */
    b->SetFrequency(0.7, true); //perche

    /* enable transparent mode needed for analog trigger */
    b->SetTranspMode(1);

    /* set input range to -0.5V ... +0.5V */
    b->SetInputRange(0);

    /* use following lines to enable hardware trigger on CH1 at 50 mV positive edge */
    if (b->GetBoardType() >= 8) { // Evaluaiton Board V4&5
        b->EnableTrigger(1, 0); // enable hardware trigger
        //b->SetTriggerSource(1<<triggerSource);        // set input trigger source
    } else if (b->GetBoardType() == 7) { // Evaluation Board V3
        b->EnableTrigger(0, 1); // lemo off, analog trigger on
        // b->SetTriggerSource(0); // use CH1 as source  DA TOGLIERE
    }
    b->SetTriggerLevel(cset.thresh[0] / 1000.); // threshold is in V
    b->SetTriggerPolarity(triggerEdge); // positive edge

    /* use following lines to set individual trigger elvels */
    b->SetIndividualTriggerLevel(1, cset.thresh[0] / 1000.);
    b->SetIndividualTriggerLevel(2, cset.thresh[0] / 1000.);
    b->SetIndividualTriggerLevel(3, cset.thresh[0] / 1000.);


    /*setta la sorgente del trigger in codice binario
     es: CH1=1 CH2=2 CH3=4, CH1_OR_CH2 = 3*/
    /*
       AND  OR
     * 0000 0000
     * 
     *      0001
     * 0101 0000
     */

    b->SetTriggerSource(triggerSource);
    b->SetTriggerDelayNs(cset.delayns); // ns trigger delay

    t0 = time(0);
    int totevents = 0;

    // Ciclo Eventi - Acquisizione per un intervallo di tempo deltaT
    totevents = 0;

    double drstime = 0;
    double pctime = 0;

    while ((time(0) - t0) < cset.deltaT) {
        /* start board (activate domino wave) */
        b->StartDomino();

        /* wait for trigger - write to console only first time */
        if (totevents == 0) cout << "Waiting for trigger..." << endl;
        rieussec tr;
        while (b->IsBusy());
        drstime += tr.elapsed();

        rieussec tr2;
        if (totevents == 0) cout << "Trigger found, session started..." << endl;

        /* read all waveforms */
        b->TransferWaves(0, 8);



        for (int ch = 0; ch < maxchan; ch++) {

            ev.eventID = totevents;

            /* read time (X) array of first channel in ns
             *    Note: On the evaluation board input #1 is connected to channel 0 and 1 of
               the DRS chip, input #2 is connected to channel 2 and 3 and so on. So to
               get the input #2 we have to read DRS channel #2, not #1.
             */
            b->GetTime(0, 2 * ch, b->GetTriggerCell(0), ev.time_array[ch]);
            /* decode waveform (Y) array of first channel in mV */
            b->GetWave(0, 2 * ch, ev.wave_array[ch]);


        }



        tree->Fill();

        /* print some progress indication */
        printf("%d ev - %d sec rem.", totevents, cset.deltaT - (time(0) - t0));
        //printf("\n%d %d %f\n",cset.deltaT, (time(0)-t0),(float)((time(0) - t0) / (float)cset.deltaT));
        printStatus((float) ((time(0) - t0) / (float) cset.deltaT));

        totevents++;
        pctime += tr2.elapsed();
    }


    TBranch * b_trigId = tree -> Branch("trigCH", &ev.trigCH, "trigCH/I");
    for (i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        ev.trigCH = getTriggerSource(&ev, &cset);
        // printf("%d\n\n", ev.trigCH);
        b_trigId->Fill();
    }

    //tree->Write();
    f1->Write();
    f1->Close();

    printf("\nTempo DRS:\t%lf\t(%lf )\nTempo PC:\t%lf\t(%lf )\n", drstime, pctime, (double) drstime / (drstime + pctime), (double) pctime / (drstime + pctime));
    printf("\n*** ACQUISITION IS COMPLETE ***\n");


    //  fclose(tempofile    );
    /* delete DRS object -> close USB connection */
    delete drs;
}


//da velocizzare utilizzado la funzioen GetMaximum al posto di fare tutti questi loop;
//            printf("Lollo voleva dire min %f\n", min_element(ev.wave_array[ch], ev.wave_array[ch] + 1024));
//            printf("Lollo dice maxmin %f\n", cset.thresh[i]);
//            float* santanas = (float*)min_element(&ev.wave_array[ch][0], &ev.wave_array[ch][0] + 1024);
//            if (&santanas < cset.thresh[i]) {
//                ev.trigCH = ch;
//            }

float getTriggerSource(myEvent *ev, mySetting *st) {
    int i;
    int j;
    float santamax[SANTA_MAX] = {0};
    float santaI[SANTA_MAX] = {0};
    for (i = 0; i < st->Nchan; i++) {
        j = 0;
        while (santamax[i] > st->thresh[i]) {
            santamax[i] = ev->wave_array[i][j];
            santaI[i] = j;
            j++;
        }
        //printf("Santa came from %f", santamax[i]);

    }

    float nastasiomax = N_SAMPLES;
    float nastasioI = 0;
    for (i = 0; i < st->Nchan; i++) {
        if (santaI[i] < nastasiomax) {
            nastasiomax = santaI[i];
            nastasioI = i;
        }
    }
    // printf("Santa came from %f", nastasiomax);
    return nastasioI;
}




