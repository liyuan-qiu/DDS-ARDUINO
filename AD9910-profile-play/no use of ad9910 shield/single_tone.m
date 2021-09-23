% %% 通讯协议
% %输入字符串，字节位
% % byte1 update 128 len = b & 0x1F==0
% %       read   >128,=128+数据长度，b & 0x80第一位为1，同时len = b & 0x1F!=0
% %       write  数据长度，通常是4或8, b & 0x80第一位为0
% %同时byte1的最后五位2进制是用来确定选择哪一个slave片的，用32*i中的i=0,1,2,3来判断
% % byte2 寄存器地址 0~32，常用地址14 （0x0E），是波形参数寄存器
% % byte3~ 数据 
% %      data byte 1 2| 3 4| 5 6 7 8
% %      data byte 1~2 幅值参数14位，最大2^14对应幅值幅值5V，峰峰值10V，输入数据对应（byte1*128+byte2）/2^14 *5V
% %      data byte 3~4 相位参数16位
% %      data byte 5~8 频率参数32位，最大模拟量1GHz，实际模拟输出1GHz*（byte5*256^3+byte6*256^2+byte7*256+byte8)/2^32
% 
% 
% % Instrument Connection
% 
% % Find a serial port object.
% com = instrfind('Type', 'serial', 'Port', 'COM10', 'Tag', '');
% 
% % Create the serial port object if it does not exist
% % otherwise use the object that was found.
% if isempty(com)
%     com = serial('COM10');
% else
%     fclose(com);
%     com = com(1);
% end
% 
% % Connect to instrument object, obj1.
% fopen(com); %如果此时串口被其他程序占用，会报错，需要重启单片机
% pause(3);
% % % %========================================================
reference_CLK=50*10^6;
SYS_CLK=10^9;
PLL_multiple=round(SYS_CLK/reference_CLK);
frequency_set=10*10^6;%设定的frequency大小
AMP_Max=1;%这里需要再弄成dBm为单位的设定
phase_set=0;
AMP_Set=0.5;
i=0
%AD9910 initialization
%第一位 update 128；write <=9;read >128,数据位数等于第一位-128
% fwrite(com,[32*i+4,0,0,128,0,0],'uint8');%第一位空：写；第二位4：写4位；第三位0：寄存器地址0；剩下的是写入的数据
request1=strcat("Single Tone ", num2str(uint8([32*i+4,0,0,128,0,0])));
fprintf(com,'%s \n',request1);
% fwrite(com,[32*i+4,1,1,64,8,32],'uint8');%寄存器地址1
request2=strcat("Single Tone ", num2str(uint8([32*i+4,1,1,64,8,32])));
fprintf(com,'%s \n',request2);
% fwrite(com,[32*i+4,2,29,63,65,PLL_multiple],'uint8');%寄存器地址2(CFR2)，这个是确定PLL的倍频倍数的,即PLL_multiple
request3=strcat("Single Tone ", num2str(uint8([32*i+4,2,29,63,65,PLL_multiple])));
fprintf(com,'%s \n',request3);
% fwrite(com,[32*i+4,3,0,0,0,127],'uint8');%寄存器地址3
request4=strcat("Single Tone ", num2str(uint8([32*i+4,3,0,0,0,127])));
fprintf(com,'%s \n',request4);
%写入波形，8位数据，地址E
% if i==0
% fwrite(com,[32*i+8,14,23,0,0,0,64,0,0,0],'uint8');%数据长度8，寄存器地址E，幅值,相位，控制频率
% else
% fwrite(com,[32*i+8,14,63,255,0,0,66,143,92,40],'uint8');
% end

% 15.625MHz, ~980mV
[freq_B1,freq_B2,freq_B3,freq_B4]=encodeFreq(frequency_set,SYS_CLK);
[amp_B1,amp_B2]=encodeAmp(AMP_Set,AMP_Max);
[phase_B1, phase_B2]=encodePhase(phase_set, max_phase);
% fwrite(com,[32*i+8,14,amp_B1,amp_B2,phase_B1,phase_B2,freq_B1,freq_B2,freq_B3,freq_B4],'uint8');%数据长度8，寄存器地址E，幅值,相位，控制频率fwrite(com,[32*i+8,14,63,255,0,0,66,143,92,40],'uint8');
request5=strcat("Single Tone ", num2str(uint8([32*i+8,14,amp_B1,amp_B2,phase_B1,phase_B2,freq_B1,freq_B2,freq_B3,freq_B4])));
fprintf(com,'%s \n',request5);


