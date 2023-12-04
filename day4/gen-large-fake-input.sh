#!/bin/bash
# Generate an input of 800 mb for testing purposes.
# Note that numbers are repeated from input.txt

IN="input.txt"
TMP="tmp.txt"
OUT="long-fake-input.txt"

cat "$IN" "$IN" > "$OUT"

for i in {1..14}; do
    cat "$OUT" "$OUT" > "$TMP" && mv "$TMP" "$OUT"
done
