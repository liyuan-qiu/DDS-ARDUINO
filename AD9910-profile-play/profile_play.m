global com;
freq_set=[300,300,300,300,300,300,300,300];
amp_set=(-35:5:-0);
Profile_set=[0,1,2,3,4,5,6,7];
for count1=1:length(freq_set)
    load_PROF_out(freq_set(count1)*10^6,amp_set(count1),Profile_set(count1));
    pause(1)
end
time_set=(0.1:0.1:0.8)*1000;%delay us
time_length=length(time_set);
request1=['Profile Play ',num2str(time_length,'%02d'),'group']
for count2=1:length(time_set)
    request1=[request1,num2str(time_set(count2),'%06d')];
end
request1
fprintf(com,'%s\n',request1)