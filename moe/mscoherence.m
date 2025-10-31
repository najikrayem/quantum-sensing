%fileID = fopen('QuantumSensingOutput/2025-08-15T11-50-22_-Downloads-2-rxtxt_yc.txt','r');
fileID = fopen('QuantumSensingOutput/2025-07-05T19-36-34_Downloads-log_secrettxt_yc.txt','r');
formatSpec = '%d';
c = fscanf(fileID,formatSpec);
tfe  = dsp.TransferFunctionEstimator("OutputCoherence",true);
%d = interp;
n = 70;
d = repmat(c(1:n),floor(length(c)/n),1);
d = cat(1,d,c(1:mod(length(c),n)));
cd = fft(d);
cc = fft(c);
%ps -> 6
% attempt = ifft(tfe(a,c).*cc-tfe(c,a).*ca-cc-ca);
%ps -> 14
%attempt = ifft(tfe(a,c).*ca - ca);
%ps -> 6 195,158
%ps -> 12 100,63
% [txy, cxy] = tfe(a,d);
% attempt = ifft(conj(sqrt(cxy)).*cd - cc);
%ps -> 7 70
[txy, cxy] = tfe(c,d);
attempt = ifft(cc - conj(sqrt(cxy)).*cd);
%this code is due to defects in the signal
%snipoutdefect
%attempt = cat(1,attempt(1:450), attempt(700:end));
subplot(3,1,1)
plot(c)
subplot(3,1,2)
plot(d)
subplot(3,1,3)
plot(attempt)
