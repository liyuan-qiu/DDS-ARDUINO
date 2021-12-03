function load_PROF_out(freq_set,amp_set,prof_num)
% from ad9910.cpp void DDS_Fout (uint32_t *F_OUT, int16_t Amplitude_dB, uint8_t Num_Prof)
global com;
request1=['Profile Load ',num2str(freq_set),'Hz',num2str(amp_set),'dBm',num2str(prof_num,'%01d'),'prof_set']
if (freq_set>600*10^6)
    print('frequency is larger than 600MHz\n')
else
    if (amp_set>0|amp_set<-84)
        print('Amp should between 0 to -84dBm\n')
    else
        if (prof_num<0|prof_num>7)
            print('Profile should between 0 to 7\n')
        else
            fprintf(com,'%s\n',request1);
        end
    end
end

end
