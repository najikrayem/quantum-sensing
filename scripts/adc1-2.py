import sys
import pandas as pd

def read_file_as_set(file_path):
    try:
        with open(file_path, 'r') as file:
            lines = set(file.readlines())
        return lines
    except FileNotFoundError:
        print(f"Error: The file at '{file_path}' was not found.")
        sys.exit(1)
    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)


def process_lines(lines):

    extracted_data = pd.DataFrame(columns=['time', 'adc', 'value'])
    for line in lines:
        sep_line = line.strip().split()
        if len(sep_line) != 6:
            print(f"Skipping line due to unexpected format: {line.strip()}")
            continue
        
        if sep_line[0] == 'ADC1':
            adc = 1
        elif sep_line[0] == 'ADC2':
            adc = 2
        else:
            print(f"Skipping line due to unexpected ADC type: {line.strip()}")
            continue

        value = float(sep_line[2])
        time = int(sep_line[5])

        extracted_data = pd.concat([extracted_data, pd.DataFrame([{'time': time, 'adc': adc, 'value': value}])], ignore_index=True)

    extracted_data = extracted_data.sort_values(by='time').reset_index(drop=True)

    return extracted_data
#enddef

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python adc1-2.py <in_file_path> <out_file_path>")
        sys.exit(1)


    in_file_path = sys.argv[1]
    out_file_path = sys.argv[2]
    lines_set = read_file_as_set(in_file_path)
    extracted_data = process_lines(lines_set)
    extracted_data.to_csv(out_file_path, index=False)
    print(f"Data extracted and saved to {out_file_path}")
