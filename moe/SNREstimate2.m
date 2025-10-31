% tx = fscanf(fileID,formatSpec);
fileID = fopen('QuantumSensingOutput/2025-06-19T21-45-14_Downloads-log_secrettxt_yb.txt','r');
formatSpec = '%d';
rx = fscanf(fileID,formatSpec);
noise = zeros(length(rx),1);
noise(noise<1) = min(rx);
signal = rx - min(rx);
[wd,lo,hi,powersig] = obw(signal,16);
powtotsig = powersig/0.99
[wd,lo,hi,powernoise] = obw(noise,16);
powtotnoise = powernoise/0.99
(powtotsig/20)/powtotnoise
snrpess = 10.*log10((powtotsig/20)/(powtotnoise+20*powtotsig))
snropt = 10.*log10((powtotsig/20)/(powtotnoise))