#!/usr/bin/env python3
import csv
import os
import sys
import matplotlib.pyplot as plt
from collections import defaultdict

def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <summary.csv>")
        sys.exit(1)
    
    in_csv = sys.argv[1]
    
    # data[(f1, f2)][n][k] = jaccard_score
    data = defaultdict(lambda: defaultdict(dict))
    # store just one ANI per (f1, f2).
    skani_by_pair = {}

    # 1) read the CSV
    with open(in_csv, "r") as fh:
        reader = csv.DictReader(fh)
        for row in reader:
            f1 = row["fasta1"]
            f2 = row["fasta2"]
            n  = int(row["n"])
            k  = int(row["k"])

            # jaccard
            jaccard = float(row["jaccard_score"])
            data[(f1, f2)][n][k] = jaccard

            # store the ANI in a dict
            if (f1, f2) not in skani_by_pair:
                skani_by_pair[(f1, f2)] = float(row["skani_dist"])

    # 2) create a directory for plots (if needed)
    os.makedirs("results/plots", exist_ok=True)

    # 3) for each pair, plot “Jaccard vs. k” with lines for different n
    #    and a horizontal dash line for the ANI (scaled by /100).
    for (f1, f2), n_dict in data.items():
        plt.figure(figsize=(7,5))

        # plot each N as a separate line
        for n_val in sorted(n_dict.keys()):
            k_dict = n_dict[n_val]
            k_vals = sorted(k_dict.keys())
            scores = [k_dict[kx] for kx in k_vals]
            
            plt.plot(k_vals, scores, marker='o', label=f"N={n_val}")

        # add a horizontal line for ANI (scaled to 0..1 range).
        if (f1, f2) in skani_by_pair:
            ani_val = skani_by_pair[(f1, f2)]
            ani_jacc = ani_val / 100.0
            plt.axhline(ani_jacc, color="purple", linestyle="--",
                        label=f"ANI = {ani_val}")

        # labels and styling, no pink sorry
        plt.xlabel("K Values")
        plt.ylabel("Jaccard Score")
        plt.title(f"Jaccard vs K for {f1}_{f2}")
        plt.grid(True)
        plt.legend()

        plt.xticks([21, 31, 41, 51, 61])

        # 4) save figure as a png
        out_png = os.path.join("results/plots", f"jaccard_{f1}_{f2}.png")
        plt.savefig(out_png, dpi=150, bbox_inches="tight")
        plt.close()
        print(f"[+] Created plot: {out_png}")

if __name__ == "__main__":
    main()
