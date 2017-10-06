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

#include "strlcpy.h"
#include "DRS.h"


#include "HVPowerSupply.h"
#include "CAENHVWrapper.h" 


#include "volt_fit.cpp"
#include "ChargeHist.cpp"
#include "Cs_fit.cpp"
#include "Ba_fit.cpp"
#include "Na_fit.cpp"
#include "Co_fit.cpp" 

#include "Waveform.cpp"
#include "drs_menu.cpp"

#include "defines.h"





using namespace std;

/*------------------------------------------------------------------*/
float getTriggerSource(myEvent *ev, mySetting *st);
void startCapture(char* fileName, mySetting cset);
void preCalibra(char* fileName, mySetting cset);
void initHV(std::vector<myPMTconfig> myPMTs, std::vector<myHVchannel> myChannels);

void Calibra(char* fileName, mySetting cset, std::vector<myPMTconfig> myPMTs, std::vector<myHVchannel> myChannels);
void WebParanoid(char* fileName, mySetting cset, std::vector<myPMTconfig> myPMTs, std::vector<myHVchannel> myChannels);
void Calibra(char* fileName, mySetting cset, std::vector<myPMTconfig> myPMTs, std::vector<myHVchannel> myChannels);
void Web_analyze(char* capturename);
void Calibra_analyze(char* capturename);
void preCalibra_analyze(char* capturename);
void LolFit(char* capturename);
void PMTRangeLT(char * capturename, int PMTid);
void finalizeCapture(char* fileName, mySetting cset);

