import pandas as pd
import matplotlib.pyplot as plt

num_channels = 7  # Number of channels to read from the CSV file

# Read the CSV file
file_path = 'output.txt'  # Replace with your CSV file path
data = pd.read_csv(file_path, skiprows=20)

# remove last two rows
data = data[:-2]

# # Filter out rows with more than two columns
# data = data[data.apply(lambda row: len(row.dropna()) == num_channels, axis=1)]

# Ensure the CSV has exactly two columns
if data.shape[1] != num_channels:
    raise ValueError(f"The CSV file must have exactly {num_channels} columns.")

# Plot the two columns as separate time series
plt.plot(data.index, data.iloc[:, 1], label='x')
plt.plot(data.index, data.iloc[:, 2], label='y')
# Plot rectangles for ranges of 1s and 0s in the 'symbol' column
symbol = data.iloc[:, 6]
start = None
for i in range(len(symbol)):
    if start is None and symbol[i] == 1:
        start = i
    elif start is not None and symbol[i] == 0:
        plt.axvspan(start, i - 1, color='green', alpha=0.3, label='TONE' if start == 0 else "")
        start = None
if start is not None:
    plt.axvspan(start, len(symbol) - 1, color='green', alpha=0.3, label='TONE' if start == 0 else "")

# Plot rectangles for ranges of 1s and 0s in the 'BIT' column
bit = data.iloc[:, 4]
start = None
for i in range(len(bit)):
    if start is None and bit[i] == 1:
        start = i
    elif start is not None and bit[i] == 0:
        plt.axvspan(start, i - 1, color='red', alpha=0.3, label='BIT' if start == 0 else "")
        start = None
if start is not None:
    plt.axvspan(start, len(bit) - 1, color='red', alpha=0.3, label='BIT' if start == 0 else "")

# Plot the "sym_num" (second to last) column as a line
plt.plot(data.index, data.iloc[:, -2], label='sym_num', linestyle='--')

# Add labels, title, and legend
plt.xlabel('Index')
plt.ylabel('Values')
plt.title('Time Series Plot of Two Columns')
plt.legend()
plt.grid(True)
plt.show()