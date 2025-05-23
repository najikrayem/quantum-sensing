import pandas as pd
import sys
import io


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