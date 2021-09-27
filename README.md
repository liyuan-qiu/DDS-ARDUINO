# DDS-ARDUINO
The pin is based on the hardware https://github.com/afch/DDS-AD9910-Arduino-Shield/tree/master/Libraries .
But I only use the single tone mode and also use the profile0-7 by profile[0:2].
The code they share is most about the display and the buttons to manually control the DDS, so I just rewrite all the code.
## profile time test: the difference between set profile time and real profile time

The CH1(yellow line) is profile0, arduino mega pin 4
The CH2(blue line) is RF output AM by output of minicircuit power detector()
The CH1(yellow line) is profile0, arduino mega pin 4![image](https://user-images.githubusercontent.com/39110126/134913598-fda06707-a702-4f6d-87f2-199caec34964.png)
There are two types of time counting
1. when T_set<6383us the time counting is delayMicroseconds(us)
T_real=1.0202*T_set + 18.516(us)
The first coefficient maybe from the system clock and the second coefficient maybe from the delay.
2. When T_set>16383us, the time counting is delay(ms)
T_real= 1.0071*T_set + 0.127(ms)

## Delay test

### Delay between the Profile_0 pin and FOUT

### Delay between the DDS_trigger and Profile_0 pin
