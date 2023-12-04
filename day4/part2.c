#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "include/ssvector.h"

#define MAX_NUM 100
#define INIT_SIZE 256

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

    return count;
}

void update_copies(ss_Vector *copies, int card_number, int count) {
    int64_t inc_count = 1;
    if (copies->size > card_number) {
        inc_count = *((int64_t *) ssv_get(copies, card_number)) + 1;
    } else {
        // size < card_number
        // create all previous elements with 0
        int64_t value = 0;
        for (size_t i=copies->size; i<card_number+1; i++) {
            ssv_push(copies, &value);
        }
    }

    int64_t *cur_val;
    size_t end = count + card_number + 1;
    size_t min = copies->size < end ? copies->size : end;
    for (size_t i = card_number+1; i < min; i++) {
        cur_val = ssv_get(copies, i);
        *cur_val += inc_count;
    }

    for (size_t i = copies->size; i < end; i++) {
        ssv_push(copies, &inc_count);
    }
}

int64_t sum_copies(ss_Vector *copies) {
    int64_t *array = copies->array;
    int64_t sum = 0; 
    for (size_t i = 0; i < copies->size; i++) {
        sum += array[i];
    }
    return sum;
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

    int card_number = 0;
    int count;
    int64_t value = 0;
    ss_Vector *copies = ssv_init(sizeof(int64_t), INIT_SIZE);

    // Always going to have exactly one instance of first scratch card.
    // => no copies are present
    ssv_push(copies, &value);

    characters_read = getline(&line, &len, f);
    if (characters_read != -1) {
        while (line[skip] != ':') {
            skip++;
        }
        skip += 2;

        count = count_wining_numbers(line, skip);

        update_copies(copies, card_number, count);

        card_number++;
    }

    while ((characters_read = getline(&line, &len, f)) != -1) {
        count = count_wining_numbers(line, skip);

        update_copies(copies, card_number, count);

        card_number++;
    }

    printf("%ld\n", card_number + sum_copies(copies));

    if (line)
        free(line);

    fclose(f);

    ssv_destroy(copies);

    return EXIT_SUCCESS;
}
