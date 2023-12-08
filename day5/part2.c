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

#define INIT_SIZE 1024

typedef struct {
    long start;
    long count;
    long times_parsed;
} Range;

void print_map(void *map) {
    Range *item = (Range *)map;
    printf("(start: %ld, count: %ld)\n", item->start, item->count);
}

/**
 * Parse out the seed values from the first line
 * of input. The read seed values are inserted into
 * the map.
 * Example: "seeds: a_start a_count b_start b_count ..."
 * @param line the line to be parsed.
 * @param map the map to insert the parsed values.
 */
void parse_seed_line(char *line, ss_Vector *map) {
    Range map_item;
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

        map_item.start = start_value;
        map_item.count = range;
        ssv_push(map, &map_item);
    }
}

/**
 * If the provided value is in range then the
 * corresponding value is returned.
 * If the value is not present, then -1 is returned.
 */
long map_value_to_range(long value, long src_start, long dst_start,
                        long range) {
    if (src_start <= value && (src_start + range) > value) {
        return dst_start + (value - src_start);
    }
    return -1;
}

void process_range(Range *map_item, long src_start, long dst_start, long range,
                   ss_Vector *map) {

    printf("    ==== Process range: \n");
    printf("    ");
    print_map(map_item);
    printf("    %ld src; %ld dst; %ld rng\n", src_start, dst_start, range);

    if (range == 0) return;

    long map_start = map_item->start;
    long map_count = map_item->count;
    long map_end = map_start + map_count - 1;

    long src_end = src_start + range - 1;
    long dst_end = dst_start + range - 1;

    // matching ranges => just update it to be this range
    if (map_start == src_start && map_count == range) {
        printf("        matching ranges\n");
        map_item->start = dst_start;
        map_item->times_parsed++;
        printf("        Updated previous part: ");
        print_map(map_item);
        return;
    }

    // map completely contained in source range
    if (map_start >= src_start && map_end <= src_end) {
        printf("        map contained in source ranges\n");
        map_item->start =
            map_value_to_range(map_item->start, src_start, dst_start, range);
        map_item->times_parsed++;
        printf("        Updated previous part: ");
        print_map(map_item);
        return;
    }

    // map completely contains sources range
    if (src_start >= map_start && src_end <= map_end) {
        printf("        source contained in map ranges\n");
        // divides map into 3 different ranges
        Range first;
        first.start = map_start;
        first.count = src_start - map_start;
        first.times_parsed = map_item->times_parsed;

        Range last;
        last.start = src_end + 1;
        last.count = map_end - src_end;
        last.times_parsed = map_item->times_parsed;

        map_item->start = dst_start;
        map_item->count = range;
        map_item->times_parsed++;

        if (first.count != 0) ssv_push(map, &first);
        if (last.count != 0) ssv_push(map, &last);

        printf("        Updated previous part: ");
        print_map(map_item);
        printf("        Create first part: ");
        print_map(&first);
        printf("        Create last part: ");
        print_map(&last);

        return;
    }

    // map range above source range 
    if (map_start <= src_start && src_start <= map_end && map_end <= src_end) {
        printf("        map above source ranges\n");

        Range part;
        part.start = map_start;
        part.count = src_start - map_start;
        part.times_parsed = map_item->times_parsed;

        map_item->start = dst_start;
        map_item->count = map_item->count - part.count;
        map_item->times_parsed++;

        printf("        Created new part: ");
        print_map(&part);
        printf("        Updated previous part: ");
        print_map(map_item);

        ssv_push(map, &part);
        return;
    }

    // map below source
    if (map_start >= src_start && map_start <= src_end && map_end >= src_end) {
        printf("        map below source ranges\n");

        Range part;
        part.times_parsed = map_item->times_parsed;

        printf("PROBLEMS??\n");
        map_item->start = map_value_to_range(map_start, src_start, dst_start, range);
        map_item->count = src_end - map_start + 1;
        map_item->times_parsed++;

        part.start = src_end + 1;
        part.count = map_end - src_end;

        printf("        Created new part: ");
        print_map(&part);
        printf("        Updated previous part: ");
        print_map(map_item);

        ssv_push(map, &part);
        return;
    }
}

void parse_map_line(char *line, ss_Vector *map, long maps_parsed) {
    long dst_start, src_start, range;

    char *p = strtok(line, " ");
    dst_start = atol(p);

    p = strtok(NULL, " ");
    src_start = atol(p);

    p = strtok(NULL, " ");
    range = atol(p);

    Range *array;
    long map_value;
    for (size_t i = 0; i < map->size; i++) {
        // This is required because sometimes process_range calls
        // ssv_push which may update map->array to be something else.
        array = map->array;
        if (maps_parsed == array[i].times_parsed) {
            continue;
        }
        process_range(&array[i], src_start, dst_start, range, map);
    }
}

long find_min_dest_in_map(ss_Vector *map) {
    Range *array = (Range *)map->array;
    long min = LONG_MAX;
    for (size_t i = 0; i < map->size; i++) {
        if (array[i].start < min)
            min = array[i].start;
    }
    return min;
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

    ss_Vector *map = ssv_init(sizeof(Range), INIT_SIZE);

    bool first_line = true;
    char *line = NULL;
    size_t len = 0;
    ssize_t characters_read;

    // First line contains the seeds
    characters_read = getline(&line, &len, f);
    parse_seed_line(line, map);

    printf("Seed ranges:\n");
    ssv_print(map, print_map);

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
        Range *array = map->array;
        for (size_t i = 0; i < map->size; i++) {
            array[i].times_parsed = maps_parsed;
        }

        if (characters_read == -1) {
            // end of maps
            break;
        }
        printf("Map after parse round %ld\n", maps_parsed);
        ssv_print(map, print_map);
        maps_parsed++;
    }

    printf("\n\nFinal Ranges:\n");
    ssv_print(map, print_map);

    long min_value = find_min_dest_in_map(map);
    printf("%ld\n", min_value);

    if (line)
        free(line);

    ssv_destroy(map);
    fclose(f);

    return EXIT_SUCCESS;
}
