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


#ifndef JUPITER

#include "TDirectory.h"
#include "TKey.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraph2D.h"

#include "TPaveText.h"

#include "TF1.h"
#include "TMath.h"
#include "TGraph.h"
#include "TSystemFile.h"
#include "TSystemDirectory.h"
#include "TList.h"

#include "TROOT.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TSystem.h"
#include "TBranch.h"
#include "TStyle.h"

#include "TVector.h"

#include "TRint.h"
#include <TApplication.h>


#include <vector>
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <stdio.h> 
#include <ostream>

//#include "Waveform.cpp"



#define JUPITER "Esperienza GioVe"
#define PLOTS_TITLE "Fisherman's Plot"
#define ERROR_CRUCIAL "Call a qualified expert: 27th Alpes Ave, view on Caprera Circus and all of Rome."
#define ERROR_DEEPER "THERE IS A PROBLEM AND IT IS DEEPER."
#define ERROR_FISHERMAN "We did not find what you were looking for: the fish escaped the net."
#define NOT_FOUND_INT -241
#define NOT_CARING_INT 433
#define NOT_CREDIBLE 233
#define NOT_PROBABLE 232
#define NOT_COMPTON 231
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
#define WANDANA_MAX 4

#define CHARGE_STEP 5 //larghezza del bin nel tprofile della forma d'onda

#define MAXCH 4  //non esiste
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
    float time_array[WANDANA_MAX][N_SAMPLES];
    float wave_array[WANDANA_MAX][N_SAMPLES];
};

struct mySetting {
    char date[STR_LENGTH];
    int Nchan;
    int deltaT;
    float voltage[WANDANA_MAX];
    int PmtID[WANDANA_MAX];
    float thresh[WANDANA_MAX];
    int triggerSetting;
    int delayns;
    char description[10 * STR_LENGTH];
    //Aggiungere altri parametri rilevanti come...

};

struct myHVchannel {
    char name[STR_LENGTH];
    int slot;
    int channel;
};

struct myPMTconfig {
    int id;
    float volt;
    float thresh;
    char chname[STR_LENGTH];
};

struct peak {
    float sigma;
    float peakpos;
    float peakvalue;
    float nBG;
    float nSGN;
    float voltage;
    float PMTid;
    float resolution;
    float thresh;
    int anyproblems;
};

int PMTtoCH(int PMT, mySetting* st);

