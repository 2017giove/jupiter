/*------Parameters and Pointer to file for configuration--*/
#define     MAXCH                  4

FILE* fp_1;

/*-------Struct read from configuration file--------*/
struct config_param
{
    int   nEvents;
    char  *filename;
    int   delayNs;
    float threshMV;
    bool  triggerEdge;
    int   channel;
    int   trigger_source;
    
};
/*-------Struct for storing Events-----*/

struct myEvent
{
    int trigId;
    int channels;
    int id[MAXCH];
    float time_array[MAXCH][1024];
    float wave_array[MAXCH][1024];
};

/*------------------------*/

/*-------Struct for interface-----------*/

struct interface
{

    int start;
    int quit;
    int time;
    int trigger_frequency;
    

};
