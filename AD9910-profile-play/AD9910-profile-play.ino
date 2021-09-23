#include <Arduino.h>

const int SPI_SCLK =13;
const int SPI_SDIO = 11;

const int OSK = 40;
const int PR0[4] = {4,4,4,4};
const int PR1[4] = {3,3,3,3};
const int PR2[4] = {2,2,2,2};

const int SPI_CSB[4] = {10,10,10,10};
//Chip Select. Digital input (active low). This pin allows 
//the AD9910 to operate on a common serial bus for the control 
//data path. Bringing this pin low enables the AD9910 to detect 
//serial clock rising/falling edges. Bringing this pin high 
//causes the AD9910 to ignore input on the serial data pins.
const int MASTER_RESET[4] = {A3,A3,A3,A3};
//Master Reset, Digital Input (Active High). 
//Master reset: clears all memory elements and sets registers to default values.

const int IO_UPDATE[4] = {6,6,6,6};
//Input/Output Update. Digital input (active high). 
//A high on this pin transfers the contents of the 
//I/O buffers to the corresponding internal registers.
const int DDS_Trigger=;//需要分配一个trigger output给DDS用来开始进行play profile
/*const int SPI_CSB = 9;
const int MASTER_RESET = 23;
const int PR0 = 29;
const int PR1 = 30;
const int IO_UPDATE = 2;*/

/*const int SPI_CSB = 8;
const int MASTER_RESET = 7;
const int PR0 = 6;
const int PR1 = 20;
const int IO_UPDATE = 21;*/

/*const int SPI_CSB = 26;
const int MASTER_RESET = 31;
const int PR0 = 4;
const int PR1 = 3;
const int IO_UPDATE = 24;*/

void write1_writen(int slave, int instruction, int n, byte* data) {//instruction:address ,data: what you want to write
      digitalWrite(SPI_SCLK, LOW); // ensure clock pin LOW when we start
      digitalWrite(SPI_CSB[slave], LOW);
      
      shiftOut(SPI_SDIO, SPI_SCLK, MSBFIRST, instruction);
      for (int i = 0; i < n; ++i) shiftOut(SPI_SDIO, SPI_SCLK, MSBFIRST, data[i]);
      
      digitalWrite(SPI_CSB[slave], HIGH);
};

void write1_readn(int slave, int instruction, int n, byte *data) {
      digitalWrite(SPI_SCLK, LOW); // ensure clock pin LOW when we start
      digitalWrite(SPI_CSB[slave], LOW);
      //shiftOut(SPI_SDIO, SPI_SCLK, MSBFIRST, (instruction >> 8)); // high byte
      shiftOut(SPI_SDIO, SPI_SCLK, MSBFIRST, instruction); // low byte
      
      pinMode(SPI_SDIO, INPUT);
      for (int i = 0; i < n; ++i) data[i] = shiftIn(SPI_SDIO, SPI_SCLK, MSBFIRST);
      
      digitalWrite(SPI_CSB[slave], HIGH);
      pinMode(SPI_SDIO, OUTPUT);  
};
static void update(int slave) {
    digitalWrite(IO_UPDATE[slave], HIGH);
    delay(1);
    digitalWrite(IO_UPDATE[slave], LOW);
}

void setup() {
    Serial.begin(9600);

    pinMode(OSK,OUTPUT);
    //pinMode(PR2,OUTPUT);
    //digitalWrite(PR2,LOW);
    pinMode(SPI_SCLK, OUTPUT);
    pinMode(SPI_SDIO, OUTPUT);
    pinMode(DDS_Trigger,OUTPUT);
    digitalWrite(OSK,HIGH);
    digitalWrite(SPI_SCLK, LOW);
      
    for (int i = 0; i < 4; ++i) {
      pinMode(MASTER_RESET[i],OUTPUT);
      pinMode(SPI_CSB[i],OUTPUT);
      pinMode(IO_UPDATE[i],OUTPUT);
      pinMode(PR0[i],OUTPUT);//PR0,PR1,PR2用来选择profile 0-7
      pinMode(PR1[i],OUTPUT);
      pinMode(PR2[i],OUTPUT);
      digitalWrite(PR0[i],LOW);
      digitalWrite(PR1[i],LOW);
      digitalWrite(PR1[i],HIGH);
//      pinMode(PR0[i],OUTPUT);//PR0,PR1,PR2用来选择profile 0-7
//      pinMode(PR1[i],OUTPUT);
//      pinMode(PR2[i],OUTPUT);

      digitalWrite(MASTER_RESET[i],HIGH);
      delay(1);
      digitalWrite(MASTER_RESET[i],LOW);// this master reset from high to low, reset to default setting of the device
      delay(1);
  
      digitalWrite(SPI_CSB[i],HIGH); // deactivate
      digitalWrite(IO_UPDATE[i],LOW);
    } 
}

String request="";

void loop() {  
  while(Serial.connected()){
  while (Serial.available()) {
//    byte b = Serial.read();
    char c = Serial.read();
    Serial.write(c);//为什么这边需要一个回给电脑的命令
    if (c == '\n') {
      break;
    }
    requese+=c;
  }
    if(request.length()!=0) {
         if (request.indexOf("Single Tone ") >= 0){
           myString= request.substring(12, request.indexOf('\n'));
           String_length=myString.length();
           myData=getBytes(myString,String_length);
           write_profile(myData);
         }
         else{
          if(request.indexOf("Load Profile ") >= 0){
            myString = request.substring(13, request.indexOf('\n'));
            String_length=myString.length();
            myData=getBytes(myString,String_length);
            write_profile(myData);
          }
          else{
            if(request.indexOf("Play Profile ") >= 0){
            TimeString = request.substring(13, request.indexOf('\n'));
            String_length=myString.length();              
          }
          }
         }
    }

}
}
void write_profile(byte b){
  byte state = 0, slave = 0, len = 0, pos = 0, data[9];
      if (state == 0) {
      slave = (b & 0x60) >> 5;//对应matlab里面32*i，判断是第几个slave，这位运算张翔写的绝了，太厉害了
      len = b & 0x1F;//对应matlab里面32*i+4/8/128/136,4的话对应的事寄存器write，8的话对应的是数据write，128的话对应的是update，136的话对应的是read，因为有三种状态，write/update/read，所以需要两个判断，包括（b & 0x80）来判断是update/read还是write，然后第二个（b & 0x1F）来区分是update或者read
      if (b & 0x80)  {//判断2进制第一位是否为1，即>=128 or1000 0000or 0x80    read or update      
        //如果update b=0x80,len=0,如果 read b>80,通常是84 或 88，所以 len=4或者8
        if (len == 0)//判断b如果等于128
          update(slave);
        else //如果b大于128， read
          state = 1;
      } else  { // 如果b<128 write,此处是数据位数
        state = 2;
        ++len;
        pos = 0;
      }
    } else if (state == 1) {
      write1_readn(slave, b | 0x80, len, data);//此时b为传过来的第二位，即地址，b|80表示地址位字节的第一位设为1，后面的bit照抄地址位,详见说明书P48，表示读操作和地址位
      for (int i = 0; i < len; ++i)//读取信息
        Serial.write(data[i]);
      state = 0;
    } else if (state == 2) {
       data[pos] = b;//这样写可以保证data[9]都可以读到b的数据吗，有点不懂？？？
       if (++pos == len) {
          write1_writen(slave,data[0],len-1,data+1);//???为什么最后的data要+1
          state = 0;
       }
    }
  }

void profile_play(byte TimeString){
  digitalWrite(DDS_Trigger,OUTPUT)
}
