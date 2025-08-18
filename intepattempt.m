%fileID = fopen('QuantumSensingOutput/2025-08-15T11-50-22_-Downloads-2-rxtxt_yc.txt','r');
fileID = fopen('QuantumSensingOutput/2025-07-05T19-36-34_Downloads-log_secrettxt_yc.txt','r');
formatSpec = '%d';
c = fscanf(fileID,formatSpec);
interp_method = 'linear';
h = 19300;
l = 18950;
time_b = [];
d_b = [];
for i=1:length(c)
    if c(i) <= l || c(i) >= h
    else
        time_b = [time_b, i];
        d_b = [d_b c(i)];
    end
end
interp =interp1(time_b, d_b, 1:length(c), interp_method).';
plot(interp)
%mscoherence