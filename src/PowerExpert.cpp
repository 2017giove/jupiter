/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Every tooth in a man's head is more valuable than a diamond.
 * You don't have to brush your teeth — just the ones you want to keep.
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <fstream>
#include <iostream>
#include "HVPowerSupply.h"
#include "CAENHVWrapper.h"
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <math.h>

#define msleep(x) usleep((x)*1000)
#define Nch 12



unsigned slot = 0;
unsigned short channel = 0;

int main(int argc, char* argv[]) {
    printf("******\tWelcome to the Pleasure Island\t*****\n");
    printf("The Land of Toys (Italian: Paese dei balocchi) is a fictional location in the Italian novel The Adventures of Pinocchio (1883).\nIt is disguised as a haven of freedom and anarchy for boys and occasionally girls.\nIn the Disney film adaptation of the novel, the land is renamed as Pleasure Island.\nThe size and nature of such location is unclear (the Disney adaptation depicts it as a large, island-sized amusement park, whereas the novel implies it is at least as large as a township);\nthe ambiguity in the original name (paese can mean country or land, but also town or village) adds to the confusion.\nTo its unsuspecting visitors, it appears to be a fantastic place where boys and girls can do whatever they want with no consequences or law.\nHowever, its real use is for a slave trade.\nLocated in the fictional land of Cocagne,[citation needed] Pleasure Island serves as a haven for wayward boys and girls, allowing them to act as they please without recrimination.\nHowever, the truer and more sinister purpose of Pleasure Island is eventually revealed as it begins to physically transform the boys and girls into donkeys, apparently by means of a curse.\n");
    printf("***************************************************\n");


    bool power = false;
    bool verbose = true;

    float current;
    float bias;
    float tensione;
    float ramp_up;
    float ramp_down;

    HVPowerSupply *dev = new HVPowerSupply((char *) "192.168.1.2", SY2527, (char *) "admin", (char *) "admin");

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'p':
                    power = ((atoi(argv[++i]) > 0) ? true : false);
                    break;
                case 's':
                    slot = atoi(argv[++i]);
                    break;
                case 'c':
                    channel = atoi(argv[++i]);
                    break;
                case 'q':
                    verbose = false;
                    break;
                case 'v':
                    tensione = atoi(argv[++i]);
                    break;
                case 'U':
                    ramp_up = atoi(argv[++i]);
                    break;
                case 'D':
                    ramp_down = atoi(argv[++i]);
                    break;
                default:
                    printf("wrong option\n");
                    return 1;
            }
        }

    }

    power = 1;
    slot = 2;
    tensione = 1000;
    channel = 9;
    ramp_up = 25;
    ramp_down = 25;

    dev->pwOnChannel(slot, 1, &channel, &power);

    unsigned short i;
    for (i = 0; i < 10; i++) {
        dev->getBias(slot, 1, &i, &bias);
        dev->getCurrent(slot, 1, &i, &current);
        printf("Channel: %d, Voltage: %2.2f [V], Current: %1.2f [uA].%c\n", i, bias, current, 0x0D);
    }

    //    dev->setBias(slot, 1, &channel, &tensione);
    //    dev->setRampUp(slot, 1, &channel, &ramp_up);
    //    dev->setRampDown(slot, 1, &channel, &ramp_down);

    //    printf("\n");
    //    do {
    //        dev->getBias(slot, 1, &channel, &bias);
    //        dev->getCurrent(slot, 1, &channel, &current);
    //        if (verbose) {
    //            printf("Channel: %d, Voltage: %2.2f [V], Current: %1.2f [uA].%c", channel, bias, current, 0x0D);
    //        }
    //
    //    } while (fabs((bias - tensione)) > ramp_up);
    //
    //    printf("\n");
    //
    delete dev;
    //    return 1;

}