/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Every tooth in a man's head is more valuable than a diamond.
 * You don't have to brush your teeth — just the ones you want to keep.
 * 
 * 
 */

/* 
 * File:   ThermoParanoid.cpp
 * Author: sfera
 *
 * Created on June 7, 2017, 10:11 PM
 */

#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <stdio.h> 
#include <ostream>
#include <istream>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

#include <unistd.h>

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Usage: ./ThermoParanoid time\n");
        return -1;
    }

    int deltat = atoi(argv[1]);

    printf("I'll be collapsing charges for %d secs\n", deltat);

    char readBuffer[1024];
    int numBytesRead;
    
    //Far scegliere in automatico il dispositivo giusto... Su bash si fa con ls -l /dev/ttyACM* si può fare anche qui...
    char serialPortFilename[] = "/dev/ttyACM0";

    FILE *serPort = fopen(serialPortFilename, "r");
    time_t t0 = time(0);
    if (serPort == NULL) {
        printf("There is a problem and it's deeper: please buy a microSD card and an adapter.\n");
        return 0;
    }
//
    FILE* myfile = fopen("/home/sfera/Desktop/jupiter/santatemp2.txt", "w");
    if (myfile== NULL){
        printf("ho finito l'inchiostro per scrivere su file \n");
    }
    
    fflush(stdout);
        
    while ((time(0) - t0) < deltat) {
        memset(readBuffer, 0, 1024);
        fread(readBuffer, sizeof (char), 1024, serPort);
        float f1, f2;
        if (sizeof (readBuffer) != 0) {

            fprintf(myfile, readBuffer);
          printf(readBuffer);
      
        }
    }
    
    printf("Saved in santatemp.txt for future analysis.\n");
    fclose(myfile);
    return 0;
}
