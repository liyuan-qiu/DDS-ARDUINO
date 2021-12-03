//This is a program where the data is not transfered from the SPI but is set in this program.
//
#include <Arduino.h>
#include "main.h"
String request="";
int M, K, H, A, P;
#define INIT_M 150
#define INIT_K 0
#define INIT_H 0
#define INIT_A 15 
//const int DDS_trigger= A8;//这个脚用来trigger开始
int interruptPin=18;//18 or 19
int  TriggerState = 0; 
int  lastTriggerState = 0; 
bool  Load_time_done=0;// 0 means not finish loading the time sequence, 1 means finish loading the time sequence
//volatile long time_play[];
//volatile int profile_play[];
volatile int time_group;
volatile long time_play[30];//set the max profile play list is 30
volatile int profile_play[30];//set the max profile play list is 30
//int time_group=10;
//long time_play[10]={500,500,500,500,500,500,500,500,500,500};//unit us, the length of time_play should be time_group
//int profile_play[10]={3,4,3,4,3,4,3,4,3,4};// profile_play should be 0,1,2,...,7
//int time_group=9;
//long time_play[10]={500,500,500,500,500,500,500,500,500};//unit us, the length of time_play should be time_group
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
//  //single tone frequency, amplitude and phase 
//  uint32_t singe_freq=150;//unit MHz, the max frequency is 600MHz
//  int single_amp=-40;//unit dBm, amplitude should be -84 to 0 dBm
//  uint16_t single_phase=0;// phase between 0-360
  //profile 0,1,2.....7 set with frequency and amplitude
  int profile_length=8;//the max profile length is 8, from 0,1,2,....,7
  uint8_t profile_load[profile_length]={0,1,2,3,4,5,6,7};//profile should be 0,1,2,....,7
  uint32_t freq_load[profile_length]={300,300,300,300,300,300,300,300};//unit MHz, the max frequency is 600MHz
  int amp_load[profile_length]={-40,-35,-30,-25,-20,-15,-10,-5};//unit dBm, amplitude should be -84 to 0 dBm
  uint16_t Phase_load[profile_length]={0,0,0,0,0,0,0,0};//phase between 0-360
  //uint32_t freq_load[profile_length]={100,150,200,250,300,350,400,450};//unit MHz, the max frequency is 600MHz
  //int amp_load[profile_length]={-40,-35,-30,-5,-40,-15,-10,-5};//unit dBm, amplitude should be -84 to 0 dBm
  attachInterrupt(digitalPinToInterrupt(interruptPin), profile_interrupt, RISING);
  

//  if (!Mode_set){//single mode 
//    SingleProfileFreqOut(singe_freq*1000000L, single_amp, single_phase);
//  }else{// profile play
//    //first step is loading the profile
//    for(int j=0;j<profile_length;j++){
//      uint32_t F1=freq_load[j]*1000000L;//the unit Hz
//      int A1=amp_load[j];
//      uint8_t Prof1=profile_load[j];
//      uint16_t Phase1=Phase_load[j];
//      DDS_Fout(&F1,A1,Phase1,Prof1+Single_Tone_Profile_0);
//    }
//     Load_time_done=1;//确定已经load结束 
//     Serial.println("finish the profile load");     
//    }
}
void profile_interrupt() {
    //TriggerState = digitalRead(DDS_trigger);//2.3-3.7us delay
    TriggerState=PINK&B00000001;
    //second step is playing the time sequence
    //if (TriggerState != lastTriggerState){
      //if (TriggerState == HIGH && lastTriggerState==LOW && Load_time_done) {
          for (int j=0;j<time_group;j++){
          profile_single_tone_DIRECT(profile_play[j]+14);
//              Serial.println("Is playing profile");
//              Serial.println(profile_play[j]);
//              Serial.println(time_play[j]/1000);
          //delay(long(time_play[j]/1000));
          //delayMicroseconds(time_play[j]);
          if (time_play[j]<16383){//时间短的话，用us来计时,这个是arduino mega设定的delayMicroseconds的最大值
            delayMicroseconds(time_play[j]);
          }else{
            delay(time_play[j]/1000);//时间长的话，用ms来计时
          }              
        }
      //}
    //}
    lastTriggerState = TriggerState;
}

