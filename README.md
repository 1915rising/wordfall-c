# Wordfall (C)

A word scramble game for the terminal, written in C (C11, no dependencies
beyond the standard library).

The program picks a word, shuffles its letters, and you unscramble it.
Three strikes per word, five words per game. Asking for a hint costs
points; skipping reveals the word and moves on.

## Build

```sh
gcc -std=c11 -O2 -Wall -Wextra -o wordfall main.c
```

## Play

```sh
./wordfall              # random game
./wordfall --seed 42    # reproducible game (fixed word order and scrambles)
```

Commands while playing: type your guess, or `hint`, `skip`, `quit`.

Scoring: 100 points per word, minus 25 per strike used and 15 if you
took a hint. Finish all five words to earn a rating, from Novice up to
Wordmaster.

## Tested envelope

- Built clean with `gcc -std=c11 -O2 -Wall -Wextra` (GCC 13) with no warnings.
- Verified end to end with scripted stdin playthroughs: a full 5-word win
  (score 500, rating Wordmaster) and a strike-out/skip path, both reaching
  the game-over screen, using `--seed 42`.
