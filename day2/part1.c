#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum { Red, Blue, Green } Color;

typedef struct {
    int red;
    int blue;
    int green;
} Group;

bool is_valid_hand(Group bag, Group hand) {
    return (
        bag.red >= hand.red &&
        bag.blue >= hand.blue &&
        bag.green >= hand.green
    );
}

/*
 * Identify the game ID from a given line.
 * This function modifies the provided line
 * and also the current pointer.
 */
int identify_game_id(char **cur) {
    /* "Game x: ...."
     * turns into
     * "Game x\0 ..."
     * and *cur now points to the character after \0
     */
    char *start_of_game_id = *cur;
    // Identify the game
    while (*start_of_game_id != ' ') start_of_game_id++;
    start_of_game_id++;

    char *end_of_game_id = start_of_game_id+1;
    while (*end_of_game_id != ':') end_of_game_id++;
    *end_of_game_id = '\0';

    int game_id = atoi(start_of_game_id);

    *cur = end_of_game_id + 1;
    return game_id;
}

int parse_num_till_space(char **cur) {

    char *start_of_num = *cur;
    char *end_of_num = start_of_num+1;
    while (*end_of_num != ' ') end_of_num++;
    *end_of_num = '\0';

    int game_id = atoi(start_of_num);

    *cur = end_of_num + 1;
    return game_id;
}

Color parse_color_till_comma_or_semicolon(char **cur) {
    Color color;
    if (**cur == 'r') {
        color = Red;
    } else if (**cur == 'b') {
        color = Blue;
    } else if (**cur == 'g') {
        color = Green;
    }

    while (**cur != ',' && **cur != ';' && **cur != '\0') (*cur)++;
    return color;
}

Group parse_game_till_semicolon(char **cur) {
    Group hand;
    hand.red = 0;
    hand.blue = 0;
    hand.green = 0;

    while (true) {
        int num = parse_num_till_space(cur);
        Color color = parse_color_till_comma_or_semicolon(cur);
        if (color == Red) {
            hand.red = num;
        } else if (color == Blue) {
            hand.blue = num;
        } else if (color == Green) {
            hand.green = num;
        }

        if (**cur == ';' || **cur == '\0') {
            break;
        } else {
            // move from , to next number
            *cur += 2;
        }
    } 

    return hand;
}

/**
 * If it's a valid game, returns the ID of the game,
 * otherwise returns 0.
 */
int is_valid_game(Group bag, char *line) {
    Group hand;

    char *cur = line;
    int game_id = identify_game_id(&cur);

    // now *cur points to the space after the :
    cur++;
    while (*cur != '\0') {
        hand = parse_game_till_semicolon(&cur);
        if (!is_valid_hand(bag, hand)) {
            return 0;
        }

        if (*cur == ';') {
            cur += 2;
        }
    } 
    return game_id;
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


    // Initial bag contents
    Group bag;
    bag.red = 12;
    bag.green = 13;
    bag.blue = 14;

    int sum = 0;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, f)) != -1) {
        if (line[read-1] == '\n') line[read-1] = '\0';
        sum += is_valid_game(bag, line);
    }

    printf("%d\n", sum);

    if (line)
        free(line);

    return EXIT_SUCCESS;
}
