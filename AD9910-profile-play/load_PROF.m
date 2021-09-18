freq_set=[100,110,120,150,200,300,350,400];
amp_set=rand(1,8);
phase_set=rand(1,8)*360;
time_set=[50,20,50,70,80,90,120];
for i=1:length(freq_set)
    freq_single=freq_set(i);
    amp_single=amp_set(i);
    angle_single=phase_set(i);
    profile_i=i;
    write_profile(profile_i,freq_single,amp_single,angle_single)
end
play_profile(time_set)

function write_profile(profile_i,freq_single,amp_single,phase_single)
    profile_register_index=[0x0C,0x0E,0x10,0x12,0x14,0x16,0x18,0x1A];
    [freq_B1,freq_B2,freq_B3,freq_B4]=encodeFreq(freq_single,SYS_CLK);
    [amp_B1,amp_B2]=encodeAmp(amp_single,AMP_Max);
    [phase_B1, phase_B2]=encodePhase(phase_single, max_phase);
    fwrite(com,[32*0+8,profile_register_index(profile_i),amp_B1,amp_B2,phase_B1,phase_B2,freq_B1,freq_B2,freq_B3,freq_B4],'uint8');%数据长度8，寄存器地址E，幅值,相位，控制频率fwrite(com,[32*i+8,14,63,255,0,0,66,143,92,40],'uint8');
end
function []=read_profile()
    fwrite(com,[32*i+136,14],'uint8');
    read_all=fread(com,8,'uint8');
    
    
end

function play_profile(time_set)
% fwrite()

end
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
