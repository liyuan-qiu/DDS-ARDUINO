%% ͨѶЭ��
%�����ַ������ֽ�λ
% byte1 update 128 len = b & 0x1F==0
%       read   >128,=128+���ݳ��ȣ�b & 0x80��һλΪ1��ͬʱlen = b & 0x1F!=0
%       write  ���ݳ��ȣ�ͨ����4��8, b & 0x80��һλΪ0
%ͬʱbyte1�������λ2����������ȷ��ѡ����һ��slaveƬ�ģ���32*i�е�i=0,1,2,3���ж�
% byte2 �Ĵ�����ַ 0~32�����õ�ַ14 ��0x0E�����ǲ��β����Ĵ���
% byte3~ ���� 
%      data byte 1 2| 3 4| 5 6 7 8
%      data byte 1~2 ��ֵ����14λ�����2^14��Ӧ��ֵ��ֵ5V�����ֵ10V���������ݶ�Ӧ��byte1*128+byte2��/2^14 *5V
%      data byte 3~4 ��λ����16λ
%      data byte 5~8 Ƶ�ʲ���32λ�����ģ����1GHz��ʵ��ģ�����1GHz*��byte5*256^3+byte6*256^2+byte7*256+byte8)/2^32


% Instrument Connection

% Find a serial port object.
com = instrfind('Type', 'serial', 'Port', 'COM10', 'Tag', '');

% Create the serial port object if it does not exist
% otherwise use the object that was found.
if isempty(com)
    com = serial('COM10');
else
    fclose(com);
    com = com(1);
end

% Connect to instrument object, obj1.
fopen(com); %�����ʱ���ڱ���������ռ�ã��ᱨ����Ҫ������Ƭ��
pause(3);
% % % %========================================================
reference_CLK=50*10^6;
SYS_CLK=10^9;
PLL_multiple=round(SYS_CLK/reference_CLK);
frequency_set=10*10^6;%�趨��frequency��С
AMP_Max=1;%������Ҫ��Ū��dBmΪ��λ���趨
phase_set=0;
AMP_Set=0.5;
i=0
%AD9910 initialization
%��һλ update 128��write <=9;read >128,����λ�����ڵ�һλ-128
fwrite(com,[32*i+4,0,0,128,0,0],'uint8');%��һλ�գ�д���ڶ�λ4��д4λ������λ0���Ĵ�����ַ0��ʣ�µ���д�������
fwrite(com,[32*i+4,1,1,64,8,32],'uint8');%�Ĵ�����ַ1
fwrite(com,[32*i+4,2,29,63,65,PLL_multiple],'uint8');%�Ĵ�����ַ2(CFR2)�������ȷ��PLL�ı�Ƶ������,��PLL_multiple
fwrite(com,[32*i+4,3,0,0,0,127],'uint8');%�Ĵ�����ַ3

%д�벨�Σ�8λ���ݣ���ַE
% if i==0
% fwrite(com,[32*i+8,14,23,0,0,0,64,0,0,0],'uint8');%���ݳ���8���Ĵ�����ַE����ֵ,��λ������Ƶ��
% else
% fwrite(com,[32*i+8,14,63,255,0,0,66,143,92,40],'uint8');
% end

% 15.625MHz, ~980mV
[freq_B1,freq_B2,freq_B3,freq_B4]=encodeFreq(frequency_set,SYS_CLK);
[amp_B1,amp_B2]=encodeAmp(AMP_Set,AMP_Max);
[phase_B1, phase_B2]=encodePhase(phase_set, max_phase);
fwrite(com,[32*i+8,14,amp_B1,amp_B2,phase_B1,phase_B2,freq_B1,freq_B2,freq_B3,freq_B4],'uint8');%���ݳ���8���Ĵ�����ַE����ֵ,��λ������Ƶ��fwrite(com,[32*i+8,14,63,255,0,0,66,143,92,40],'uint8');


% fwrite(com,[32*i+8,15,63,255,0,0,2,0,0,0],'uint8');
% fwrite(com,[32*i+8,16,63,255,0,0,3,0,0,0],'uint8');
% fwrite(com,[32*i+8,17,63,255,0,0,4,0,0,0],'uint8');
% fwrite(com,[32*i+8,18,63,255,0,0,5,0,0,0],'uint8');
% fwrite(com,[32*i+8,19,63,255,0,0,6,0,0,0],'uint8');
% fwrite(com,[32*i+8,20,63,255,0,0,7,0,0,0],'uint8');
% fwrite(com,[32*i+8,21,63,255,0,0,64,0,0,0],'uint8');
fwrite(com,[32*i+128],'uint8');%update

% read�����ݳ���=136-128=8λ��14����ַE������Ƶ����λ��ֵ����
%fwrite(com,[32*i+136,14],'uint8');
%
%    63-��ֵ 14λ
%    255-

%      0-��λ��16λ
%      0-

%      1 -
%      1 Ƶ�ʣ�15.���㷽���ǣ�AD9910Ƶ����32λ�Ĵ�����2^32
%      ��Ӧ1GHz,��������ÿһλ����һ���ֽڣ���2���Ƶ�8λ�����ֵ��2^8��������ֵ��1*2^0+1*2^8+1*2^16+1*2^24=freq,ʵ�����Ƶ��=1GHZ*freq/2^32,�˴���3.9MHz
%      1
%      1 -

% update
%out = fread(com,8,'uint8')
function [freq_B1,freq_B2,freq_B3,freq_B4]=encodeFreq(frequency_set,SYS_CLK)

%         # encode a frequency value in MHz from float to 4 bytes uint8 (32bits)
%         # DDS frequency range 0 - 1GHz (default)
%         # actual frequency value is:
%         #   1GHz *�� byte1 * 256^3 + byte2 * 256^2 + byte3 * 256 + byte4 ) / 2^32  
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
    frequency_set=10*10^6;%�趨��frequency��С
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
%         #   max_amp *�� byte1 * 256 + byte2 ) / 2^14
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
%         #   360 *�� byte1 * 256 + byte4 ) / 2^16
%         # max phase bytes are 255 255
    max_phase=360
    phase_i = min(floor(phase / max_phase * 2^16) , 2^16-1);
    phase_B1 = floor(phase_i / 2^8);
    phase_i = phase_i - phase_B1 * 2^8;
    phase_B2 = floor(phase_i);

end
      
% fclose(com);
