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

using namespace std;

/*------------------------------------------------------------------*/

int main(int argc, char* argv[]) {
    //cout << "no. of arguments: " << argc << endl;
    if (argc != 8) {
        cout << argv[0] << ": Usage" << endl;
        cout << argv[0] << " filename nchans" << endl;
        cout << "          deltaT   : time of the experiment in s" << endl;
        cout << "          [chlist] : Optional, list of Channels to be plotted. Example 0,1,3" << endl;
        cout << "          delay    : Trigger delay in ns" << endl; //<<--- ATTENZIONE: è veramente il tempo morto, o forse il ritardo del trigger (da quando far partire l'acquisizione???
        // cout << "          thresh   : Threshold in mV" << endl;
        // cout << "          edge     : Threshold edge pos=rising edge, neg=falling edge" << endl;
        cout << "          source   : trigger source 1=ch1, 2=ch2, 3=ch1 or ch2" << endl;
        cout << "          Voltage  : Voltage of PMT" << endl;
        cout << "          PMT      : ID number of the chrystal" << endl;

        cout << " Example: drs_jupiter nomefile 300 0 1200 1 1500 500" << endl;
        return 0;
    }
    
    mySetting cset;
    
    char *fileName = argv[1];
    time_t t0 = time(0);
    time_t Current_Time;
    time_t deltaT = atoi(argv[2]);
    time(&Current_Time);

    cset.delayns = atoi(argv[4]);
    //float threshMV = THRESH;

    bool triggerEdge;

    if (strncmp(TRIGGER_EDGE, "neg", 3) == 0) triggerEdge = true;
    else if (strncmp(TRIGGER_EDGE, "pos", 3) == 0) triggerEdge = false;

    int triggerSource = atoi(argv[5]);
    cset.voltage = atof(argv[6]);
    cset.PmtID = atoi(argv[7]);
    cset.thresh = -100.; //2*Voltage*THRESH/1200;

    sprintf(cset.date ,"%s",  asctime(localtime(&Current_Time)));
    cout << cset.date << endl;

    bool chanToPlot[MAXCH];

    myEvent ev;

    int i, j, nBoards;
    DRS *drs;
    DRSBoard *b;

    FILE *f;



    /*
     tset
     *  Delay_ns
     *  Date
     *  Voltage
     *  PMT_ID
     *  threshold
     */
    TTree * Tset = new TTree("tset", "Acquire Settings");
    TBranch * delay = Tset->Branch("Delay_ns", &cset.delayns, "delay/I");
    TBranch * Date = Tset->Branch("Date", cset.date, "date/C");
    TBranch * Volt = Tset->Branch("Voltage", &cset.voltage, "Voltage/F");
    TBranch * PmtId = Tset->Branch("PMT_ID", &cset.PmtID, "PMT/I");
    TBranch * Thresh = Tset->Branch("threshold", &cset.thresh, "thresh/F");

    char rootFileName[130];
    sprintf(rootFileName, "%s.root", fileName);
    TFile f1(rootFileName, "RECREATE");
   Int_t comp = 0;
    f1.SetCompressionLevel(comp);
    
    Tset->Fill();
    Tset->Write();



    /*
     t1
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
    TTree *tree = new TTree("t1", "title");

    char branchDef[STR_LENGTH];
    TBranch * b_trigId = tree->Branch("trigId", &ev.trigId, "trigId/I");
    TBranch * b_channels = tree->Branch("channels", &ev.channels, "channels/I");

    sprintf(branchDef, "id[%d]/I", MAXCH);
    TBranch * b_id = tree->Branch("id", ev.id, branchDef);

    sprintf(branchDef, "time_array[%d][1024]/F", MAXCH);
    TBranch * b_time_array = tree->Branch("time_array", &ev.time_array[0][0], branchDef);

    sprintf(branchDef, "wave_array[%d][1024]/F", MAXCH);
    TBranch * b_wave_array = tree->Branch("wave_array", &ev.wave_array[0][0], branchDef);





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
    b->SetTriggerLevel(cset.thresh / 1000.); // threshold is in V
    b->SetTriggerPolarity(triggerEdge); // positive edge

    /* use following lines to set individual trigger elvels */
    b->SetIndividualTriggerLevel(1, cset.thresh / 1000.);
    b->SetIndividualTriggerLevel(2, cset.thresh / 1000.);

    /*setta la sorgente del trigger in codice binario
     es: CH1=1 CH2=2 CH3=4, CH1_OR_CH2 = 3*/
    b->SetTriggerSource(triggerSource); //fallo piu esperto
    b->SetTriggerDelayNs(cset.delayns); // ns trigger delay

    t0 = time(0);




    // Ciclo Eventi - Acquisizione per un intervallo di tempo deltaT
    j = 0;
    while ((time(0) - t0) < deltaT) {
        /* start board (activate domino wave) */
        b->StartDomino();

        /* wait for trigger - write to console only first time */
        if (j == 0) cout << "Waiting for trigger..." << endl;
        //fflush(stdout);
        while (b->IsBusy());
        if (j == 0) cout << "Trigger found, reading data..." << endl;



        /* read all waveforms */
        b->TransferWaves(0, 8);

        chanToPlot [0] = 1;
        chanToPlot [1] = 0;
        chanToPlot [2] = 0;
        chanToPlot [3] = 0;

        ev.trigId = j;
        ev.channels = 0;

        for (int ch = 0; ch < MAXCH; ch++) {

            if (chanToPlot[ch] == true) {
                ev.id[ch] = 1;
                ev.channels++;

                /* read time (X) array of first channel in ns
                 *    Note: On the evaluation board input #1 is connected to channel 0 and 1 of
                   the DRS chip, input #2 is connected to channel 2 and 3 and so on. So to
                   get the input #2 we have to read DRS channel #2, not #1.
                 */
                b->GetTime(0, 2 * ch, b->GetTriggerCell(0), ev.time_array[ch]);

                /* decode waveform (Y) array of first channel in mV */
                b->GetWave(0, 2 * ch, ev.wave_array[ch]);

            }

        }

        /* print some progress indication */
        if (j % 500 == 0) cout << j << " Eventi misurati, tempo mancante " << deltaT - (time(0) - t0) << " sec" << endl;

        tree->Fill();
        j++;
    }

    tree->Write();

    /* delete DRS object -> close USB connection */
    delete drs;
}
