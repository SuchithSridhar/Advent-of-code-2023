#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/ssvector.h"

/*
 * x-to-y map:
 * a b c
 *
 * x: type
 * y: type
 *
 * a: destination range start
 * b: source range start
 * c: range length
 *
 * Example:
 * seed-to-soil map:
 * 50 98 2
 *
 * Maps 50 to 98 and 51 to 99
 *
 * Any source numbers that aren't
 * mapped correspond to the same
 * destination number.
 * Example: seed 10 maps to soil 10
 *
 * seed is the start and location is the end
 */

#define INIT_SIZE 8

typedef struct {
    long src;
    long dst;
    long times_parsed;
} MapItem;

/**
 * Parse out the seed values from the first line
 * of input. The read seed values are inserted into
 * the map.
 * Example: "seeds: a_start a_count b_start b_count ..."
 * @param line the line to be parsed.
 * @param map the map to insert the parsed values.
 */
void parse_seed_line(char *line, ss_Vector *map) {
    MapItem map_item;
    map_item.times_parsed = 0;

    // skip the "seeds: " part.
    char *p = strtok(line, " ");

    long start_value;
    long range;

    while (p != NULL) {
        p = strtok(NULL, " ");
        if (p == NULL)
            break;

        start_value = atol(p);

        p = strtok(NULL, " ");
        if (p == NULL)
            break;

        range = atol(p);

        for (size_t i = 0; i < range; i++) {
            map_item.src = start_value + i;
            map_item.dst = start_value + i;
            ssv_push(map, &map_item);
        }
    }
}

/**
 * If the provided value is in range then
 * the corresponding value is returned. If the
 * value is not present, then -1 is returned.
 */
long extract_from_map(long value, long dst_start, long src_start, long range) {

    if (src_start <= value && (src_start + range) > value) {
        return dst_start + (value - src_start);
    }
    return -1;
}

void parse_map_line(char *line, ss_Vector *map, long maps_parsed) {
    long dst_start, src_start, range;

    char *p = strtok(line, " ");
    dst_start = atol(p);

    p = strtok(NULL, " ");
    src_start = atol(p);

    p = strtok(NULL, " ");
    range = atol(p);

    MapItem *array = map->array;
    long map_value;
    for (size_t i = 0; i < map->size; i++) {
        if (maps_parsed == array[i].times_parsed) {
            continue;
        }

        map_value = extract_from_map(array[i].dst, dst_start, src_start, range);
        if (map_value != -1) {
            array[i].dst = map_value;
            array[i].times_parsed++;
        }
    }
}

long find_min_dest_in_map(ss_Vector *map) {
    MapItem *array = (MapItem *)map->array;
    long min = LONG_MAX;
    for (size_t i = 0; i < map->size; i++) {
        if (array[i].dst < min)
            min = array[i].dst;
    }
    return min;
}

void print_map(void *map) {
    MapItem *item = (MapItem *)map;
    printf("%ld -> %ld\n", item->src, item->dst);
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

    ss_Vector *map = ssv_init(sizeof(MapItem), INIT_SIZE);

    bool first_line = true;
    char *line = NULL;
    size_t len = 0;
    ssize_t characters_read;

    // First line contains the seeds
    characters_read = getline(&line, &len, f);
    parse_seed_line(line, map);

    // Empty second line
    characters_read = getline(&line, &len, f);

    long maps_parsed = 1;
    while (true) {
        // Skip the "x-to-y map:" line
        characters_read = getline(&line, &len, f);
        while ((characters_read = getline(&line, &len, f)) != -1) {
            if (characters_read < 3) {
                break;
            }
            parse_map_line(line, map, maps_parsed);
        }

        // update maps for non-updated items
        MapItem *array = map->array;
        for (size_t i = 0; i < map->size; i++) {
            array[i].times_parsed = maps_parsed;
        }

        if (characters_read == -1) {
            // end of maps
            break;
        }
        maps_parsed++;
    }

    long min_value = find_min_dest_in_map(map);
    printf("%ld\n", min_value);

    if (line)
        free(line);

    ssv_destroy(map);
    fclose(f);

    return EXIT_SUCCESS;
}
