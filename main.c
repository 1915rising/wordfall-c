/* Wordfall - a word scramble game in C.
 *
 * The program picks a word, shuffles its letters, and you unscramble it.
 * Three strikes per word, five words per game. Hints cost points.
 * Usage: ./wordfall [--seed N]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static const char *WORDS[] = {
    "lantern", "harbor", "meadow", "cinder", "velvet",
    "quartz", "willow", "saffron", "timber", "zephyr",
    "cobalt", "juniper", "marble", "orchard", "pepper",
    "saddle", "tundra", "walnut", "yonder", "zealot"
};
#define NWORDS (sizeof(WORDS) / sizeof(WORDS[0]))
#define ROUNDS 5
#define STRIKES 3

static unsigned long rng_state;

static unsigned rng_next(void) {
    /* xorshift32 */
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 17;
    rng_state ^= rng_state << 5;
    return (unsigned)(rng_state & 0x7fffffffUL);
}

static void scramble(const char *word, char *out) {
    size_t n = strlen(word);
    strcpy(out, word);
    /* Fisher-Yates, retry until the result differs from the original. */
    for (int attempt = 0; attempt < 20; attempt++) {
        for (size_t i = n - 1; i > 0; i--) {
            size_t j = rng_next() % (i + 1);
            char t = out[i]; out[i] = out[j]; out[j] = t;
        }
        if (strcmp(out, word) != 0) return;
    }
}

static void trim_lower(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r' || s[n - 1] == ' '))
        s[--n] = '\0';
    for (size_t i = 0; i < n; i++) s[i] = (char)tolower((unsigned char)s[i]);
}

int main(int argc, char **argv) {
    unsigned long seed = 20261125UL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc)
            seed = strtoul(argv[++i], NULL, 10);
    }
    rng_state = seed ? seed : 1;

    /* Pick ROUNDS distinct words. */
    int picked[ROUNDS];
    for (int r = 0; r < ROUNDS; r++) {
        int idx, dup;
        do {
            idx = (int)(rng_next() % NWORDS);
            dup = 0;
            for (int k = 0; k < r; k++) if (picked[k] == idx) dup = 1;
        } while (dup);
        picked[r] = idx;
    }

    printf("=== WORDFALL ===\n");
    printf("Unscramble %d words. %d strikes per word. Type 'hint' for a hint (-15 pts),\n", ROUNDS, STRIKES);
    printf("'skip' to give up on a word, 'quit' to stop.\n\n");

    int score = 0;
    char line[128], scrambled[64];

    for (int r = 0; r < ROUNDS; r++) {
        const char *word = WORDS[picked[r]];
        scramble(word, scrambled);
        printf("Word %d/%d: %s  (%zu letters)\n", r + 1, ROUNDS, scrambled, strlen(word));
        int strikes = 0, solved = 0, hint_used = 0;
        while (strikes < STRIKES) {
            printf("> ");
            fflush(stdout);
            if (!fgets(line, sizeof(line), stdin)) { printf("\n"); goto done; }
            trim_lower(line);
            if (strcmp(line, "quit") == 0) goto done;
            if (strcmp(line, "skip") == 0) {
                printf("Skipped. The word was '%s'.\n\n", word);
                break;
            }
            if (strcmp(line, "hint") == 0) {
                printf("Hint: starts with '%c', ends with '%c'.\n", word[0], word[strlen(word) - 1]);
                hint_used = 1;
                continue;
            }
            if (strcmp(line, word) == 0) {
                int pts = 100 - 25 * strikes - (hint_used ? 15 : 0);
                if (pts < 10) pts = 10;
                score += pts;
                printf("Correct! +%d pts (score %d)\n\n", pts, score);
                solved = 1;
                break;
            }
            strikes++;
            printf("Not quite. Strikes: %d/%d\n", strikes, STRIKES);
        }
        if (!solved && strikes >= STRIKES)
            printf("Out of strikes. The word was '%s'.\n\n", word);
    }

done:
    printf("=== GAME OVER ===\nFinal score: %d\n", score);
    if (score >= 400) printf("Rating: Wordmaster\n");
    else if (score >= 250) printf("Rating: Wordsmith\n");
    else if (score >= 100) printf("Rating: Apprentice\n");
    else printf("Rating: Novice\n");
    return 0;
}
