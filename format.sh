#!/bin/bash

files=$(find src tests \( -name "*.c" -o -name "*.h" \))
count=0

while IFS= read -r file; do
    original=$(cat "$file")
    clang-format -i "$file"
    formatted=$(cat "$file")

    if [ "$original" != "$formatted" ]; then
        echo "Formatted: $file"
        ((count++))
    fi
done <<<"$files"

echo "Done: $count file(s) formatted."
