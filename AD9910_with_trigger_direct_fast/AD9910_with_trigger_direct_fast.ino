//This is a program where the data is not transfered from the SPI but is set in this program.
//
#include <Arduino.h>
#include "main.h"
String request="";
int M, K, H, A, MenuPos;
#define INIT_M 150
#define INIT_K 0
#define INIT_H 0
#define INIT_A 15 
//const int DDS_trigger= A8;//这个脚用来trigger开始
int  TriggerState = 0; 
int  lastTriggerState = 0; 
bool  Load_time_done=0;// 0 means not finish loading the time sequence, 1 means finish loading the time sequence
int time_group=10;
long time_play[10]={500,500,500,500,500,500,500,500,500,500};//unit us, the length of time_show should be time_group
int profile_play[10]={3,4,3,4,3,4,3,4,3,4};// profile_play should be 0,1,2,...,7
//int time_group=9;
//long time_play[10]={500,500,500,500,500,500,500,500,500};//unit us, the length of time_show should be time_group
//int profile_play[10]={0,1,2,3,4,5,6,7,0};// profile_play should be 0,1,2,...,7

void setup() {

  DDRK&=~(1<<DDK0);//SET ARDUINO MEGA A8='PK0' AS INPUT
  //pinMode(DDS_trigger, INPUT);
  Serial.begin(9600);

  LoadClockSettings();
//  M = INIT_M;
//  K = INIT_K;
//  H = INIT_H;
//  A = INIT_A;
//  SingleProfileFreqOut(M*1000000L + K*1000L + H, A*-1);  
  int Mode_set=1;// 0 is single-tone mode, 1 is profile load mode
  //single tone frequency, amplitude and phase 
  uint32_t singe_freq=150;//unit MHz, the max frequency is 600MHz
  int single_amp=-40;//unit dBm, amplitude should be -84 to 0 dBm
  uint16_t single_phase=0;// phase between 0-360
  //profile 0,1,2.....7 set with frequency and amplitude
  int profile_length=8;//the max profile length is 8, from 0,1,2,....,7
  uint8_t profile_load[profile_length]={0,1,2,3,4,5,6,7};//profile should be 0,1,2,....,7
  uint32_t freq_load[profile_length]={300,300,300,300,300,300,300,300};//unit MHz, the max frequency is 600MHz
  int amp_load[profile_length]={-40,-35,-30,-25,-20,-15,-10,-5};//unit dBm, amplitude should be -84 to 0 dBm
  uint16_t Phase_load[profile_length]={0,0,0,0,0,0,0,0};//phase between 0-360
  //uint32_t freq_load[profile_length]={100,150,200,250,300,350,400,450};//unit MHz, the max frequency is 600MHz
  //int amp_load[profile_length]={-40,-35,-30,-5,-40,-15,-10,-5};//unit dBm, amplitude should be -84 to 0 dBm
  

  if (!Mode_set){//single mode 
    SingleProfileFreqOut(singe_freq*1000000L, single_amp, single_phase);
  }else{// profile play
    //first step is loading the profile
    for(int j=0;j<profile_length;j++){
      uint32_t F1=freq_load[j]*1000000L;//the unit Hz
      int A1=amp_load[j];
      uint8_t Prof1=profile_load[j];
      uint16_t Phase1=Phase_load[j];
      DDS_Fout(&F1,A1,Phase1,Prof1+Single_Tone_Profile_0);
    }
     Load_time_done=1;//确定已经load结束 
     Serial.println("finish the profile load");     
    }
}
void loop() {
    //TriggerState = digitalRead(DDS_trigger);//2.3-3.7us delay
    TriggerState=PINK&B00000001;
    //second step is playing the time sequence
    //if (TriggerState != lastTriggerState){
      if (TriggerState == HIGH && lastTriggerState==LOW && Load_time_done) {
          for (int j=0;j<time_group;j++){
          profile_single_tone_DIRECT(profile_play[j]+14);
//              Serial.println("Is playing profile");
//              Serial.println(profile_play[j]);
//              Serial.println(time_show[j]/1000);
          //delay(long(time_show[j]/1000));
          //delayMicroseconds(time_play[j]);
          if (time_play[j]<16383){//时间短的话，用us来计时,这个是arduino mega设定的delayMicroseconds的最大值
            delayMicroseconds(time_play[j]);
          }else{
            delay(time_play[j]/1000);//时间长的话，用ms来计时
          }              
        }
      }
    //}
    lastTriggerState = TriggerState;
}


           