int main(int argc, char* argv[]) {

    
 //   WaveProfileCosmic("data/cesiotadpoleHH.root");
    
    int targc = 0;
    char* targv[50];
    TApplication theApp("App", &targc, targv);

    vector<std::string> myArgs;
    std::string tempstring;
    for (int j = 0; j < argc; j++) {
        tempstring = argv[j];
        //  printf("%d %s \n", j, tempstring.c_str());
        myArgs.push_back(tempstring);
    }

    // MENU PRINCIPALE (senza argomenti)
    if (argc == 1) {
        mainmenu();
        return 0;
    }


    // ANALISI FILE DI DATI SINGOLO GIA SCELTO
    if (argc == 2) {
        std::string argext = myArgs[1].substr(myArgs[1].length() - 4);

        if (!strcmp(argext.c_str(), "root")) {
            filemenu(myArgs[1].c_str());
        }
        return 0;
    }

    char *acqName = argv[2];
    //    char tmp[STR_LENGTH];

    std::string filen = acqName;
    if (endsWith(filen, ".jpt")) {
        printf("%s\nAre you confusing filename with configuration file??", ERROR_CRUCIAL);
        return -1;
    }


    //COMANDI PER ANALISI DATI
    if (strcmp(myArgs[1].c_str(), "analyze_precalib") == 0) {

        preCalibra_analyze(acqName);
        return 0;

    } else if (strcmp(myArgs[1].c_str(), "analyze_calib") == 0) {

        Calibra_analyze(acqName);
        return 0;

    } else if (strcmp(myArgs[1].c_str(), "chargehist") == 0) {

        ChargeHist(acqName);
        return 0;

    } else if (strcmp(myArgs[1].c_str(), "chargehist_force") == 0) {
        char fileRAWname[STR_LENGTH];
        sprintf(fileRAWname, "%sRAW.root", filenameFromPath(acqName).c_str());
        std::vector<std::string> myrottenhist = list_files("data/", fileRAWname, "");
        removeFileList(myrottenhist);
        ChargeHist(acqName);

        return 0;

    } else if (strcmp(myArgs[1].c_str(), "cs_fit") == 0) {

        Cs_fitall(acqName);
        return 0;

    } else if (strcmp(myArgs[1].c_str(), "wp") == 0) {

        WaveProfile(acqName);
        return 0;

    } else if (strcmp(myArgs[1].c_str(), "wf") == 0) {

        WaveformAll(acqName);
        return 0;

    } else if (strcmp(myArgs[1].c_str(), "tbrowse") == 0) {
        TFile *sorgente_file = TFile::Open(acqName);
        new TBrowser();
        gSystem->ProcessEvents();
        int targc = 0;
        char* targv[50];
        TApplication theApp("App", &targc, targv);
        theApp.Run();

    } else if (strcmp(myArgs[1].c_str(), "analyze_web") == 0) {

        Web_analyze(acqName);
        return 0;

    } else if (strcmp(myArgs[1].c_str(), "LolFit") == 0) {

        LolFit(acqName);
        return 0;


    } else if (strcmp(myArgs[1].c_str(), "voltfit") == 0) {

        // Sceglie il valore migliore del trigger per ogni PMT; ipotesi di linearità
        char tempf[STR_LENGTH];
        char tempf2[STR_LENGTH];

        sprintf(tempf, "data/%s", myArgs[3].c_str());
        sprintf(tempf2, "data/%s.bestcal", myArgs[3].c_str());
        volt_fit(tempf, tempf2, acqName);

        return 0;

        
        //studio linearità in energia
    } else if (strcmp(myArgs[1].c_str(), "lavaninaval") == 0) {

        // Sceglie il valore migliore del trigger per ogni PMT; ipotesi di linearità
        char tempf[STR_LENGTH];

     
        dollar_fit(acqName);

        return 0;

    } else if (strcmp(myArgs[1].c_str(), "PMTRangeLT") == 0) {

        std::vector<int> mylst = PMTList(acqName);
        for (unsigned int k = 0; k < mylst.size(); k++) {
            PMTRangeLT(acqName, mylst[k]);
        }
        getchar();
        return 0;



    } else {

        //COMANDI PER ACQUISIZIONE DATI: inizializza?
        mySetting cset;

        //./drs_expert acquire ananas 100 testACQ.jpt
        char* settingsFilename = argv[4];

        std::vector<myPMTconfig> myPMTs = PMT_ReadConfig(settingsFilename, &cset.triggerSetting, &cset.delayns);
        std::vector<myHVchannel> myChannels = HV_ReadConfig("channelsHV.cfg");

        int maxchan = myPMTs.size();

        allocateSetting(&cset, maxchan);

        cset.Nchan = myPMTs.size();
        cset.deltaT = atoi(argv[3]);

        printf("Canali da acquisire\n");

        for (int i = 0; i < maxchan; i++) {
            cset.voltage[i] = myPMTs[i].volt;
            cset.thresh[i] = -myPMTs[i].thresh; //attenzione al segno -
            cset.PmtID[i] = myPMTs[i].id;

            printf("%d\n", cset.PmtID[i]);
        }

        time_t Current_Time;
        time(&Current_Time);
        sprintf(cset.date, "%s", asctime(localtime(&Current_Time)));




        // OPZIONI ACQUISIZIONI MULTIPLE O SINGOLE
        if (strcmp(myArgs[1].c_str(), "precalib") == 0) {
            initHV(myPMTs, myChannels);
            preCalibra(acqName, cset);
            return 0;

        } else if (strcmp(myArgs[1].c_str(), "calib") == 0) {
            Calibra(acqName, cset, myPMTs, myChannels);
            return 0;

            //            char tempc[STR_LENGTH];
            //            sprintf(tempc, "data/%s.expert", fileName);
            //            std::vector<peak> peaks = peak_load(tempc);
            //
            //            if (resolution[i] < 0) {
            //                printf("Il fit a %f volt sarà rigettato in acqua.\n%s\n", voltage[i], ERROR_FISHERMAN);
            //                i--;
            //            } else {
            //               int realCH = PMTtoCH(PMTid[i], &cset);
            //                cset.thresh[realCH] = cset.voltage[realCH] / voltage[i] * tresh[i];
            //            }

        } else if (strcmp(myArgs[1].c_str(), "web") == 0) {
            WebParanoid(acqName, cset, myPMTs, myChannels);
            return 0;

        } else if (strcmp(myArgs[1].c_str(), "acq") == 0) {
            mySetting_print(&cset);
            initHV(myPMTs, myChannels);
            startCapture(acqName, cset);

        } else if (strcmp(myArgs[1].c_str(), "acq_recover") == 0) {
            mySetting_print(&cset);
            finalizeCapture(acqName, cset);

        }


        //    mySetting_print(&cset);
        //    initHV(myPMTs, myChannels);
        //    startCapture(acqName, cset);



        return 0;
    }

}

