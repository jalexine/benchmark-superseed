import matplotlib.pyplot as plt
import argparse
import os

# Argument parser
parser = argparse.ArgumentParser(description="Plot sum length vs k for different N values.")
parser.add_argument("input_file", help="Path to the input file.")
parser.add_argument("output_file", help="Path to save the output plot.")

args = parser.parse_args()
file_path = args.input_file
output_file = args.output_file

# Extract the base name of the file
title_name = os.path.basename(file_path).split(".")[1]  # Adjust based on the file naming convention

# Initialize a dictionary to store data
# Structure: {N_value: {k_value: sum_length}}
data = {}

# Read and extract data
with open(file_path, "r") as file:
    for line in file:
        if line.strip() and not line.startswith("K"):  # Skip headers
            parts = line.split()  # Split the line
            k = int(parts[0])  # Extract k
            N = int(parts[1])  # Extract N
            sum_length = int(parts[6].replace(",", ""))  # Extract and parse sum_length

            if N not in data:
                data[N] = {}
            data[N][k] = sum_length

# Prepare data for plotting
sorted_k_values = sorted({k for n_data in data.values() for k in n_data.keys()})
sorted_N_values = sorted(data.keys())

# Plot the data
plt.figure(figsize=(10, 6))
for N in sorted_N_values:
    sum_lengths = [data[N].get(k, 0) for k in sorted_k_values]
    plt.plot(sorted_k_values, sum_lengths, marker="o", label=f"N = {N}")

# Add plot details
plt.title(f"{title_name}")
plt.xlabel("k")
plt.ylabel("Sum Length")
plt.xticks(sorted_k_values)  
plt.legend(title="N values")
plt.grid(True)

# Save the plot
plt.savefig(output_file)
print(f"Plot saved to {output_file}")
