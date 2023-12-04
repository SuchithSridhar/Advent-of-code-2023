#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_NUM 100

typedef struct {
    int64_t a;
    int64_t b;
} HashMap;

HashMap HASHMAP; 

// https://stackoverflow.com/a/9631424
unsigned char str2int(const char* str, int len) {
    int i;
    unsigned char ret = 0;
    for(i = 0; i < len; ++i) {
        if (str[i] == ' ') continue;
        ret = ret * 10 + (str[i] - '0');
    }
    return ret;
}

static void hashmap_set(unsigned char num) {
    int64_t x = 1;
    if (num < 64) {
        HASHMAP.a = HASHMAP.a | (x << num);
    } else {
        HASHMAP.b = HASHMAP.b | (x << (num-64));
    }
}

static int64_t hashmap_in(unsigned char num) {
    int64_t x = 1;
    if (num < 64) {
        return HASHMAP.a & (x << num);
    } else {
        return HASHMAP.b & (x << (num-64));
    }
}

/**
 * Count the wining numbers in a given line.
 * Assumptions made:
 *     - the numbers are arranged in perfect columns
 *     - the numbers are all positive integers less than 100.
 */
int count_wining_numbers(char *line, int skip) {
    char *cur = line;

    // Skip first $skip character which are "Card  xxx: "
    cur += skip; 

    // Initialize hashmap
    HASHMAP.a = 0;
    HASHMAP.b = 0;
    unsigned char num;

    while (*cur != '|') {
        num = str2int(cur, 2);
        hashmap_set(num);
        cur += 3;
    }

    cur += 2;  // skip "| " at the end of winning

    int count = 0;

    while (*cur != '\n' && *cur != '\0') {
        num = str2int(cur, 2);
        if (hashmap_in(num)) {
            count++;
        }

        cur += 3;
    }

    printf("Count: %d\n", count);
    if (count == 0) return 0;
    else return 1 << (count - 1);
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Invalid call to program.\n");
        fprintf(stderr, "./%s <input-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Unable to open file: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    char * line = NULL;
    size_t len = 0;
    ssize_t characters_read;

    int sum = 0;
    int skip = 6;  // "Card x" <- at least 6 characters
    
    // For the first line, we calculate $skip and then use
    // the same $skip for all following lines.

    characters_read = getline(&line, &len, f);
    if (characters_read != -1) {
        while (line[skip] != ':') {
            skip++;
        }
        skip += 2;

        sum += count_wining_numbers(line, skip);
        printf("Points: %d\n\n", sum);
    }

    int points;
    while ((characters_read = getline(&line, &len, f)) != -1) {
        points = count_wining_numbers(line, skip);
        printf("Points: %d\n\n", points);
        sum += points;
    }

    printf("%d\n", sum);

    if (line)
        free(line);

    fclose(f);

    return EXIT_SUCCESS;
}
