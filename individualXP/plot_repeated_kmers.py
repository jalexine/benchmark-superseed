import os
import matplotlib.pyplot as plt
import sys

# Get input files and output file from command line arguments
input_files = sys.argv[1:-1]
output_png = sys.argv[-1]

# Initialize a structure to store data
data = {}
dataset_name = None  # Variable to store the dataset name

# Loop through specified input files
for file_path in input_files:
    if file_path.endswith("_repeated.txt"):  # Target only relevant files
        # Extract k and N information from the file name
        file_name = os.path.basename(file_path)
        parts = file_name.split("_")
        fasta = parts[0]  # Example: "ecoli2"
        n_value = parts[1]  # Example: "N2"
        k_value = int(parts[2][1:])  # Example: "k31" -> 31

        # Set dataset_name from the first file
        if dataset_name is None:
            dataset_name = fasta

        # Count the number of lines in the file to get the repeated k-mer count
        with open(file_path, "r") as f:
            repeated_kmers_count = sum(1 for _ in f)

        # Add data to the dictionary
        if n_value not in data:
            data[n_value] = {}
        data[n_value][k_value] = repeated_kmers_count

# Prepare data for plotting
sorted_k_values = sorted({k for n_data in data.values() for k in n_data.keys()})
n_values = sorted(data.keys())

# Plot the data
plt.figure(figsize=(10, 6))
for n_value in n_values:
    counts = [data[n_value].get(k, 0) for k in sorted_k_values]
    plt.plot(sorted_k_values, counts, marker="o", label=n_value)

# Add plot details
plt.title(f"{dataset_name}" if dataset_name else "Repeated k-mers")
plt.xlabel("k")
plt.ylabel("Number of repeated k-mers")
plt.xticks(sorted_k_values)  # Explicit x-axis values
plt.legend(title="N values")
plt.grid(True)

# Save the plot as PNG
plt.savefig(output_png)
print(f"Plot saved to {output_png}")
