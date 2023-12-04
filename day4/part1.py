'''
I wrote this script to see where I was going wrong in my C program.
'''
import sys

with open(sys.argv[1]) as f:
    data = f.readlines()

total = 0
for line in data:
    line = line.strip()
    while ("  " in line):
        line = line.replace("  ", " ")

    parts = line.split(": ")[1].split(" | ")
    winners = [int(x) for x in parts[0].split(" ")]
    yours = [int(x) for x in parts[1].split(" ")]

    count = len(set(winners).intersection(set(yours)))
    points = 0 if count == 0 else 2**(count - 1)
    total += points

print(total)
