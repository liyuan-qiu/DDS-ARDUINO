# DDS-ARDUINO
The pin is based on the hardware https://github.com/afch/DDS-AD9910-Arduino-Shield/tree/master/Libraries .
But I only use the single tone mode and also use the profile0-7 by profile[0:2].

## profile time test: the difference between set profile time and real profile time

The CH1(yellow line) is profile0, arduino mega pin 4  
The CH2(blue line) is FOUT AM from the output of minicircuit power detector  

<img width="400" height="300" src="https://user-images.githubusercontent.com/39110126/134914087-31b6bee1-1f9e-481c-8c7d-2ab932efcbe8.png">

There are two types of time counting
1. when T_set<16383us the time counting is delayMicroseconds(us)
T_real=1.0202*T_set + 18.516(us)
The first coefficient maybe from the system clock and the second coefficient maybe from the delay.
2. When T_set>16383us, the time counting is delay(ms)
T_real= 1.0071*T_set + 0.127(ms)

## Delay test

### Delay between the Profile_0 pin and FOUT
The FOUT delay from Profile_0 change is smaller than 0.5us.  
The CH1(yellow line) is profile0, arduino mega pin 4  
The CH2(blue line) is FOUT AM from the output of minicircuit power detector 

![image](https://user-images.githubusercontent.com/39110126/134915241-a7110b08-5d69-41ec-b0a8-64f3f2b3328a.png)

### Delay between the DDS_trigger and Profile_0 pin
CH1(yellow) is DDS_Trigger--arduino Mega A8 pin  
CH3(pink) is Profile_0 
CH2(blue) is FOUT AMP  

The delay between CH1 and CH3 is not fix, the delay time is between 9-14us.  
The delay between CH2 and CH3 is smaller than 0.5us   
<img width="400" height="300" src="https://user-images.githubusercontent.com/39110126/134915860-c5fd5ed2-28d2-4823-9057-b69b65fdd74a.png">
<img width="400" height="300" src="https://user-images.githubusercontent.com/39110126/134915728-1bdd1d55-d368-44e1-adc8-843bd06bfaaf.png">
