import pandas as pd
import sys
import io
import numpy as np

def proc_normalize(df, target_count, x_avg, y_avg, x_std, y_std):

    # Step 1: Find groups where sym == 0
    sym_zero_groups = df[df['sym'] == 0].groupby('sym_num')

    # Step 2: Count rows in each group
    group_sizes = sym_zero_groups.size()

    # Step 3: Identify groups needing padding
    to_pad = group_sizes[group_sizes < target_count]
    to_truncate = group_sizes[group_sizes > target_count]

    print(f"sym 0 to truncate: {to_truncate}")
    print(f"sym 0 group_sizes: {group_sizes.value_counts()}")

    # Step 3.1: Truncate groups that are too long
    # Remove rows with sample_num in the to_truncate list
    for sym_num_val, count in to_truncate.items():
        indices_to_remove = df[(df['sym_num'] == sym_num_val) & (df['sym_num'] > target_count)].index
        df.drop(indices_to_remove, inplace=True)
        #df['removed_row'] = True  # Set the removed_row flag

    # Step 4: Create padding rows
    new_rows = []
    for sym_num_val, count in to_pad.items():
        num_to_add = target_count - count

        # Create a template row with default values
        template = df[df['sym_num'] == sym_num_val].iloc[-1].copy()
        for _ in range(num_to_add):
            template['sample_num'] = template['sample_num'] + 1
            template['x'] = int(np.random.normal(x_avg, x_std))
            template['y'] = int(np.random.normal(y_avg, y_std))
            new_rows.append(template.copy())

    # Step 5: Append and reset index if needed
    if new_rows:
        df = pd.concat([df, pd.DataFrame(new_rows)], ignore_index=True)
    df = df.sort_values(by=['sym_num', 'sample_num']).reset_index(drop=True)
    df['sample_num'] = df.index

    # Step 6: truncate the sym_1 groups to the target_count
    sym_one_groups = df[df['sym'] == 1].groupby('sym_num')
    for sym_num_val, group in sym_one_groups:
        group_length = len(group)
        if group_length > target_count:
            # Remove the last rows to truncate
            indices_to_remove = group.index[-(group_length - target_count):]
            df.drop(indices_to_remove, inplace=True)

    # Step 7: Reset the index
    df = df.reset_index(drop=True)
    df['sample_num'] = df.index

    # Step 8: pad the sym_1 groups to the target_count
    sym_one_groups = df[df['sym'] == 1].groupby('sym_num')
    for sym_num_val, group in sym_one_groups:
        group_length = len(group)
        if group_length < target_count:
            num_to_add = target_count - group_length
            # Create a template row with default values
            template = group.iloc[-1].copy()
            for _ in range(num_to_add):
                template['sample_num'] = template['sample_num'] + 1
                template['x'] = int(np.random.normal(x_avg, x_std))
                template['y'] = int(np.random.normal(y_avg, y_std))
                df = pd.concat([df, pd.DataFrame([template.copy()])], ignore_index=True)

    # Step 9: Reset the index again
    df = df.sort_values(by=['sym_num', 'sample_num']).reset_index(drop=True)
    df['sample_num'] = df.index

    return df
#end proc_normalize




def proc_df(df):

    # Convert all columns to integers if possible
    for column in df.columns:
        try:
            df[column] = pd.to_numeric(df[column], downcast='integer', errors='coerce').fillna(0).astype(int)
        except ValueError:
            pass

    # Add two new columns: 'bit_changed' and 'sym_changed'
    df['bit_changed'] = df['bit_num'] != df['bit_num'].shift()
    df['sym_changed'] = df['sym_num'] != df['sym_num'].shift()

    # Reset 'sym_num' and 'bit_num' columns to all zeros
    df['sym_num'] = 0
    df['bit_num'] = 0

    bit_counter = -1
    sym_counter = -1

    for index, row in df.iterrows():
        if row['bit_changed']:
            bit_counter += 1
        if row['bit_changed'] or row['sym_changed']:
            sym_counter += 1

        df.at[index, 'bit_num'] = bit_counter
        df.at[index, 'sym_num'] = sym_counter
    
    # Drop the 'bit_changed' and 'sym_changed' columns
    df.drop(columns=['bit_changed', 'sym_changed'], inplace=True)


    # remove the last bit if the flag "-rm-end" is set
    if "-rm-end" in sys.argv:
        greatest_bit_num = df['bit_num'].max()
        print(f"Removing the last bit at index {greatest_bit_num}")
        df = df[df['bit_num'] != greatest_bit_num]
    

    # make all NO_TONE envelopes equal in length to the TONE envelopes if the flag "-norm" is set
    if "-norm" in sys.argv:

        # Get the most common lengths of the sym_num_1 groups
        sym_one = df[df['sym'] == 1]
        sym_one_lengths = sym_one.groupby('sym_num').size()
        most_common_length = sym_one_lengths.mode()[0]

        # Get x and y distributions of the sym_num_0 groups
        sym_zero = df[df['sym'] == 0]
        x_avg = sym_zero['x'].mean()
        x_std = sym_zero['x'].std()
        y_avg = sym_zero['y'].mean()
        y_std = sym_zero['y'].std()

        df = proc_normalize(df, most_common_length, x_avg, y_avg, x_std, y_std)
    
    return df

#end proc_df


def main():
    if len(sys.argv) < 2:
        print("Usage: python sym_num_fix.py <input_file> [<output_file>]")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 and not sys.argv[2].startswith('-') else None

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

        # Process the DataFrame here
        df = proc_df(df)


        # Place the original comment lines back at the top of the file
        original_comments = [line for line in lines if line.lstrip().startswith('=')]

        def save_file(file_path):
            with open(file_path, 'w') as file:
                file.writelines(original_comments)
                df.to_csv(file, index=False)


        if output_file:
            save_file(output_file)
            print(f"File saved to {output_file}")
        else:
            # Ask for user confirmation before overwriting the input file
            confirm = input(f"No output file provided. Overwrite the input file '{input_file}'? (yes/no): ").strip().lower()
            if confirm in ['yes', 'y']:
                save_file(input_file)
                print(f"File overwritten: {input_file}")
            else:
                print("Operation canceled. No file was overwritten.")

    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    main()