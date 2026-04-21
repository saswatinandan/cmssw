import pandas as pd
import argparse
import os

# Set up argument parser
parser = argparse.ArgumentParser(description="Process a text file and display it as a table.")
parser.add_argument('filename', type=str, help="The path to the text file to be processed")
parser.add_argument('-o', '--output_file', type=str, help="The path to save the output (default: same as input filename)")
parser.add_argument('-f', '--format', type=str, help="The output of the format (default: md)", default="md")
args = parser.parse_args()

# Set default output filename if not provided
if args.output_file is None:
    base_name = os.path.splitext(args.filename)[0]
    args.output_file = f"{base_name}.{args.format}"

# Load the data from the file specified in the argument
with open(args.filename, 'r') as file:
    lines = file.readlines()

# Extract metadata information from the first line
file_info = lines[0].strip()

# Extract headers from the second line
headers = lines[1].strip().split(" | ")

# Extract data rows, split by whitespace for each row
data = [line.strip().split() for line in lines[2:]]

# Create DataFrame
df = pd.DataFrame(data, columns=headers)

# Convert appropriate columns to numeric, if applicable
for col in headers[1:]:  # Skipping the first column since it is a name
    df[col] = pd.to_numeric(df[col], errors='coerce')

# Display metadata info and the table
print("File Info:", file_info)
if args.format == "md":
    df.to_markdown(args.output_file, index=False)
elif args.format == "xls" or args.format == "xlsx":
    df.to_excel(args.output_file, index=False)
else:
    df.to_csv(args.output_file, index=False)
print(f"Data saved to {args.output_file}")