void loop() {
 // TriggerState = digitalRead(DDS_trigger);
  if (Serial.available() > 0)//判读是否串口有数据
  {
    String request = "";//缓存清零
    while (Serial.available() > 0)//循环串口是否有数据
    {
      request += char(Serial.read());//叠加数据到request
      delay(2);//延时等待响应
    }
    if (request.length() > 0)//如果request有数据
    {
      Serial.println(request);//打印request数据
      if (request.indexOf("Single Tone ") >= 0){     
      M=request.substring(12, request.indexOf("MHz")).toInt();
      //Serial.println(request.substring(12, 15));
      Serial.println(M);
      K=request.substring(request.indexOf("MHz")+3,request.indexOf("kHz")).toInt();
      //Serial.println(request.substring(18,21));
      Serial.println(K);
      H=request.substring(request.indexOf("kHz")+3,request.indexOf("HHz")).toInt();
      Serial.println(H);
      A=request.substring(request.indexOf("HHz")+3,request.indexOf("dBm")).toInt();
      Serial.println(A);
      P=request.substring(request.indexOf("dBm")+3,request.indexOf("Phase")).toInt();
      Serial.println(P);
        //single tone frequency, amplitude and phase 
      uint32_t singe_freq=M*1000000L + K*1000L + H;//unit Hz, the max frequency is 600MHz
      int single_amp=A*-1;//unit dBm, amplitude should be -84 to 0 dBm
      uint16_t single_phase=P;// phase between 0-360
      SingleProfileFreqOut(singe_freq, single_amp, single_phase);
      } 
      else{
        if(request.indexOf("Profile Load ") >= 0){
          Serial.println("Profile Load");
          //from ad9910.cpp void DDS_Fout (uint32_t *F_OUT, int16_t Amplitude_dB, uint8_t Num_Prof)
          //void SingleProfileFreqOut(uint32_t freq_output, int16_t amplitude_dB_output) 
          uint32_t F1=request.substring(13, request.indexOf("Hz")).toInt();
          Serial.println(F1);
          int16_t A1=request.substring(request.indexOf("Hz")+2, request.indexOf("dBm")).toInt();
          Serial.println(A1);
          uint16_t Phase1=request.substring(request.indexOf("dBm")+3, request.indexOf("Deg")).toInt();
          Serial.println(Phase1);
          uint8_t  Prof1=request.substring(request.indexOf("Deg")+3, request.indexOf("prof_set")).toInt();
          Serial.println(Prof1);
          DDS_Fout(&F1,A1,Phase1,Prof1+Single_Tone_Profile_0);
        }else{
          if(request.indexOf("Profile Play") >= 0){
            Serial.println("Profile Play");
            delay(1000);
            time_group=request.substring(13, 15).toInt();
            Serial.println("time group");
            Serial.println(time_group);
            for (int j=0;j<time_group;j++){
              time_play[j]=request.substring(20+11*j, 20+10+11*j).toInt();
              profile_play[j]=request.substring(20+10+11*j, 20+11+11*j).toInt();
              Serial.println(time_play[j]);
              Serial.println(profile_play[j]);
            }
            Serial.println('end loading');
            //if (TriggerState != lastTriggerState){//这个是要求Serial.available(),应该是不对的，需要在loop里进行
              //if (TriggerState == HIGH) {
//            for (int j=0;j<time_group;j++){
//              profile_single_tone(profile_play[j]+Single_Tone_Profile_0);
//              Serial.println("Is playing profile");
//              Serial.println(profile_play[j]);
//              Serial.println(time_play[j]/1000);
//              //delay(long(time_play[j]/1000));
//              if (time_play[j]<16383){//时间短的话，用us来计时
//                delayMicroseconds(time_play[j]);
//              }else{
//                delay(time_play[j]/1000);//时间长的话，用ms来计时
//              }
//              
//            }
              //}
            //}
            
          }
        }
        
      }
    }
  }
  lastTriggerState = TriggerState;
}
           
