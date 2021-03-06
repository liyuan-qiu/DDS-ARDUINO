#include <Arduino.h>
#include "main.h"
String request="";
int M, K, H, A, MenuPos;
#define INIT_M 150
#define INIT_K 0
#define INIT_H 0
#define INIT_A 15 
const int DDS_trigger= A8;//这个脚用来trigger开始
int TriggerState = 0; 
int lastTriggerState = 0; 
void setup() {
  pinMode(DDS_trigger, INPUT);
//  attachInterrupt(digitalPinToInterrupt(DDS_trigger), playprofile, RISING );
  Serial.begin(9600);

  LoadClockSettings();
  M = INIT_M;
  K = INIT_K;
  H = INIT_H;
  A = INIT_A;
  SingleProfileFreqOut(M*1000000L + K*1000L + H, A*-1);  

}
void loop() {
  TriggerState = digitalRead(DDS_trigger);
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
      SingleProfileFreqOut(M*1000000L + K*1000L + H, A*-1);
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
          uint8_t  Prof1=request.substring(request.indexOf("dBm")+3, request.indexOf("prof_set")).toInt();
          Serial.println(Prof1);
          DDS_Fout(&F1,A1,Prof1+Single_Tone_Profile_0);
          //DDS_UPDATE();
          //SingleProfileFreqOut(F1,A1);
        }else{
          if(request.indexOf("Profile Play") >= 0){
            Serial.println("Profile Play");
            delay(1000);
            int time_group=request.substring(13, 15).toInt();
            Serial.println("time group");
            Serial.println(time_group);
            long time_show[time_group];
            int profile_play[time_group];
            for (int j=0;j<time_group;j++){
              time_show[j]=request.substring(20+11*j, 20+10+11*j).toInt();
              profile_play[j]=request.substring(20+10+11*j, 20+11+11*j).toInt();
              Serial.println(time_show[j]);
              Serial.println(profile_play[j]);
            }
            Serial.println('end loading');
            //if (TriggerState != lastTriggerState){//这个是要求Serial.available(),应该是不对的，需要在loop里进行
              //if (TriggerState == HIGH) {
            for (int j=0;j<time_group;j++){
              profile_single_tone(profile_play[j]+Single_Tone_Profile_0);
              Serial.println("Is playing profile");
              Serial.println(profile_play[j]);
              Serial.println(time_show[j]/1000);
              //delay(long(time_show[j]/1000));
              if (time_show[j]<16383){//时间短的话，用us来计时
                delayMicroseconds(time_show[j]);
              }else{
                delay(time_show[j]/1000);//时间长的话，用ms来计时
              }
              
            }
              //}
            //}
            
          }
        }
        
      }
    }
  }
  lastTriggerState = TriggerState;
//  if (TriggerState != lastTriggerState) {
//  if (TriggerState == HIGH) {
//      for (int j=0;j<time_group;j++){
//      profile_single_tone(profile_play[j]+Single_Tone_Profile_0);
////              Serial.println("Is playing profile");
////              Serial.println(profile_play[j]);
////              Serial.println(time_show[j]/1000);
//      //delay(long(time_show[j]/1000));
//      if (time_show[j]<16383){//时间短的话，用us来计时
//        delayMicroseconds(time_show[j]);
//      }else{
//        delay(time_show[j]/1000);//时间长的话，用ms来计时
//      }
//      
//    }
//  }
//}
}

//void loop() {  
//  if (Serial.available()>0){
//    request="";//缓存清零
//  while (Serial.available()) {//循环串口是否有数据
////    byte b = Serial.read();
//    char c = Serial.read();
//    request+=Serial.read();
//    //Serial.write(c);//为什么这边需要一个回给电脑的命令
//  }
//
//    if(request.length()>0) {
//      Serial.println(request);
//      if (request.indexOf("Single Tone ") >= 0){     
//      M=request.substring(12, request.indexOf('MHz')).toInt();
//      K=request.substring(request.indexOf('MHz')+3, request.indexOf('kHz')).toInt();
//      H=request.substring(request.indexOf('kHz')+3, request.indexOf('HHz')).toInt();
//      A=request.substring(request.indexOf('HHz')+3, request.indexOf('dBm')).toInt();
//      SingleProfileFreqOut(M*1000000L + K*1000L + H, A*-1); 
//      }
//
//    }
//  }
//}
// --------------------------------------------------------------------------------------//
//           A=request.substring(12, request.indexOf('dBm')).toInt();
//           //Serial.print(A);
//           M=request.substring(request.indexOf('dBm')+3, request.indexOf('MHz')).toInt();
//           Serial.print(M);  
//           K=request.substring(request.indexOf('MHz')+3, request.indexOf('kHz')).toInt();
//           Serial.print(K);
//           H=request.substring(request.indexOf('kHz')+3, request.indexOf('HHz')).toInt();
//           SingleProfileFreqOut(M*1000000L + K*1000L + H, A*-1); 
// --------------------------------------------------------------------------------------//
//这个没什么问题，说明toInt这个函数没什么问题
//           String A_string="15";
//           A=A_string.toInt(); 
//           String M_string="200";
//           M=M_string.toInt();
//           String K_string="10";
//           K=K_string.toInt();
//           String H_string="10";
//           H=H_string.toInt();                         
//           SingleProfileFreqOut(M*1000000L + K*1000L + H, A*-1); 

           
