
fileID = fopen('QuantumSensingOutput/2025-06-19T21:45:14_Downloads-log_secrettxt_yb.txt','r');
formatSpec = '%d';
x = fscanf(fileID,formatSpec);
most = max(x);
least = min(x);
[yupper2, ylower2] = envelope(x,6 , 'peak');
output = quantalph(yupper2, [min(x), max(x)]);
tone = false;
tone_entry = 0;
notone_entry = 0;
notones = [];
tones = [];
order = [];
binary = [];
for i=1:length(output)
    tone_temp = tone;
    tone  = output(i) == max(x);
    if tone
        tone_entry = tone_entry + 1;
    else
        notone_entry =notone_entry+1;
    end 
    if tone_temp ~= tone
        if tone
            notones = [notones notone_entry];
            notone_entry = 0;
            order = [order false];
        else
            tones = [tones tone_entry];
            tone_entry = 0;
            order = [order true];
        end

    end
    
end
Ti = 1;
NTi = 1;
twoT = 2*min(tones);
twoNT = 2*min(notones);
for i=order
    if i
        if tones(Ti) >= twoT
            binary = [binary 1 1];
        else
            binary = [binary 1];
        end
        Ti = Ti + 1;
    else
        if notones(NTi) >= twoNT
            binary = [binary 0 0];
        else
            binary = [binary 0];
        end
        NTi = NTi + 1;
    end
end

binary = binary(3:end);
msg = "";
lastbit = 0;
for i = 1:length(binary)/2
    if lastbit ~= binary((i*2)-1)
        msg = msg + "0";
    else
        msg = msg + "1";
    end
    lastbit = binary((i*2));
end
msg