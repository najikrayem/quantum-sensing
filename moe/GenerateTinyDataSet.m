tfe  = dsp.TransferFunctionEstimator();
base_sigs = 40000.*randn(10,1000000);
base_index = 1;
output_transfer = zeros(10,1000000);
correct_transfer = zeros(10,1000000);
for j=1:1000000
    if mod(j,250) == 1
        j
    end
    base_sig = base_sigs(:,j);
    correct_transfer(:,j) = rand(10,1)+1i.*rand(10,1);
    output_transfer(:,j) = tfe(base_sig, ifft(fft(base_sig).*(correct_transfer(:,j))));
end
writematrix(correct_transfer,'CorrectTiny.csv')
writematrix(output_transfer,'OutputTiny.csv')