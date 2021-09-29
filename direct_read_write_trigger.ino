//direct digitalwrite after trigger test
//DDS_trigger A8='PK0'

//const int DDS_trigger= A8;//这个脚用来trigger开始,A8='PK0'

int  TriggerState = 0; 
int  lastTriggerState = 0; 
bool  Load_time_done=1;// 0 means not finish loading the time sequence, 1 means finish loading the time sequence
int time_group=10;
long time_play[10]={500,500,500,500,500,500,500,500,500,500};//unit us, the length of time_show should be time_group
int profile_play[10]={3,4,3,4,3,4,3,4,3,4};// profile_play should be 0,1,2,...,7
int Single_Tone_Profile_0=14;

void setup() {
  //pinMode(DDS_trigger, INPUT);
  DDRE|= 3<<DDE4;//SET ARDUINO MEGA 3=profile1=PE5,2=profile2=PE4 PIN AS OUTPUT , 
  DDRG|=1<<DDG5;//SET ARDUINO MEGA 4=profile0=PG5 PIN AS OUTPUT 
  DDRK&=~(1<<DDK0);//SET ARDUINO MEGA A8='PK0' AS INPUT
}

void loop() {
    TriggerState=PINK&B00000001;
    //2.3-3.7us delay,偶尔还会到10us，感觉好像很不robust
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
        //用不用判断实际上是一样的时间           
        }
      }
    //}
    lastTriggerState = TriggerState;
}
//这个是direct digital manipulation的方法来切换profile
void profile_single_tone_DIRECT(uint8_t Num_Prof){
  //PORTB |= 1 << PORTB7;set the Arduino pin 13 high
  //PORTB &= ~(1 << PORTB7);set the Arduino pin 13 low
  //在DDS_trigger读取用的是digitalRead的情况下profile0,1,2和DDS_trigger rising之间是有3-7us的变化
    int Prof=Num_Prof;
    Prof=Prof-14; // address of 0 profile: 0x0E
    if (bitRead(Prof, 0)==1) PORTG |= 1 << PORTG5;
    else PORTG &= ~(1 << PORTG5);
    if (bitRead(Prof, 1)==1) PORTE |= 1 << PORTE5;
    else PORTE &= ~(1 << PORTE5);
    if (bitRead(Prof, 2)==1) PORTE |= 1 << PORTE4;
    else PORTE &= ~(1 << PORTE4);

}
//DDS_trigger=A8 pin,DDS_PROFILE_0_PIN=4,DIGITAL READ AND DIGITAL WRITE,delay7.4-12us

//DDS_trigger=19 pin,DDS_PROFILE_0_PIN=4,DIGITAL READ AND DIGITAL WRITE,delay7.4-12us

//DDS_trigger=A8 pin,DDS_PROFILE_0_PIN=4,完全是直接操作，delay 1.2-3us
//DDS_trigger=19 pin,DDS_PROFILE_0_PIN=4,完全是直接操作，delay 1.2-3us
