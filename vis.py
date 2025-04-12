import pandas as pd
import matplotlib.pyplot as plt

num_channels = 4  # Number of channels to read from the CSV file

# Read the CSV file
file_path = 'output.txt'  # Replace with your CSV file path
data = pd.read_csv(file_path, skiprows=1)

# remove last two rows
data = data[:-2]

# # Filter out rows with more than two columns
# data = data[data.apply(lambda row: len(row.dropna()) == num_channels, axis=1)]

# Ensure the CSV has exactly two columns
if data.shape[1] != num_channels:
    raise ValueError(f"The CSV file must have exactly {num_channels} columns.")

# # normalize both columns to avg 0
# data.iloc[:, 0] = (data.iloc[:, 0] - data.iloc[:, 0].mean())
# data.iloc[:, 1] = (data.iloc[:, 1] - data.iloc[:, 1].mean())



# Plot the two columns as separate time series
plt.plot(data.index, data.iloc[:, 0], label='CH0')
plt.plot(data.index, data.iloc[:, 1], label='CH1')
plt.plot(data.index, data.iloc[:, 2], label='CH2')
plt.plot(data.index, data.iloc[:, 3], label='CH3')

# Add labels, title, and legend
plt.xlabel('Index')
plt.ylabel('Values')
plt.title('Time Series Plot of Two Columns')
plt.legend()
plt.grid(True)
plt.show()