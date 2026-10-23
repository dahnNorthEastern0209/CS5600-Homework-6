
#!/usr/bin/env bash
set -euo pipefail

OUTPUT="tlb-trial-comparison-ubuntu.csv"
PAGES=64

echo "run,pages,trials,ns_per_access" > "$OUTPUT"

for trials in 1000 10000 100000 1000000
do
    for run in 1 2 3 4 5
    do
        result=$(./tlb "$PAGES" "$trials")
        echo "$run,$result" >> "$OUTPUT"
    done

    echo "Completed: $trials trials"
done

echo "Saved results to $OUTPUT"
