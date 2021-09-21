function single_tone_out(MHz,kHz,Hz,Amp_dB)
global com;
request1=['Single Tone ',num2str(MHz,'%03d'),'MHz',num2str(kHz,'%03d'),'kHz',num2str(Hz,'%03d'),'HHz', num2str(-1*Amp_dB,'%03d'),'dBm']
% request1=strcat("Single Tone ", num2str(-1*Amp_dB),'dBm',num2str(MHz),'MHz',num2str(kHz),'kHz',num2str(Hz),'HHz')

if ((MHz*10^6 + kHz*10^3 + Hz)>600*10^6)
    print('frequency is larger than 600MHz\n')
else
    if (Amp_dB>0|Amp_dB<-84)
        print('Amp should between 0 to -84dBm\n')
    else
        fprintf(com,'%s\n',request1);
    end
end
read_data=fread(com)
% MHz_read=read_data
end