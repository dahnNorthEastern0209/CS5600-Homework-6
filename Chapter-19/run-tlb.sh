
#!/usr/bin/env bash
set -euo pipefail

OUTPUT="${1:-tlb-results.csv}"
REPEATS=5
TARGET_ACCESSES=10000000

echo "run,pages,trials,ns_per_access" > "$OUTPUT"

for pages in 1 2 4 8 16 32 64 128 256 512 1024 2048 4096
do
    trials=$((TARGET_ACCESSES / pages))

    if (( trials < 1000 )); then
        trials=1000
    fi

    for (( run=1; run<=REPEATS; run++ ))
    do
        result=$(./tlb "$pages" "$trials")
        echo "$run,$result" >> "$OUTPUT"
    done

    echo "Completed: $pages pages"
done

echo "Saved results to $OUTPUT"
