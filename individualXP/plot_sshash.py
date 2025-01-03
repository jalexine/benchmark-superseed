import pandas as pd
import matplotlib.pyplot as plt
import sys

if len(sys.argv) < 3:
    print("♡ pls use: python sshashplot.py <input_file> <output_file>")
    sys.exit(1)

input_file = sys.argv[1]
output_file = sys.argv[2]

try:
    df = pd.read_csv(input_file)
except Exception as e:
    print(f"Error loading file: {e}")
    sys.exit(1)

# Convert columns to numeric
df["Size_MB"] = pd.to_numeric(df["Size_MB"])
df["N"] = pd.to_numeric(df["N"])

# Clean up the 'k' column by removing '.log' and converting it to integers
df["k"] = df["k"].str.replace(".log", "").astype(int)

plt.figure(figsize=(10, 6))
for n_value, group in df.groupby("N"):
    # Plot data for each value of N
    plt.plot(group["k"], group["Size_MB"], marker="o", label=f"N = {n_value}")

# Set explicit x-axis ticks
plt.xticks([21, 31, 41, 51, 61])
plt.xlabel("k-mer size (k)")
plt.ylabel("Index size (MB)")
plt.title("Index Size vs k-mer Size")
plt.legend(title="N values")
plt.grid()

# Adjust layout and save the plot
plt.tight_layout()
plt.savefig(output_file)
print(f"Plot saved as {output_file}")
