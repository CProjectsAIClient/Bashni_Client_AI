#ifndef thinker
#define thinker

#include "random_ki.h"

char my_brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE];

void startThinker(void * shmdata1, int pipe);

void printfield(char print[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE]);

void signal_handler(int signal_key);

void reinitialize_brett_with_null();

void save_brett_in_matrix(char color, int column, int row);

#endif