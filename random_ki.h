#ifndef random_ki
#define random_ki

#define TRUE 1
#define FALSE 0

#define MOVE_RATING -3
#define JUMP_RATING -2
#define GET_QUEEN_RATING -1
#define JUMP_QUEEN_RATING 3

#include "performConnection.h"

void initialize_random_ki(struct game* game_struct);

void getPossibleMovesForPiece(short** possible_moves, short i, short j, char my_brett[9][9][13], int is_jump, int jump_dir);

char* getMove(char my_brett[9][9][13]);

char* translateMove(short* moves);

char colour, colourEnemy;

#endif

//3r1s19fspsivp