void initHV(std::vector<myPMTconfig> myPMTs, std::vector<myHVchannel> myChannels) {
    printf("Loading High Voltage...\n");
    HVPowerSupply *dev = new HVPowerSupply((char *) "192.168.1.2", SY2527, (char *) "admin", (char *) "admin");

    bool power = false;
    //    bool verbose = true;

    unsigned slot = 0;
    unsigned short channel = 0;

    float current;
    float bias;
    float tensione;
    float ramp_up;
    float ramp_down;


    int maxchan = myPMTs.size();
    myHVchannel temp;



    for (int i = 0; i < maxchan; i++) {

        temp = HV_findChannel(myPMTs[i].chname, myChannels);

        power = 1;
        slot = temp.slot;
        tensione = myPMTs[i].volt;
        channel = temp.channel;
        ramp_up = 25;
        ramp_down = 25;

        dev->setBias(slot, 1, &channel, &tensione);
        dev->setRampUp(slot, 1, &channel, &ramp_up);
        dev->setRampDown(slot, 1, &channel, &ramp_down);

        dev->pwOnChannel(slot, 1, &channel, &power);

    }

    initscr();
    start_color();
    init_pair(4, COLOR_BLACK, COLOR_WHITE);
    attron(COLOR_PAIR(4));
    bkgd(COLOR_PAIR(4));

    move(1, 1);
    printw("\tPREPARING THE SNIPER\n");

    move(6, 1);
    menu_printtextimage(menu_getimage());

    refresh();

    int isready = 0;
    int stupid = 1;
    do {
        move(2, 1);

        printw("Loading HIGH VOLTAGE");
        for (int k = 0; k < 20; k++) {
            if (k < stupid)printw(".");
            else printw(" ");
            stupid++;
            if (stupid == 20) stupid = 1;
        }
        printw("\n");

        isready = 1;
        for (int i = 0; i < maxchan; i++) {
            temp = HV_findChannel(myPMTs[i].chname, myChannels);
            slot = temp.slot;
            channel = temp.channel;

            dev->getBias(slot, 1, &channel, &bias);
            dev->getCurrent(slot, 1, &channel, &current);
            printw("CH %d, Voltage: %2.2f [V], Current: %1.2f [uA] ", i, bias, current);

            if (fabs(bias - myPMTs[i].volt) > ramp_up) {
                isready = 0;
            } else {
                printw(" >> Ready to spike ");
            }

            printw("\n");
        }

        printw("\n");
        move(10, 1);
        refresh();
    } while (isready == 0);

    endwin();
    printf("Ready to land in the spacetime continuum.\n");


    delete dev;
}

