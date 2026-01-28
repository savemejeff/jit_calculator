#!/usr/bin/env bash

set -euo pipefail

while IFS= read -r line; do
    tmpfile=$(mktemp)

    # Save input to temp file
    printf '%s\n' "$line" > "$tmpfile"

    # Run your command on the file
    ./bin/calculator "$tmpfile"

    # Delete temp file
    rm -f "$tmpfile"
done
