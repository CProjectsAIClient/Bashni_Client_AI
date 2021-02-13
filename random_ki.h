#ifndef random_ki
#define random_ki

#include "performConnection.h"


#define MIN_MAX_DEPTH 7
#define MAX_TOWER_SIZE 13
#define FIELD_SIZE 9

void initialize_random_ki(struct game* game_struct);

int getPossibleMovesForPiece(short** possible_moves, short i, short j, char my_brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE],char piece_colour, int is_jump, int jump_dir);

char* getMove(char my_brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE]);

char* translateMove(short* moves);

void printMoves(short** possible_moves);

void printMove(short* move);

char colour, colourEnemy;

#define MOVE_RATING -4
#define GET_QUEEN_FROM_MOVE_RATING -3
#define JUMP_RATING -2
#define GET_QUEEN_FROM_JUMP_RATING -1
#define JUMP_QUEEN_RATING 3

#endif

//3r1s19fspsivp