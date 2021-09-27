%% 通讯协议
%输入字符串，字节位
% byte1 update 128 len = b & 0x1F==0
%       read   >128,=128+数据长度，b & 0x80第一位为1，同时len = b & 0x1F!=0
%       write  数据长度，通常是4或8, b & 0x80第一位为0
%同时byte1的最后五位2进制是用来确定选择哪一个slave片的，用32*i中的i=0,1,2,3来判断
% byte2 寄存器地址 0~32，常用地址14 （0x0E），是波形参数寄存器
% byte3~ 数据 
%      data byte 1 2| 3 4| 5 6 7 8
%      data byte 1~2 幅值参数14位，最大2^14对应幅值幅值5V，峰峰值10V，输入数据对应（byte1*128+byte2）/2^14 *5V
%      data byte 3~4 相位参数16位
%      data byte 5~8 频率参数32位，最大模拟量1GHz，实际模拟输出1GHz*（byte5*256^3+byte6*256^2+byte7*256+byte8)/2^32


% Instrument Connection
clear
global com;
% Find a serial port object.
com = instrfind('Type', 'serial', 'Port', 'COM5', 'Tag', '','baudRate',9600);

% Create the serial port object if it does not exist
% otherwise use the object that was found.
if isempty(com)
    com = serial('COM5');
else
    fclose(com);
    com = com(1);
end

% Connect to instrument object, obj1.
fopen(com); %如果此时串口被其他程序占用，会报错，需要重启单片机
pause(3);