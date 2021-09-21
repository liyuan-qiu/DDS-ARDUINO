#include <Arduino.h>
#include "main.h"
String request="";
int M, K, H, A, MenuPos;
#define INIT_M 150
#define INIT_K 0
#define INIT_H 0
#define INIT_A 15 

void setup() {
  Serial.begin(9600);

  LoadClockSettings();
  M = INIT_M;
  K = INIT_K;
  H = INIT_H;
  A = INIT_A;
  SingleProfileFreqOut(M*1000000L + K*1000L + H, A*-1);  

}
void loop() {
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
      M=request.substring(12, 15).toInt();
      Serial.println(request.substring(12, 15));
      K=request.substring(18,21).toInt();
      Serial.println(request.substring(18,21));
      H=request.substring(24,27).toInt();
      Serial.println(request.substring(24,27));
      A=request.substring(30,33).toInt();
      Serial.println(request.substring(30,33));
      SingleProfileFreqOut(M*1000000L + K*1000L + H, A*-1);
      } 
      else{
        if(request.indexOf("Profile Load") >= 0){
          Serial.println("Profile Load");
        }else{
          if(request.indexOf("Profile Play") >= 0){
            Serial.println("Profile Play");
          }
        }
        
      }
    }
  }
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

           
