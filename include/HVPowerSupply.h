/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Every tooth in a man's head is more valuable than a diamond.
 * You don't have to brush your teeth — just the ones you want to keep.
 * 
 * 
 */

/* 
 * File:   HVPowerSupply.h
 * Author: leopoldo
 *
 * Created on April 23, 2017, 5:50 PM
 */

#ifndef HVPOWERSUPPLY_H
#define HVPOWERSUPPLY_H



#endif /* HVPOWERSUPPLY_H */

#ifndef __HVPOWERSUPPLY__
#define __HVPOWERSUPPLY__

//#define length(x) (sizeof(x)/sizeof(x[0]))
//#include <fstream>
//#include <string.h>
#include <string>

class HVPowerSupply {
private:
    int handle;
    char *ip;
    int sys;
    char *usr;
    char *pass;
    bool newInitialized;
public:
    HVPowerSupply(); //Default constructor.


    HVPowerSupply(char *ip,
            int sys,
            char *usr,
            char *pass);
    int connect();

    int setBias(short slot,
            unsigned short len,
            unsigned short *channel,
            float *value);

    int getBias(short slot,
            unsigned short len,
            unsigned short *channel,
            float *value);

    int setCurrent(short slot,
            unsigned short len,
            unsigned short *channel,
            float *value);


    int getCurrent(short slot,
            unsigned short len,
            unsigned short *channel,
            float *value);

    int setRampUp(short slot,
            unsigned short len,
            unsigned short *channel,
            float *value);

    int setRampDown(short slot,
            unsigned short len,
            unsigned short *channel,
            float *value);

    int pwOnBehavior(short slot,
            unsigned short len,
            unsigned short *channel,
            bool *value);

    int pwOffBehavior(short slot,
            unsigned short len,
            unsigned short *channel,
            bool *value);

    int pwOnChannel(short slot,
            unsigned short len,
            unsigned short *channel,
            bool *value);

    void LoadBiasVoltage(short slot, char *path);
    std::string GetStatus(short slot, unsigned short len, unsigned short *channel);
    char* GetError();

    ~HVPowerSupply();
};
#endif