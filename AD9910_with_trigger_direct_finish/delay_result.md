用第二个单片机产生trigger信号
void loop() {
  // put your main code here, to run repeatedly:
  while(1){
  digitalWrite(test_trigger,1);
  delay(100);  
  digitalWrite(test_trigger,0);
  delay(200); 
  }
}
第一个单片机用来控制DDS
int time_group=10;
long time_play[10]={500,500,500,500,500,500,500,500,500,500};//unit us, the length of time_show should be time_group
int profile_play[10]={3,4,3,4,3,4,3,4,3,4};// profile_play should be 0,1,2,...,7

1.代码如下
if (TriggerState != lastTriggerState){
      if (TriggerState == HIGH && Load_time_done) {
}
}
//          if (time_play[j]<16383){//时间短的话，用us来计时,这个是arduino mega设定的delayMicroseconds的最大值
//            delayMicroseconds(time_play[j]);
//          }else{
//            delay(time_play[j]/1000);//时间长的话，用ms来计时
//          }

delay时间在50-100us之间
2.代码如下
if (TriggerState == HIGH && lastTriggerState==LOW && Load_time_done) {}
          if (time_play[j]<16383){//时间短的话，用us来计时,这个是arduino mega设定的delayMicroseconds的最大值
           delayMicroseconds(time_play[j]);
       }else{
           delay(time_play[j]/1000);//时间长的话，用ms来计时
  }
delay时间在9-14us之间
这个效果最好，而且可以兼容更长时间的设置


Channel1 是DDS_Trigger也就是arduino Mega A8脚
Channel3是Profile0信号
Channel2是FOUT信号的AMP

Channel1和channel3之间的delay在9-14us之间
Channel2和channel3之间的delay在小于0.5us，可以忽略不计
所以最好用switch控制在15us以后开启RF信号，可以保证有更好的输出




3.代码如下
if (TriggerState == HIGH && lastTriggerState==LOW && Load_time_done) {}
delayMicroseconds(time_play[j]);
delay时间在9-14us
![image](https://user-images.githubusercontent.com/39110126/134906979-e9e82cc5-6cda-49b0-a6ea-069b476835bf.png)