void allocateSetting(struct mySetting* st, int NCHAN) {
    int i;
    for (i = 0; i < WANDANA_MAX; i++) {
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
    for (i = 0; i < WANDANA_MAX; i++) {
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

int get_minimum_pos(float* test, int start, int end) {

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

bool isaspike(float* test, int triggerpos, float baseline) {

    float testint = -BoundIntegral(test, triggerpos, triggerpos + 100);
    int minpos = get_minimum_pos(test, triggerpos, triggerpos + 100);
    float maxspike = -test[minpos];
    float troppospike = (testint - baseline) / (maxspike - baseline);
    if (troppospike < 0.1) {
        //  printf("\nBrava gente: bomba %f\ntestint %f\nminpos %f\nmaxspike %f\ntroppospike %f\n\n", test[0], testint,minpos, maxspike,troppospike);
        return 1;
    }
    return 0;
}

bool issaturated(float* test, int triggerpos) {
    int nsat = 0;

    for (int i = triggerpos; i < N_SAMPLES; i++) {

        if (test[i] < -490) {
            //   printf("\n\n%f\n\n", test[i]);
            nsat++;
        }
    }


    if (nsat > 100) {
        return 1;
    } else {
        return 0;
    }

}

std::vector<std::string> list_files(const char *dirname = "data/",
        const char *prefix = "",
        const char *suffix = ".root"
        ) {

    std::vector<std::string> myfiles;

    if (!dirname || !(*dirname)) return myfiles; // just a precaution
    TString pwd(gSystem->pwd());
    TSystemDirectory dir(dirname, dirname);
    TList *files = dir.GetListOfFiles();
    gSystem->cd(pwd.Data()); // bug fix for ROOT prior to 5.34
    if (files) {
        TSystemFile *file;
        TString fname;
        TIter next(files);
        while ((file = (TSystemFile*) next())) {
            fname = file->GetName();
            if (!(file->IsDirectory()) &&
                    (!prefix || !(*prefix) || fname.BeginsWith(prefix)) &&
                    (!suffix || !(*suffix) || fname.EndsWith(suffix))) {
                myfiles.push_back(file->GetName());
                std::cout << fname.Data() << std::endl;
            }
        }
    }
    delete files;
    return myfiles;
}

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

void mySetting_histoprint(mySetting *st, int PMTid) {
    char temp[STR_LENGTH];

    int CH = PMTtoCH(PMTid, st);

    sprintf(temp, "PMT %d - data acquired on %s ", st->PmtID[CH], st->date);
    TText *label1 = new TText();
    label1->SetNDC();
    label1->SetTextSize(0.03);
    label1->DrawText(0.01, 0.04, temp);

    sprintf(temp, "Voltage = %d V, Threshold = %2.0f mV", st->voltage[CH], st->thresh[CH]);
    TText *label2 = new TText();
    label2->SetNDC();
    label2->SetTextSize(0.03);
    label2->DrawText(0.01, 0.0, temp);

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

bool startsWith(std::string& src, char* tk) {
    std::string token = tk;
    return src.substr(0, token.length()) == token;
}

bool endsWith(const std::string& a, char* bb) {
    std::string b = bb;
    if (b.size() > a.size()) return false;
    return std::equal(a.begin() + a.size() - b.size(), a.end(), b.begin());
}

std::vector<myPMTconfig> PMT_ReadConfig(char* filename, int* triggersource, int* delayns) {


    std::ifstream myfile1;
    myfile1.open(filename);

    std::string myline;

    std::vector<myPMTconfig> myPMTs;

    myPMTconfig temp;
    char tempstring [STR_LENGTH];
    while (std::getline(myfile1, myline)) {
        if (startsWith(myline, "#") == 0) {
            std::istringstream strm(myline);

            if (startsWith(myline, "triggersource")) {
                sscanf(myline.c_str(), "%s %d", tempstring, triggersource);
                //  printf("\n%s\t%d\n", tempstring, *triggersource);
            } else if (startsWith(myline, "delay")) {
                sscanf(myline.c_str(), "%s %d", tempstring, delayns);
                // printf("\n%s\t%d\n", tempstring, *delayns);
            } else {


                if (strm >> temp.id >> temp.volt >> temp.thresh >> temp.chname) {

                    myPMTs.push_back(temp);

                }
            }
        }
    }

    //    for (int i = 0; i < myPMTs.size(); i++) {
    //        std::cout << myPMTs[i].id << " " << myPMTs[i].volt << " " << myPMTs[i].thresh << " " << myPMTs[i].chname << std::endl;
    //    }

    return myPMTs;
}

std::vector<myHVchannel> HV_ReadConfig(char* filename) {


    std::ifstream myfile1;
    myfile1.open(filename);

    std::string myline;

    std::vector<myHVchannel> myChannels;

    myHVchannel temp;

    while (std::getline(myfile1, myline)) {
        if (startsWith(myline, "#") == 0) {

            std::istringstream strm(myline);
            if (strm >> temp.name >> temp.slot >> temp.channel) {
                myChannels.push_back(temp);

            }
        }
    }

    //    for (int i = 0; i < myChannels.size(); i++) {
    //        std::cout << myChannels[i].name << " " << myChannels[i].slot << " " << myChannels[i].channel << std::endl;
    //    }

    return myChannels;
}

myHVchannel HV_findChannel(char* name, std::vector<myHVchannel> myChannels) {

    for (int i = 0; i < myChannels.size(); i++) {
        if (strcmp(name, myChannels[i].name) == 0) {
            return myChannels[i];
        }
    }

    myHVchannel nullCH;
    nullCH.channel = NOT_FOUND_INT;
    return nullCH;
}

void removeFileList(std::vector<std::string> rottenf) {
    char temp[STR_LENGTH];
    for (int k = 0; k < rottenf.size(); k++) {
        sprintf(temp, "data/%s", rottenf[k].c_str());
        printf("removing %s\n", temp);
        remove(temp);
    }
}

void peak_save(char* wheretosave, peak* mypeak) {
    std::ofstream savefile(wheretosave, std::ios_base::app);
    if (mypeak->anyproblems == NOT_CREDIBLE) {
        savefile << "#";
    }
    savefile << mypeak->PMTid << " " << mypeak->voltage << " " << mypeak->thresh << " " << mypeak->peakpos << " " << mypeak->sigma << " " << mypeak->peakvalue << " " << mypeak->nSGN << " " << mypeak->nBG << " " << mypeak->resolution << " " << mypeak->anyproblems << std::endl;

}

std::vector<peak> peak_load(char* peaksfile) {

    std::ifstream myfile1;
    std::string myline;
    printf("file %s\n", peaksfile);
    myfile1.open(peaksfile);

    std::vector<peak> peaks;


    int i = 0;

    while (std::getline(myfile1, myline)) {
        peak temp;
        std::istringstream strm(myline);
        if (strm >> temp.PMTid >> temp.voltage >> temp.thresh >> temp.peakpos >> temp.sigma >> temp.peakvalue >> temp.nSGN >> temp.nBG >> temp.resolution >> temp.anyproblems) {
            std::cout << i << " " << temp.PMTid << " " << temp.voltage << " " << temp.thresh << " " << temp.peakpos << " " << temp.sigma << " " << temp.peakvalue << " " << temp.nSGN << " " << temp.nBG << " " << temp.resolution << " " << temp.anyproblems << std::endl;
            i++;
            peaks.push_back(temp);
        } else {
            printf("(riga ignorata)\n");
        }
    }

    return peaks;
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




#endif