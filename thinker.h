#ifndef thinker
#define thinker

char my_brett[9][9][13];

void startThinker(void * shmdata1, int pipe);

void printfield(char print[9][9][13]);

void signal_handler(int signal_key);

void reinitialize_brett_with_null();

void save_brett_in_matrix(char color, int column, int row);

#endif