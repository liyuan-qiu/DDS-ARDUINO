global com;
freq_set=(100:50:400);%MHz
amp_set=ones(1,8)*(-20);%dBm
Phase_set=ones(1,8)*(0);%degree
Profile_set=[0,1,2,3,4,5,6,7];

time_set=ones(1,4)*(1*10^6);%delay us
profile_list=[0,1,2,3,4,5,6,7,6,5,4,3,2,1];
time_length=min(length(time_set),length(profile_list))
if (sum(Profile_set>7)|sum(Profile_set<0)|sum(profile_list>7)|sum(profile_list<0))
    print('Profile should be 0,1,2....7')
else
    for count1=1:min([length(freq_set),length(freq_set),length(Profile_set)],8)
        load_PROF_out(freq_set(count1)*10^6,amp_set(count1),Phase_set(count1),Profile_set(count1));
        pause(1)
    end
% time_set=[1,2,3,4,5,6,7]*50;%delay us

    request1=['Profile Play ',num2str(time_length,'%02d'),'group']
    for count2=1:time_length
        request1=[request1,num2str(time_set(count2),'%010d'),num2str(profile_list(count2),'%01d')];
        
    end
    request1
    length(request1)%Arduino Mega buffer size is 64byte, to solve this problem, I will send the order of ms/us to shorten the request1 length
    fprintf(com,'%s\n',request1)
end

