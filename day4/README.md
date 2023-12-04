# Day 4

Part 1 of this was quite fun because I tried to optimize every little
instruction I could think of.

### Bitwise Set/HashSet?

The core concept of today's challenge was to check if a given number is in a set
of "winning numbers". There's the obvious way, to just check each element
against every other element to see if it exists. However, I wanted to think of a
way to speed this up. The first idea I had was to use a hashset to look if a
given number is a winning number. I though I would have an array and if a number
`x` was a winning number then `array[x]` would be set to `1`. That's when I
has the idea to just use a single number and use the bits of that number to keep
track of the numbers we've seen (`x`).

How it works:

The idea is simple, have a long sequence of bits and if number `x` is a winning
number then set bit `x` to be `1`. To check if a number if a winning number then
we can just perform a bitwise and operation on our bit string and check if it's
not 0.

Implementation:

I needed at least `99` bits since that the maximum number that appeared in the
`input.txt` file. So, I just chose the next power of 2 (128). A normal computer
cannot operate on `128` bits (usually), it can however (like my computer),
operator on `64` bits in a single instruction. Therefore, we have the struct:

```c
typedef struct {
    int64_t a;
    int64_t b;
} HashMap;
```

`a` is going to hold number from `0->63` and `b` is going to hold from
`64->127`. Ignore the fact that I called it a `HashMap`, I didn't know what to
call it exactly. Maybe `Bitmap` would have made more sense.

But now we just implement the `set` and `in` methods to set and check
respectively.

### Bitwise Operations

To add on to the previous concept, since all operations for inserting, checking,
converting, point calculation, are all bitwise operations, this makes the
program insanely fast. This is because computers are designed to bitwise options
and can crunch these within a literal single instruction.

### Cache Locality

I think this achieves both spatial and temporal cache locality because the
`line` variable is just overwritten so that doesn't change. The `HashMap` is a
global variable, so that does change. Memory access should be pretty linear
within the `line` and `HashMap`.

Checking the cache on my computer:

```
$ getconf -a | grep CACHE
LEVEL1_ICACHE_SIZE                 32768
LEVEL1_ICACHE_ASSOC
LEVEL1_ICACHE_LINESIZE             64
LEVEL1_DCACHE_SIZE                 32768
LEVEL1_DCACHE_ASSOC                8
LEVEL1_DCACHE_LINESIZE             64
LEVEL2_CACHE_SIZE                  524288
LEVEL2_CACHE_ASSOC                 8
LEVEL2_CACHE_LINESIZE              64
LEVEL3_CACHE_SIZE                  4194304
LEVEL3_CACHE_ASSOC                 16
LEVEL3_CACHE_LINESIZE              64
LEVEL4_CACHE_SIZE                  0
LEVEL4_CACHE_ASSOC
LEVEL4_CACHE_LINESIZE
```

Level 1 cache is `32768` and I'm pretty confident that the footprint of the
program should fit well within this. The line size of the level 1 cache is `64`
bits, which ensures that each element of `HashMap` gets its own cache line.

I think at this point, the program mainly spends its time just reading the file.
