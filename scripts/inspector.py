#python3 scripts/inspector.py data_to_keep/0-19-5.txt -plt x:line,bit:line,sym:line,sym_num:changed--,bit_num:changed


import pandas as pd
import sys
import io
import numpy as np
import matplotlib.pyplot as plt


def plot_data(columns):
    # Plot all numerical columns as time series
    #numerical_columns = df.select_dtypes(include=[np.number]).columns

    for _, row in columns.iterrows():
        def changed(linestyle):
            changed_indices = df[col_name].ne(df[col_name].shift()).to_numpy().nonzero()[0]
            for idx in changed_indices:
                plt.axvline(x=idx, color='black', linestyle=linestyle, label=f"{col_name} change" if idx == changed_indices[0] else "")

        col_name = row["name"]
        plot_type = row["type"]

        if col_name not in df.columns:
            print(f"Column '{col_name}' not found in the DataFrame.")
            continue

        if plot_type == "line":
            df[col_name].plot(label=col_name, linestyle='-', marker=None)

        elif plot_type == "point":
            df[col_name].plot(label=col_name, linestyle='', marker='o')

        elif plot_type == "changed--":
            changed(linestyle='--')

        elif plot_type == "changed":
            changed(linestyle='-')

        elif plot_type == "binbox":
            color = next(plt.gca()._get_lines.prop_cycler)['color']
            # Find contiguous ranges where the column is equal to 1
            is_one = df[col_name] == 1
            ranges = []
            start = None

            for i, val in enumerate(is_one):
                if val and start is None:
                    start = i
                elif not val and start is not None:
                    ranges.append((start, i - 1))
                    start = None

            if start is not None:
                ranges.append((start, len(is_one) - 1))

            # Shade a box for each range
            for start, end in ranges:
                plt.axvspan(start - 0.5, end + 0.5, alpha=0.5, color=color, label=f"{col_name}" if start == ranges[0][0] else "")
        else:
            print(f"Unknown plot type '{plot_type}' for column '{col_name}'. Skipping.")
    

    plt.xlabel("Index")
    plt.ylabel("Values")
    plt.legend(title="Columns")
    plt.grid(False)
    plt.show()


def main():
    if len(sys.argv) < 2:
        print("Usage: python inspect.py <input_file>")
        sys.exit(1)

    input_file = sys.argv[1]

    try:
        # Load the CSV file, skipping the first n lines that start with "="
        with open(input_file, 'r') as file:
            lines = file.readlines()
        filtered_lines = [line for line in lines if not line.lstrip().startswith('=')]

        # Use the first row of the filtered lines as column names and load everything as integers
        df = pd.read_csv(io.StringIO(''.join(filtered_lines)), engine='python')

        # Drop all last n lines that contain NaNs
        while df.tail(1).isnull().any(axis=None):
            df = df.iloc[:-1]

    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)

    return df


if __name__ == "__main__":
    df = main()

    if "-plt" in sys.argv[1:]:
        plt_index = sys.argv.index("-plt")
        columns_to_plot = sys.argv[plt_index + 1].split(",") if plt_index + 1 < len(sys.argv) else []

        # Convert columns_to_plot to a DataFrame with 'name' and 'type' columns
        columns_df = pd.DataFrame(
            [col.split(":") for col in columns_to_plot],
            columns=["name", "type"]
        ).astype(str)


        if columns_to_plot:
            print(f"Plotting columns:")
            print(columns_df)
            plot_data(columns_df)
        else:
            print("No columns specified for plotting. Use -plt <columns> to plot specific columns.")
    

    if "-verify-tx-rx" in sys.argv[1:]:
        # Check if the DataFrame has the required columns
        required_columns = ['sample_num','x','y','bit_num','bit','sym_num','sym']
        if all(col in df.columns for col in required_columns):
            print("TX-RX-Fusion file columns are correct.")
        else:
            raise ValueError("TX-RX-Fusion file columns are incorrect. Required columns are: sample_num, x, y, bit_num, bit, sym_num, sym")
        
        # Group the data by the 'bit_num' column
        bit_grouped_data = df.groupby('bit_num')

        # Ensure that for each bit_num, there is an equal number of rows
        row_counts = bit_grouped_data.size()
        if row_counts.nunique() == 1:
            print(f"✓ Each bit_num has an equal number of rows: {row_counts.iloc[0]}")
        else:
            print(f"! The number of rows for each bit_num is not equal: {row_counts.unique()}")

        # ensure that each bitnum group as exactly one bit
        invalid_bit_groups = bit_grouped_data.filter(lambda group: group['bit'].nunique() != 1)
        if invalid_bit_groups.empty:
            print("✓ Each bit group has exactly one bit.")
        else:
            print("! Some bit groups do not have exactly one bit:")
            for bit_num, group in invalid_bit_groups.groupby('bit_num'):
                print(f"\tbit_num {bit_num} has {group['bit'].nunique()} unique bits.")

        
        # Group the data by the 'sym_num' column
        sym_grouped_data = df.groupby('sym_num')

        # Ensure that for each sym_num, there is an equal number of rows
        row_counts = sym_grouped_data.size()
        if row_counts.nunique() == 1:
            print(f"✓ Each sym_num has an equal number of rows: {row_counts.iloc[0]}")
        else:
            print(f"! The number of rows for each sym_num is not equal: {row_counts.unique()}")


        # Ensure that each sym_num group has exactly one sym
        invalid_sym_groups = sym_grouped_data.filter(lambda group: group['sym'].nunique() != 1)
        if invalid_sym_groups.empty:
            print("✓ Each sym group has exactly one sym.")
        else:
            print("! Some sym groups do not have exactly one sym:")
            for sym_num, group in invalid_sym_groups.groupby('sym_num'):
                print(f"\tsym_num {sym_num} has {group['sym'].nunique()} unique syms.")


        # Ensure that each bit group has exactly two unique sym_nums
        invalid_bit_groups = bit_grouped_data.filter(lambda group: group['sym_num'].nunique() != 2)

        if invalid_bit_groups.empty:
            print("✓ Each bit group has exactly two unique sym_nums.")
        else:
            print("! Some bit groups do not have exactly two unique sym_nums:")
            for bit_num, group in invalid_bit_groups.groupby('bit_num'):
                print(f"\tbit_num {bit_num} has {group['sym_num'].nunique()} unique sym_nums.")
