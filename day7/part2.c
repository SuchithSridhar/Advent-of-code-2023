#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "hand-part2.h"
#include "include/ssvector.h"

int64_t str2int64(const char *str) {
    int64_t ret = 0;
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] <= '9' && str[i] >= '0') {
            ret = ret * 10 + (str[i] - '0');
        }
        i++;
    }
    return ret;
}

void print_hand(void *hand_ptr) {

    int hand_lookup[15] = {0};
    hand_lookup[TWO] = '2';
    hand_lookup[THREE] = '3';
    hand_lookup[FOUR] = '4';
    hand_lookup[FIVE] = '5';
    hand_lookup[SIX] = '6';
    hand_lookup[SEVEN] = '7';
    hand_lookup[EIGHT] = '8';
    hand_lookup[NINE] = '9';
    hand_lookup[TEN] = 'T';
    hand_lookup[JOKER] = 'J';
    hand_lookup[QUEEN] = 'Q';
    hand_lookup[KING] = 'K';
    hand_lookup[ACE] = 'A';

    char* class_lookup[8];
    class_lookup[HIGHCARD] = "HIGHCARD";
    class_lookup[ONE_PAIR] = "ONE_PAIR";
    class_lookup[TWO_PAIR] = "TWO_PAIR";
    class_lookup[THREE_OAK] = "THREE_OAK";
    class_lookup[FULL_HOUSE] = "FULL_HOUSE";
    class_lookup[FOUR_OAK] = "FOUR_OAK";
    class_lookup[FIVE_OAK] = "FIVE_OAK";
    class_lookup[UNCLASSIFED] = "UNCLASSIFIED";

    hand_t *hand = (hand_t *) hand_ptr;
    printf("HAND(%c, %c, %c, %c, %c, %s, %ld)",
           hand_lookup[hand->cards[0]],
           hand_lookup[hand->cards[1]],
           hand_lookup[hand->cards[2]],
           hand_lookup[hand->cards[3]],
           hand_lookup[hand->cards[4]],
           class_lookup[hand->hand_class],
           hand->bid);
}

void print_hand_newline(void *hand) {
    print_hand(hand);
    printf("\n");
}


void parse_hand_bid(char *line, hand_t *hand) {

    // Assuming ASCII characters
    card_t lookup[128] = {0};
    lookup['J'] = JOKER;
    lookup['2'] = TWO;
    lookup['3'] = THREE;
    lookup['4'] = FOUR;
    lookup['5'] = FIVE;
    lookup['6'] = SIX;
    lookup['7'] = SEVEN;
    lookup['8'] = EIGHT;
    lookup['9'] = NINE;
    lookup['T'] = TEN;
    lookup['Q'] = QUEEN;
    lookup['K'] = KING;
    lookup['A'] = ACE;

    for (size_t i = 0; i < HAND_SIZE; i++) {
        hand->cards[i] = lookup[line[i]];
    }

    hand->bid = str2int64(&line[HAND_SIZE + 1]);
}

class_t classify_hand(hand_t *hand) {
    // Array to count occurrences of each card, 2-14 (Ace)
    printf(">> Classifying ");
    print_hand_newline(hand);

    int counts[15] = {0};

    int max_1 = 0;
    int max_2 = 0;

    // Count the occurrences of each card
    for (int i = 0; i < HAND_SIZE; i++) {
        counts[hand->cards[i]]++;
    }

    for (int i = TWO; i <= ACE; i++) {
        if (max_1 < counts[i]) {
            max_2 = max_1;
            max_1 = counts[i];
        } else if (max_2 < counts[i]) {
            max_2 = counts[i];
        }
    }

    printf(" --->> calculated maxes: %d, %d\n", max_1, max_2);

    if (max_1 + counts[JOKER] >= 5) {
        hand->hand_class = FIVE_OAK;
        return FIVE_OAK;
    } else if (max_1 + counts[JOKER] == 4) {
        hand->hand_class = FOUR_OAK;
        return FOUR_OAK;
    } else if (max_1 + counts[JOKER] == 3 && max_2 == 2) {
        hand->hand_class = FULL_HOUSE;
        return FULL_HOUSE;
    } else if (max_1 + counts[JOKER] == 3) {
        hand->hand_class = THREE_OAK;
        return THREE_OAK;
    } else if (max_1 == 2 && max_2 == 2) {
        hand->hand_class = TWO_PAIR;
        return TWO_PAIR;
    } else if (max_1 + counts[JOKER] == 2) {
        hand->hand_class = ONE_PAIR;
        return ONE_PAIR;
    } else {
        hand->hand_class = HIGHCARD;
        return HIGHCARD;
    }
}

int compare_hands(const void *h1, const void *h2) {
    hand_t *hand1 = (hand_t *)h1;
    hand_t *hand2 = (hand_t *)h2;

    class_t c1 = classify_hand(hand1);
    class_t c2 = classify_hand(hand2);

    printf("Comparing hands: \n");
    printf("    First: ");
    print_hand_newline(hand1);
    printf("    Second: ");
    print_hand_newline(hand2);

    if (c1 < c2) {
        printf("    decided hand1 < hand2 from class\n");
        return -1;
    }
    if (c1 > c2) {
        printf("    decided hand1 > hand2 from class\n");
        return 1;
    }

    for (size_t i = 0; i < HAND_SIZE; i++) {
        if (hand1->cards[i] < hand2->cards[i]) {
            printf("    decided hand1 < hand2 from card %ld\n", i);
            return -1;
        }
        if (hand1->cards[i] > hand2->cards[i]) {
            printf("    decided hand1 > hand2 from card %ld\n", i);
            return 1;
        }
    }

    return 0;
}

int64_t get_winnings(ss_Vector *hands) {
    hand_t *array = hands->array;
    qsort(array, hands->size, hands->memb_size, compare_hands);
    int64_t total = 0;
    int64_t value = 0;
    for (size_t i = 0; i < hands->size; i++) {
        value = array[i].bid * (i + 1);
        printf("Index %ld: ", i + 1);
        print_hand(&array[i]);
        printf(" -> evaluated: %ld\n", value);
        total += value;
    }
    return total;
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

    hand_t hand;
    hand.hand_class = UNCLASSIFED;
    int64_t winnings = 0;

    ss_Vector *hands = ssv_init(sizeof(hand_t), 1024);
    printf("Created hands vector\n");

    char *line = NULL;
    size_t len = 0;
    ssize_t characters_read;

    while ((characters_read = getline(&line, &len, f)) != -1) {
        parse_hand_bid(line, &hand);
        printf("Parsed ");
        print_hand_newline(&hand);

        ssv_push(hands, &hand);
    }
    printf("\n");

    if (line)
        free(line);

    fclose(f);

    winnings = get_winnings(hands);

    printf("\n\n");
    printf("Total Winnings: %ld\n", winnings);

    return EXIT_SUCCESS;
}
