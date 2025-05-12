import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Constants -------------------------------------------------------------------

NUM_CHAN = 4  # Number of channels to read from the CSV file

NUM_PERIODS = 5  # Number of periods of the TX_FREQ to analyze in each block

TX_FREQ = 1  # Frequency of the antenna in Hz
RX_FREQ = 4  # Sampling rate in Hz

FILE = 'output.txt'  # Replace with your CSV file path

# Functions -------------------------------------------------------------------

def goertzel(block, sample_rate, freq):
    block_size = len(block)
    k = int(0.5 + ((block_size * freq) / sample_rate))
    w = (2.0 * np.pi / block_size) * k
    cosine = np.cos(w)
    sine = np.sin(w)
    coeff = 2.0 * cosine

    q1 = 0
    q2 = 0

    for val in block:
        q0 = coeff * q1 - q2 + val
        q2 = q1
        q1 = q0
    # end for

    real = (q1 - q2 * cosine)
    imag = (q2 * sine)
    magnitude = np.sqrt(real * real + imag * imag)
    #magnitude = real*real + imag*imag
    return (real, imag, magnitude)
# end goertzel

# Main ------------------------------------------------------------------------


# Read the CSV file
data = pd.read_csv(FILE, skiprows=1)
# remove last two rows
data = data[:-2]
# Ensure the CSV has exactly two columns
if data.shape[1] != NUM_CHAN:
    raise ValueError(f"The CSV file must have exactly {NUM_CHAN} columns.")


# determine block size of goertzel such that it contains NUM_PERIODS of the TX_FREQ
block_size = int(RX_FREQ / TX_FREQ * NUM_PERIODS)

# subtract CH0 from CH1 and CH3 from CH2
x = data.iloc[:, 1] - data.iloc[:, 0]
y = data.iloc[:, 2] - data.iloc[:, 3]

# divide x and y into blocks of block_size and run goertzel on each block
# pad with zeros if the length of x and y is not a multiple of block_size
if len(x) % block_size != 0:
    x = np.pad(x, (0, block_size - len(x) % block_size), 'constant')
if len(y) % block_size != 0:
    y = np.pad(y, (0, block_size - len(y) % block_size), 'constant')
x_blocks = np.split(x, len(x) // block_size)
y_blocks = np.split(y, len(y) // block_size)


# run goertzel on each block
print(x_blocks)
x_results = [goertzel(block, RX_FREQ, TX_FREQ) for block in x_blocks]
y_results = [goertzel(block, RX_FREQ, TX_FREQ) for block in y_blocks]

# get the real and imaginary parts of the results
x_real = np.array([result[0] for result in x_results])
x_imag = np.array([result[1] for result in x_results])
y_real = np.array([result[0] for result in y_results])
y_imag = np.array([result[1] for result in y_results])
x_mag = np.array([result[2] for result in x_results])
y_mag = np.array([result[2] for result in y_results])


# find the phases of x and y
x_phase = np.arctan2(x_imag, x_real)  # in radians
y_phase = np.arctan2(y_imag, y_real)  # in radians


# calculate the phase difference between x_phase and y_phase
phase_diff = x_phase - y_phase
phase_diff = (phase_diff + np.pi) % (2 * np.pi) - np.pi  # normalize to range [-π, π]

# calculate the angle of the device using the magnitude and phase difference
angle = np.arctan2(y_mag, x_mag) + phase_diff  # incorporate phase difference
angle = np.degrees(angle)
angle = (angle + 360) % 360  # normalize to range [0, 360]
angle = (angle + 180) % 360  # rotate by 180 degrees


# plot the signals, the goertzel results and the phases all on the same plot
plt.figure(figsize=(12, 8))
plt.subplot(3, 1, 1)
plt.plot(x, label='x')
plt.plot(y, label='y')
plt.title('Signals')
plt.legend()

plt.subplot(3, 1, 2)
plt.plot(x_mag, label='x_mag')
plt.plot(y_mag, label='y_mag')
plt.plot(x_mag+y_mag, label='Sum')
plt.title('Goertzel Results')
plt.legend()


plt.subplot(3, 1, 3)
plt.plot(x_phase, label='x_phase')
plt.plot(y_phase, label='y_phase')
plt.title('Phases')
plt.legend()
plt.tight_layout()
plt.show()

# add the angle to the plot
plt.figure(figsize=(12, 4))
plt.plot(angle, label='angle')
plt.title('Angle')
plt.xlabel('Block Number')
plt.ylabel('Angle (degrees)')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()

