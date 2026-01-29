#ifndef SCORE_H
#define SCORE_H

#include <stdint.h>

#define LOGIN_MAX 32

void score_init(const char *path);                 // wczytaj bazę z pliku (opcjonalnie)
uint32_t score_get_best(const char *login);        // pobierz best score
void score_update_best(const char *login, uint32_t score); // zapisz jeśli lepszy
uint32_t score_calc(int word_len, int wrong_guesses);      // policz score za grę
void score_print_all(int fd);

#endif