void startCapture(char* fileName, mySetting cset) {
    /* Apertura files e creazione albero */
    TFile * f1;
    Int_t comp = 4;
    int i;

    time_t t0 = time(0);
    time_t Current_Time;
    time(&Current_Time);
    sprintf(cset.date, "%s", asctime(localtime(&Current_Time)));
    cout << cset.date << endl;



    int triggerSource = cset.triggerSetting; //in futuro da rimuovere

    myEvent ev;
    allocateEvent(&ev, cset.Nchan);

    std::vector<myEvent> totEvents;


    bool triggerEdge;
    if (strncmp(TRIGGER_EDGE, "neg", 3) == 0) triggerEdge = true;
    else if (strncmp(TRIGGER_EDGE, "pos", 3) == 0) triggerEdge = false;



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

    Tset->Branch("triggersetting", &cset.triggerSetting, "triggersetting/I");

    Tset->Branch("description", cset.description, "description/C");

    sprintf(branchDef, "Voltage[%d]/F", WANDANA_MAX);
    Tset->Branch("Voltage", cset.voltage, branchDef);

    sprintf(branchDef, "PMT_ID[%d]/I", WANDANA_MAX);
    Tset->Branch("PMT_ID", cset.PmtID, branchDef);

    sprintf(branchDef, "threshold[%d]/F", WANDANA_MAX);
    Tset->Branch("threshold", cset.thresh, branchDef);



    Tset->Fill();

    // FILE* thermo = popen("./ThermoParanoid 100");


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

    //    TBranch * b_eventId = tree->Branch("eventID", &ev.eventID, "eventID/I");



    sprintf(branchDef, "time_array[%d][1024]/F", WANDANA_MAX);
    TBranch * b_time_array = tree->Branch("time_array", &ev.time_array[0][0], branchDef);

    sprintf(branchDef, "wave_array[%d][1024]/F", WANDANA_MAX);
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
        return;
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
    b->SetIndividualTriggerLevel(2, cset.thresh[1] / 1000.);
    b->SetIndividualTriggerLevel(3, cset.thresh[2] / 1000.);
    b->SetIndividualTriggerLevel(4, cset.thresh[3] / 1000.);


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


    Exotourbillion totaltr;
    Exotourbillion tr;
    Exotourbillion tr2;
    totaltr.WashAndRefill();
    while ((time(0) - t0) < cset.deltaT) {
        /* start board (activate domino wave) */

        tr2.GoToBed();
        tr.WakeFromBed();
        b->StartDomino();

        /* wait for trigger - write to console only first time */
        if (totevents == 0) cout << "Waiting for trigger..." << endl;

        while (b->IsBusy());
        tr.GoToBed();
        tr2.WakeFromBed();


        if (totevents == 0) cout << "Trigger found, session started..." << endl;

        /* read all waveforms */
        b->TransferWaves(0, 8);



        for (int ch = 0; ch < cset.Nchan; ch++) {

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


        //   totEvents.push_back(ev);
        tree->Fill();

        /* print some progress indication */
        printf("%d ev - %d sec rem.", totevents, (int) (cset.deltaT - (time(0) - t0)));
        //printf("\n%d %d %f\n",cset.deltaT, (time(0)-t0),(float)((time(0) - t0) / (float)cset.deltaT));
        printStatus((float) ((time(0) - t0) / (float) cset.deltaT));

        totevents++;



    }

    totaltr.GoToBed();
    // is trig?
    TBranch * b_trigId = tree -> Branch("trigCH", &ev.trigCH, "trigCH/I");
    for (i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        ev.trigCH = getTriggerSource(&ev, &cset);
        // printf("%d\n\n", ev.trigCH);
        b_trigId->Fill();
    }

    tree->Write();
    f1->Write();
    f1->Close();
    printf("\nTempo totale\t%lf\n", totaltr.TotalUsedInk());
    printf("\nTempo DRS:\t%lf\t(%lf )\nTempo PC:\t%lf\t(%lf )\n", tr.TotalUsedInk(), tr2.TotalUsedInk(), tr.TotalUsedInk() / (tr.TotalUsedInk() + tr2.TotalUsedInk()), (double) tr.TotalUsedInk() / (tr2.TotalUsedInk() + tr.TotalUsedInk()));
    printf("\n*** ACQUISITION IS COMPLETE ***\n\n\n");


    //  fclose(tempofile    );
    /* delete DRS object -> close USB connection */
    delete drs;
}

/**
 * Use this function to recover an unluckily crashed acquisition
 * @param fileName
 * @param cset
 */
void finalizeCapture(char* fileName, mySetting cset) {
    TFile * f1;
    Int_t comp = 4;

    char rootFileName[130];
    sprintf(rootFileName, "data/%s.root", fileName);
    f1 = (TFile*) new TFile(rootFileName, "UPDATE");
    f1->SetCompressionLevel(comp);

    myEvent ev;
    allocateEvent(&ev, cset.Nchan);
    int i;

    TTree * tree;
    tree = (TTree*) new TTree("t1", "title");

    tree->SetBranchAddress("trigCH", &ev.trigCH);
    tree->SetBranchAddress("wave_array", ev.wave_array);
    tree->SetBranchAddress("time_array", ev.time_array);


    // is trig?
    TBranch * b_trigId = tree -> Branch("trigCH", &ev.trigCH, "trigCH/I");
    for (i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        ev.trigCH = getTriggerSource(&ev, &cset);
        // printf("%d\n\n", ev.trigCH);
        b_trigId->Fill();
    }

    tree->Write();
    f1->Write();
    f1->Close();

    new TBrowser();
    gSystem->ProcessEvents();
    int targc = 0;
    char* targv[50];
    TApplication theApp("App", &targc, targv);
    theApp.Run();



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
    float santamax[WANDANA_MAX] = {0};
    float santaI[WANDANA_MAX] = {0};
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











// ****************************************************************
// EXPERT's SQUARE

void preCalibra(char* fileName, mySetting cset) {
    printf("He is washing his fountain pen...\n");
    //    int thresh;
    char tmp[STR_LENGTH];
    float frac[MAXCH];

    for (int i = 0; i < cset.Nchan; i++) {
        frac[i] = cset.thresh[i] / 4;
    }

    for (int k = 1; k < 7; k++) {

        for (int i = 0; i < cset.Nchan; i++) {
            cset.thresh[i] = frac[i] * k;
        }

        sprintf(tmp, "%s_%d_%d.th", fileName, (int) cset.voltage[0], k);

        char temp2[STR_LENGTH];
        sprintf(temp2, "%s.root", tmp);
        std::vector<std::string> myrottenfish = list_files("data/", temp2, "");
        if (myrottenfish.size() == 0) {
            startCapture(tmp, cset);
        } else {
            printf("You already acquired %s.\n", tmp);
        }
    }


    for (int k = 1; k <= 4; k++) {

        for (int i = 0; i < cset.Nchan; i++) {
            cset.thresh[i] = frac[i]*4 * k;
        }

        sprintf(tmp, "%s_%d_%d.th", fileName, (int) cset.voltage[0], 4 * k);

        char temp2[STR_LENGTH];
        sprintf(temp2, "%s.root", tmp);
        std::vector<std::string> myrottenfish = list_files("data/", temp2, "");
        if (myrottenfish.size() == 0) {
            startCapture(tmp, cset);
        } else {
            printf("You already acquired %s.\n", tmp);
        }
    }

    preCalibra_analyze(fileName);

}

void WebParanoid(char* fileName, mySetting cset, std::vector<myPMTconfig> myPMTs, std::vector<myHVchannel> myChannels) {
    mySetting_print(&cset);
    initHV(myPMTs, myChannels);
    char tmp[STR_LENGTH];
    sprintf(tmp, "%s.web", fileName);
    startCapture(tmp, cset);

    Web_analyze(fileName);

}

void Calibra(char* fileName, mySetting cset, std::vector<myPMTconfig> myPMTs, std::vector<myHVchannel> myChannels) {
    printf("He is filling his fountain pen again...\n");

    char tmp[STR_LENGTH];
    sprintf(tmp, "data/%s.besttrigger", fileName);
    std::vector<peak> peaks = peak_load(tmp);




    for (int volt = 1500; volt < 1900; volt += 50) {

        for (int i = 0; i < cset.Nchan; i++) {


            // find pmt position in peaks file from pmt id
            int j = NOT_FOUND_INT;
            for (unsigned int k = 0; k < peaks.size(); k++) {
                if (peaks[k].PMTid == CHtoPMT(i, &cset)) {
                    j = k;
                }
            }

            myPMTs[i].volt = volt;
            cset.voltage[i] = volt;
            cset.thresh[i] = peaks[j].thresh * volt / peaks[j].voltage;
            printf("Soglia a %f\n", cset.thresh[i]);
        }


        initHV(myPMTs, myChannels);

        sprintf(tmp, "%s_%d.cal", fileName, (int) cset.voltage[0]);
        // printf("%s_%d.cal", fileName, (int) cset.voltage[0]);

        char temp2[STR_LENGTH];
        sprintf(temp2, "%s.root", tmp);
        std::vector<std::string> myrottenfish = list_files("data/", temp2, "");
        if (myrottenfish.size() == 0) {
            startCapture(tmp, cset);
        } else {
            printf("You already acquired %s.\n", tmp);
        }
    }


    Calibra_analyze(fileName);

}





//Si abilita con 
//  su 
//  setenforce 0
// da terminale, poi si può acquisire dal sito http://lxsfera.roma1.infn.it

void Web_analyze(char* capturename) {
    char capturename_[STR_LENGTH];
    char temp1[STR_LENGTH];
    char temp2[STR_LENGTH];
    //    char rottentemp[STR_LENGTH];

    // Rimuove i file vecchi eventualmente presenti
    std::vector<std::string> myrottenfish = list_files("data/", capturename, ".RAW.root");
    removeFileList(myrottenfish);

    std::vector<std::string> myrottenhist = list_files("data/", capturename, ".histoweb.root");
    removeFileList(myrottenhist);



    // Cerca tutti i file appartenenti alla presa dati indicata
    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myfiles = list_files("data/", capturename, ".web.root");

    // Crea istogramma carica
    for (unsigned int i = 0; i < myfiles.size(); i++) {
        sprintf(capturename_, "data/%s", myfiles[i].c_str());
        //        printf("\n\n%d\t%d\t%s\n", i, myfiles.size(), myfiles[i].c_str());
        ChargeHist(capturename_, ".histoweb");
    }

    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myHistfiles = list_files("data/", capturename, ".histoweb.root");

    for (unsigned int i = 0; i < myHistfiles.size(); i++) {

        sprintf(temp1, "data/%s", myHistfiles[i].c_str());
        TFile *sorgente_file = TFile::Open(temp1);

        mySetting st;
        TTree* tset1 = (TTree*) sorgente_file->Get("tset");
        mySetting_get(tset1, &st);

        for (int j = 0; j < st.Nchan; j++) {
            int PMTid = CHtoPMT(j, &st);
            //            int voltage = st.voltage[j];
            sprintf(temp2, "data/%s_%d.calfish", capturename, PMTid);
            printf("\nFilename iniziale %s \n>> Salvato in %s\n", temp1, temp2);
            Cs_getPeak(temp1, PMTid, temp2);
        }


    }
    printf("Esecuzione terminata\n");
}

void Calibra_analyze(char* capturename) {
    char capturename_[STR_LENGTH];
    //    char temp1[STR_LENGTH];
    //    char temp2[STR_LENGTH];
    //    char rottentemp[STR_LENGTH];

    // Rimuove i file vecchi eventualmente presenti
    std::vector<std::string> myrottenfish = list_files("data/", capturename, ".calfish");
    removeFileList(myrottenfish);

    std::vector<std::string> myrottenhist = list_files("data/", capturename, ".histcal.root");
    removeFileList(myrottenhist);

    std::vector<std::string> myrottencal = list_files("data/", capturename, ".bestcal");
    removeFileList(myrottencal);

    std::vector<std::string> myrottenia = list_files("data/", capturename, ".bestcal.ia");
    removeFileList(myrottenia);



    // Cerca tutti i file appartenenti alla presa dati indicata
    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myfiles = list_files("data/", capturename, ".cal.root");

    // Crea istogramma carica
    for (unsigned int i = 0; i < myfiles.size(); i++) {
        sprintf(capturename_, "data/%s", myfiles[i].c_str());
        //        printf("\n\n%d\t%d\t%s\n", i, myfiles.size(), myfiles[i].c_str());
        ChargeHist(capturename_, ".histcal");
    }

    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myHistfiles = list_files("data/", capturename, ".histcal.root");

    char tempfpath[STR_LENGTH];
    for (unsigned int i = 0; i < myHistfiles.size(); i++) {
        sprintf(tempfpath, "data/%s", myHistfiles[i].c_str());
        Cs_fitall(tempfpath);
    }

    // Sceglie il valore migliore del trigger per ogni PMT; ipotesi di linearità
    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myFish = list_files("data/", capturename, ".calfish");

    char tempf[STR_LENGTH];
    char tempf2[STR_LENGTH];
    for (unsigned int f = 0; f < myFish.size(); f++) {
        sprintf(tempf, "data/%s", myFish[f].c_str());
        sprintf(tempf2, "data/%s.bestcal", capturename);
        volt_fit(tempf, tempf2, capturename);
    }


}

void preCalibra_analyze(char* capturename) {

    char capturename_[STR_LENGTH];
    char temp1[STR_LENGTH];
    char temp2[STR_LENGTH];
    //    char rottentemp[STR_LENGTH];


    // Rimuove i file vecchi eventualmente presenti
    std::vector<std::string> myrottenfish = list_files("data/", capturename, ".fish");
    removeFileList(myrottenfish);

    std::vector<std::string> myrottenhist = list_files("data/", capturename, ".histprec.root");
    removeFileList(myrottenhist);

    std::vector<std::string> myrottentrigger = list_files("data/", capturename, ".besttrigger");
    removeFileList(myrottentrigger);

    std::vector<std::string> myrottenia = list_files("data/", capturename, ".besttrigger.ia");
    removeFileList(myrottenia);



    // Cerca tutti i file appartenenti alla presa dati indicata
    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myfiles = list_files("data/", capturename, ".th.root");

    // Crea istogramma carica
    for (unsigned int i = 0; i < myfiles.size(); i++) {
        sprintf(capturename_, "data/%s", myfiles[i].c_str());
        //        printf("\n\n%d\t%d\t%s\n", i, myfiles.size(), myfiles[i].c_str());
        ChargeHist(capturename_, ".histprec");
    }

    // Fit con CsFit (CESIO) >> Mette i valori del picco in un file
    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myHistfiles = list_files("data/", capturename, ".histprec.root");


    for (unsigned int i = 0; i < myHistfiles.size(); i++) {
        //riscrivere utilizzand Cs fit, senza dover riscrivere lo stesso codice
        sprintf(temp1, "data/%s", myHistfiles[i].c_str());
        TFile *sorgente_file = TFile::Open(temp1);

        mySetting st;
        TTree* tset1 = (TTree*) sorgente_file->Get("tset");
        mySetting_get(tset1, &st);

        for (int j = 0; j < st.Nchan; j++) {
            int PMTid = CHtoPMT(j, &st);
            int voltage = st.voltage[j];
            sprintf(temp2, "data/%s_%d_%d.fish", capturename, voltage, PMTid);
            printf("\nFilename iniziale %s \n>> Salvato in %s\n", temp1, temp2);
            Cs_getPeak(temp1, PMTid, temp2);
        }



    }

    // Sceglie il valore migliore del trigger per ogni PMT; ipotesi di linearità
    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myFish = list_files("data/", capturename, ".fish");

    char tempf[STR_LENGTH];
    char tempf2[STR_LENGTH];
    for (unsigned int f = 0; f < myFish.size(); f++) {
        sprintf(tempf, "data/%s", myFish[f].c_str());
        sprintf(tempf2, "data/%s.besttrigger", capturename);
        bestresolution_find(tempf, tempf2);
    }

}

/**
 * [press a button an dream - don't even say a word]
 * Analizza automaticamente una serie di dati relativa all'acquisizione con nome capturenae
 * @param capturename
 */


void LolFit(char* capturename) {
    char capturename_[STR_LENGTH];
    char temp1[STR_LENGTH];
    char temp2[STR_LENGTH];
    //    char rottentemp[STR_LENGTH];


    // Rimuove i file vecchi eventualmente presenti
    std::vector<std::string> myrottenfish = list_files("data/", capturename, ".calfish");
    removeFileList(myrottenfish);

    std::vector<std::string> myrottenhist = list_files("data/", capturename, "0hist.root");
    removeFileList(myrottenhist);

    std::vector<std::string> myrottencal = list_files("data/", capturename, ".bestcal");
    removeFileList(myrottencal);


    // Cerca tutti i file appartenenti alla presa dati indicata
    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myfiles = list_files("data/", capturename, "0.root");

    // Crea istogramma carica
    for (unsigned int i = 0; i < myfiles.size(); i++) {
        sprintf(capturename_, "data/%s", myfiles[i].c_str());
        //printf("\n\n%d\t%d\t%s\n", i, (int)myfiles.size(), myfiles[i].c_str());
        ChargeHist(capturename_, "0hist");
    }


    // sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myHistfiles = list_files("data/", capturename, "0hist.root");

    for (unsigned int i = 0; i < myHistfiles.size(); i++) {

        sprintf(temp1, "data/%s", myHistfiles[i].c_str());
        TFile *sorgente_file = TFile::Open(temp1);

        mySetting st;
        TTree* tset1 = (TTree*) sorgente_file->Get("tset");
        mySetting_get(tset1, &st);

        for (int j = 0; j < st.Nchan; j++) {
            int PMTid = CHtoPMT(j, &st);
            //            int voltage = st.voltage[j];
            sprintf(temp2, "data/%s_%d.calfish", capturename, PMTid);
            printf("\nFilename iniziale %s \n>> Salvato in %s\n", temp1, temp2);
            Cs_getPeak(temp1, PMTid, temp2);
        }
        Cs_fitsumofchannels(temp1);

    }


    // Sceglie il valore migliore del trigger per ogni PMT; ipotesi di linearità
    //  sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myFish = list_files("data/", capturename, ".calfish");

    char tempf[STR_LENGTH];
    char tempf2[STR_LENGTH];
    for (unsigned int f = 0; f < myFish.size(); f++) {
        sprintf(tempf, "data/%s", myFish[f].c_str());
        sprintf(tempf2, "data/%s.bestcal", capturename);
        volt_fit(tempf, tempf2, capturename);
    }



}

void PMTRangeLT(char * capturename, int PMTid) {

    char capturename_[STR_LENGTH];
    char temp1[STR_LENGTH];
    //    char temp2[STR_LENGTH];
    char rottentemp[STR_LENGTH];


    sprintf(capturename_, "%s_", capturename);
    std::vector<std::string> myFish = list_files("data/", capturename, "fish");

    //  gStyle->SetOptFit(1111);
    sprintf(rottentemp, "data/%s_%d.root", capturename, PMTid);
    TFile *FOut = new TFile(rottentemp, "UPDATE");
    TCanvas *c41 = new TCanvas("Fish", PLOTS_TITLE, 640, 480);




    TGraph * confPlot = new TGraph();
    TGraph * confPlotProblematic = new TGraph();

  
    
    confPlotProblematic->SetMarkerColor(kRed);
    confPlot->SetMarkerStyle(8);
    confPlot->SetMarkerSize(1);
    confPlotProblematic->SetMarkerSize(1);
    confPlotProblematic->SetMarkerStyle(8);




    for (unsigned int i = 0; i < myFish.size(); i++) {
        sprintf(temp1, "data/%s", myFish[i].c_str());
        std::vector<peak> peaks = peak_load(temp1);

        for (unsigned int j = 0; j < peaks.size(); j++) {
            if (PMTid == peaks[j].PMTid) {
                TLatex * lbl = new TLatex(peaks[j].voltage + 0.2, -peaks[j].thresh + 0.2, Form("%1.5f", peaks[j].IA*100));
                lbl->SetTextSize(0.025);
                lbl->SetTextFont(42);


                if (peaks[j].anyproblems == 0) {
                    confPlot->SetPoint((confPlot->GetN()), peaks[j].voltage, -peaks[j].thresh);

                } else {
                    confPlotProblematic->SetPoint((confPlotProblematic->GetN()), peaks[j].voltage, -peaks[j].thresh);
                    lbl->SetTextColor(kRed);
                }


                confPlot->GetListOfFunctions()->Add(lbl);
            }
        }
    }

  c41->SetLogy();
      confPlot->GetYaxis()->SetRangeUser(0, 500);
    confPlotProblematic->GetYaxis()->SetRangeUser(0, 500);

    confPlot->SetMinimum(0);
    confPlot->SetMaximum(500);
    
       confPlotProblematic->SetMinimum(0);
    confPlotProblematic->SetMaximum(500);
    
       
    confPlot->Draw("ap");
    confPlotProblematic->Draw("psame");

    confPlot->Write("lollo");
    confPlotProblematic->Write("lollo");



    sprintf(rottentemp, "Ia - PMT %d", PMTid);
    confPlot->SetTitle(rottentemp);
    confPlot->GetXaxis()->SetTitle("Tensione (V)");
    confPlot->GetYaxis()->SetTitle("Soglia Trigger (-mV)");

    sprintf(rottentemp, "img/%s_%d_ia.png", capturename, PMTid);
    c41->SaveAs(rottentemp);
    c41->Write();
    FOut->Close();


}

