#!/bin/bash
# Program explanation:
# - The while loop is used to read line by line
# from the provided file.
# - The grep -o -E is being used to find all the
# digits in the given line. The -o flag specifies
# to only output the matches (each match on a newline)
# and the -E flag is used to specific using regex.
# - the head and tail are finding the first and last values.

# Check if a file was provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

# File name from the first argument
FILE=$1

# Check if the file exists
if [ ! -f "$FILE" ]; then
    echo "File not found: $FILE"
    exit 1
fi

# Initialize sum
sum=0

# Read each line
while IFS= read -r line; do
    # Extract the first digit
    first_digit=$(echo "$line" | grep -o -E '[0-9]' | head -1)
    
    # Extract the last digit
    last_digit=$(echo "$line" | grep -o -E '[0-9]' | tail -1)

    # Combine digits and add to sum
    if [ -n "$first_digit" ] && [ -n "$last_digit" ]; then
        value=$((first_digit * 10 + last_digit))
        sum=$((sum + value))
    fi
done < "$FILE"

# Output the sum
echo "$sum"
