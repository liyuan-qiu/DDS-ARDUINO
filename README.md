# DDS-ARDUINO
The pin is based on the hardware https://github.com/afch/DDS-AD9910-Arduino-Shield/tree/master/Libraries .
But I only use the single tone mode and also use the profile0-7 by profile[0:2].
The code they share is most about the display and the buttons to manually control the DDS, so I just rewrite all the code.
## profile time test: the difference between set profile time and real profile time
The CH1(yellow line) is profile0, arduino mega pin 4
The CH2(blue line) is RF output AM by output of minicircuit power detector()![image](https://user-images.githubusercontent.com/39110126/134913237-80617bbc-fa4b-42f3-acbb-ba40a499e314.png)
There are two types of 
小于16383us，用的是delayMicroseconds(us)
T_real=1.0202*T_set + 18.516(us)
第一个系数可能是因为时钟，第二个系数可能是延迟
====================================================
大于16383us以后，用的是delay(ms)
T_real= 1.0071*T_set + 0.127(ms)
![image](https://user-images.githubusercontent.com/39110126/134913158-1429cb84-77b1-4e8a-8db5-1a9344d2622a.png)

## Delay test

### Delay between the Profile_0 pin and FOUT

### Delay between the DDS_trigger and Profile_0 pin
