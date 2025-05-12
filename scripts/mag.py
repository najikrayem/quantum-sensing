import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

num_channels = 4  # Number of channels to read from the CSV file

"""
CH0 = COS_N
CH1 = COS_P
CH2 = SIN_P
CH3 = SIN_N

0   = COS_N
90  = SIN_N
180 = COS_P
270 = SIN_P

X=0-180
Y=90-270

Top of the board is 180 degrees
"""

# Read the CSV file
file_path = 'output.txt'  # Replace with your CSV file path
data = pd.read_csv(file_path, skiprows=1)

# remove last two rows
data = data[:-2]

# Ensure the CSV has exactly two columns
if data.shape[1] != num_channels:
    raise ValueError(f"The CSV file must have exactly {num_channels} columns.")


# subtract CH0 from CH1 and CH3 from CH2
x = data.iloc[:, 1] - data.iloc[:, 0]
y = data.iloc[:, 2] - data.iloc[:, 3]

# Normalize both columns to have an average of 0
# x = x - np.mean(x)
# y = y - np.mean(y)

# Normalize both columns to have a range of -1 to 1
# x = x / np.max(np.abs(x))
# y = y / np.max(np.abs(y))

# # now find the ratios of x and y
# xy = x + y
# x = x / xy
# y = y / xy

# for each 

# find the angle of x and y with respect to the origin
angle = np.arctan2(y, x)  # in radians
# convert to degree between 0 and 360, not -180 - 180
angle = np.degrees(angle)  # convert to degrees
angle = (angle + 360) % 360  # convert to 0-360 degrees
angle = (angle + 180) % 360  # rotate by 180 degrees



# Plot the two columns as separate time series
plt.plot(data.index, x, label='x')
plt.plot(data.index, y, label='y')
plt.plot(data.index, angle, label='angle')


# Add labels, title, and legend
plt.xlabel('Index')
plt.ylabel('Values')
plt.title('Time Series Plot of Two Columns')
plt.legend()
plt.grid(True)
plt.show()