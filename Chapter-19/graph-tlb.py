import csv
import statistics
from collections import defaultdict

import matplotlib
matplotlib.use("Agg")

import matplotlib.pyplot as plt

def load_results(filename):
    measurements = defaultdict(list)

    with open(filename, newline="") as file:
        reader = csv.DictReader(file)

        for row in reader:
            pages = int(row["pages"])
            access_time = float(row["ns_per_access"])
            measurements[pages].append(access_time)

    pages = sorted(measurements)

    medians = [
        statistics.median(measurements[p])
        for p in pages
    ]

    minimums = [
        min(measurements[p])
        for p in pages
    ]

    maximums = [
        max(measurements[p])
        for p in pages
    ]

    return pages, medians, minimums, maximums

ubuntu = load_results("tlb-results-ubuntu-vm.csv")
khoury = load_results("tlb-results-khoury.csv")

plt.figure(figsize=(11, 6))

for label, results, color in [
    ("Ubuntu VirtualBox VM", ubuntu, "royalblue"),
    ("Khoury Linux Server", khoury, "darkorange"),
]:
    pages, medians, minimums, maximums = results

    plt.plot(
        pages,
        medians,
        marker="o",
        linewidth=2,
        label=label,
        color=color,
    )

    plt.fill_between(
        pages,
        minimums,
        maximums,
        alpha=0.15,
        color=color,
    )

plt.xscale("log", base=2)

plt.xticks(
    [2**i for i in range(13)],
    [str(2**i) for i in range(13)],
    rotation=45,
)

plt.xlabel("Number of Pages Accessed")
plt.ylabel("Average Memory Access Time (ns/access)")
plt.title("TLB Performance: Ubuntu VM vs. Khoury Server")

plt.grid(True, alpha=0.3)
plt.legend()
plt.tight_layout()

plt.savefig("tlb-comparison.png", dpi=300)

print("Graph saved to tlb-comparison.png")