% fwrite(com,[32*i+8,15,63,255,0,0,2,0,0,0],'uint8');
% fwrite(com,[32*i+8,16,63,255,0,0,3,0,0,0],'uint8');
% fwrite(com,[32*i+8,17,63,255,0,0,4,0,0,0],'uint8');
% fwrite(com,[32*i+8,18,63,255,0,0,5,0,0,0],'uint8');
% fwrite(com,[32*i+8,19,63,255,0,0,6,0,0,0],'uint8');
% fwrite(com,[32*i+8,20,63,255,0,0,7,0,0,0],'uint8');
% fwrite(com,[32*i+8,21,63,255,0,0,64,0,0,0],'uint8');
% fwrite(com,[32*i+128],'uint8');%update
request6=strcat("Single Tone ", num2str(uint8([32*i+128])));
fprintf(com,'%s \n',request6);
% read，数据长度=136-128=8位，14：地址E，读出频率相位幅值数据
%fwrite(com,[32*i+136,14],'uint8');
%
%    63-幅值 14位
%    255-

%      0-相位，16位
%      0-

%      1 -
%      1 频率，15.计算方法是，AD9910频率是32位寄存器，2^32
%      对应1GHz,输入数据每一位代表一个字节，即2进制的8位，最大值是2^8，所以数值是1*2^0+1*2^8+1*2^16+1*2^24=freq,实际输出频率=1GHZ*freq/2^32,此处是3.9MHz
%      1
%      1 -

% update
%out = fread(com,8,'uint8')
function [freq_B1,freq_B2,freq_B3,freq_B4]=encodeFreq(frequency_set,SYS_CLK)

%         # encode a frequency value in MHz from float to 4 bytes uint8 (32bits)
%         # DDS frequency range 0 - 1GHz (default)
%         # actual frequency value is:
%         #   1GHz *（ byte1 * 256^3 + byte2 * 256^2 + byte3 * 256 + byte4 ) / 2^32  
%         # max frequency bytes are 255 255 255 255
    freq_i=min(round(frequency_set/SYS_CLK*2^32),2^32-1);
    freq_B1=floor(freq_i / 2^24); 
    freq_i = freq_i - freq_B1 * 2^24;
    freq_B2 = floor(freq_i / 2^16);
    freq_i = freq_i - freq_B2 * 2^16;
    freq_B3 = floor(freq_i / 2^8);
    freq_i = freq_i - freq_B3 * 2^8;
    freq_B4 = floor(freq_i);

end
function [freq_B1,freq_B2,freq_B3,freq_B4]=encodeFreq2(frequency_set,SYS_CLK)
    reference_CLK=50*10^6;
    SYS_CLK=10^9;
    PLL_multiple=round(SYS_CLK/reference_CLK);
    frequency_set=10*10^6;%设定的frequency大小
    freq_i=uint32(min(round(frequency_set/SYS_CLK*2^32),2^32-1));
    freq_B1=bitshift(freq_i,-24);
    freq_i=uint32(bitshift(freq_i,8));
    freq_B2=bitshift(freq_i,-24);
    freq_i=uint32(bitshift(freq_i,8));
    freq_B3=bitshift(freq_i,-24); 
    freq_i=uint32(bitshift(freq_i,8));
    freq_B4=bitshift(freq_i,-24); 
end
function [amp_B1,amp_B2]=encodeAmp(AMP_Set,AMP_Max)
%         # encode a amplitude value from float to 2 bytes uint8 (14bits)
%         # DDS amplitude range 0 - 1 (default), the max amplitude of each DDS at different frequencies are differnet, so we use a unit value max_amp to represent them
%         # actual amplitude value is:
%         #   max_amp *（ byte1 * 256 + byte2 ) / 2^14
%         # max amplitude bytes are 63 255
        amp = min(AMP_Set, AMP_Max);
        amp_i = min(floor(amp / max_amp * 2^14) , 2^14-1);
        amp_B1 = floor(amp_i / 2^8);
        amp_i = amp_i - amp_B1 * 2^8;
        amp_B2 = floor(amp_i);
end
function [phase_B1, phase_B2]=encodePhase(phase_set, max_phase)
%     def __encodePhase(self,phase, max_phase = 360):
%         # encode a phase value in degree from float to 2 bytes uint8 (16bits)
%         # DDS phase range 0-360 degree
%         # actual Amplitude value is:
%         #   360 *（ byte1 * 256 + byte4 ) / 2^16
%         # max phase bytes are 255 255
    max_phase=360
    phase_i = min(floor(phase / max_phase * 2^16) , 2^16-1);
    phase_B1 = floor(phase_i / 2^8);
    phase_i = phase_i - phase_B1 * 2^8;
    phase_B2 = floor(phase_i);

end
      
% fclose(com);
