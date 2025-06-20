import matplotlib.pyplot as plt
import sys
import pandas as pd
import os
from datetime import datetime
time_format = "%Y-%m-%dT%H:%M:%S%z"
file = open(sys.argv[1], 'r')
first = True
rewrite_flag = False
rewrite = []
for line in file:
    if first:
        if str(line.split(",")[0]) != "time":
            rewrite.append("time,a,b,c,d\n")
            rewrite.append(line)
            first = False
            rewrite_flag = True
        else:
            break
    else:
        rewrite.append(line)
if rewrite_flag:
    file.close()
    file = open(sys.argv[1], 'w')
    file.truncate(0)
    file.writelines(rewrite)
file.close()
df = pd.read_csv(sys.argv[1])
time = df["time"]
ya = df["a"].to_list()
yb = df["b"].to_list()
yc = df["c"].to_list()
yd = df["d"].to_list()
fig, ax = plt.subplots(2, 2)
line_a, = ax[0,0].plot(time, ya, color='blue')
ax[0,0].set_title('a')
line_b, = ax[0,1].plot(time, yb, color='orange')
ax[0,1].set_title('b')
line_c, = ax[1,0].plot(time, yc, color='green')
ax[1,0].set_title('c')
line_d, = ax[1,1].plot(time, yd, color='red')
ax[1,1].set_title('d')
ax[0,1].legend([line_a, line_b, line_c, line_d], ['a', 'b', 'c', 'd'], bbox_to_anchor=(1.4, 1))
plt.tight_layout()
filename = "QuantumSensingOutput/{0}_{1}"
filename = filename.format(datetime.now().strftime(time_format),sys.argv[1].replace('/','-').replace('.',''))
os.makedirs(os.path.dirname(filename), exist_ok=True)
plt.savefig(filename+"_subplots")
ya_file = open(filename+"_ya.txt", 'w')
yb_file = open(filename+"_yb.txt", 'w')
yc_file = open(filename+"_yc.txt", 'w')
yd_file = open(filename+"_yd.txt", 'w')
ya_file.writelines([str(x)+"\n" for x in ya])
yb_file.writelines([str(x)+"\n" for x in yb])
yc_file.writelines([str(x)+"\n" for x in yc])
yd_file.writelines([str(x)+"\n" for x in yd])
plt.show()
