//#include <vector.h>
#include <OneWire.h>

#define NPIN 6
int val;
int i;
float mv =0;

float avg =0.;
float stdt =0.;

void setup()
{
Serial.begin(9600);
Serial.println("Ready to measure NeutronStar's core temperature");
}
void loop()
{
  avg = 0.;
  stdt = 0.;
  for(i=0; i< NPIN; i++){
val = analogRead(i);
mv = ( val/1024.0)*500; 
avg+= mv;
stdt += mv*mv;
//Serial.print("TEMPRATURE");
//Serial.print(i);
//Serial.print("=");
//Serial.print(mv);
//Serial.print("*C");
//Serial.println();
}

//Serial.println();
//Serial.print("Media = ");
//Serial.print (avg/(NPIN));
//Serial.print(" +- ");
//Serial.print ((stdt/NPIN-avg*avg/(NPIN*NPIN)));

Serial.print (avg/(NPIN));
Serial.print("\t");
Serial.print (stdt/NPIN-avg*avg/(NPIN*NPIN));
Serial.println();
//Serial.println();
//Serial.println();

delay(1000);
}
