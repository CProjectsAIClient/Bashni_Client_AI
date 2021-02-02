#include "ki.h"
#include "random_ki.h"
#include "performConnection.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>

#define MIN_MAX_DEPTH 3
//0c3fu3fyyfs1f

int minMax(char brett[9][9][13], int depth, int maxPlayer);
void getLastPosition(short* next_move, short* childPos);
double evaluate_position();
short finalMove [27];

char* getBestMove(char my_brett[9][9][13]) {
    printf("getBestMove aufgerufen\n");
    int i;
    i= minMax(my_brett, MIN_MAX_DEPTH, 1);
    printf("nach minMax eval: %d\n",i);
    printf("finalMove: %hn\n", finalMove);
    char* best_move = translateMove(finalMove);
    printf("move: %s\n", best_move);
    return best_move;
}



int minMax(char my_brett[9][9][13], int depth, int maximizingPlayer){
    double evaluation = 0;
    double maxEval, minEval;
    //printf("depth: %i\n", depth);
    if (depth == 0){
        //printf("in der if von depth\n");
        evaluation = evaluate_position(my_brett);
        return evaluation;
    }

    if (maximizingPlayer){
        //printf("minMax: maximizingPlayer, going through pieces\n");
        maxEval = - INFINITY;

        for (int i = 1; i <= 8; i++) {
            //printf("in erster for: i = %d\n" ,i);
            for (int j = 1; j <= 8; j++) {
                //printf("in zweiter for: j = %d\n" ,j);
                if (my_brett[i][j][0] == colour || my_brett[i][j][0] == toupper(colour)) {
                    //printf(" (%d, %d)", i, j);

                    short** possible_moves = calloc(18, sizeof(short*));
                    for (int k = 0; k < 18; k++) {
                        possible_moves[k] = calloc(27, sizeof(short));
                    }
                    getPossibleMovesForPiece(possible_moves, i, j, my_brett, colour, false, 0);

                    int l = 0;
                    short *next_move = possible_moves[l];

                    //Überprüfen ob erster Zug von next_possible_moves ein Jump ist (-1)
                    while ((next_move != NULL) && (next_move[0] != -200) && (l<18)) {
                        
                        short* childPos = calloc(2, sizeof(short));
                        getLastPosition(next_move, childPos);

                        int eval = minMax(my_brett, depth-1, 0);
                        maxEval = maxEval<eval ? eval : maxEval;
                        if(eval == maxEval){
                            for(int p = 0;p<27;p++){
                                finalMove[p] = next_move[p];
                            }
                        }

                        next_move = possible_moves[++l];
                        free(childPos);
                    }

                    for(int k = 0; k < 18; k++) {
                        free(possible_moves[k]);
                    }
                    free(possible_moves);
                    //printf("\n");
                }
            }
        }

        if (maxEval == -INFINITY) {
            return evaluate_position(my_brett);
        } else {
            return maxEval;
        }
    } else {
        //printf("minMax: minimizingPlayer, going through pieces\n");
        minEval = INFINITY;

        for (int i = 1; i <= 8; i++) {
            for (int j = 1; j <= 8; j++) {
                if (my_brett[i][j][0] == colourEnemy || my_brett[i][j][0] == toupper(colourEnemy)) {
                    //printf(" (%d, %d)", i, j);

                    short** possible_moves = calloc(18, sizeof(short*));
                    for (int k = 0; k < 18; k++) {
                        possible_moves[k] = calloc(27, sizeof(short));
                    }
                    getPossibleMovesForPiece(possible_moves, i, j, my_brett, colourEnemy, false, 0);

                    int l = 0;
                    short *next_move = possible_moves[l];

                    while ((next_move != NULL) && (next_move[0] != -200) && (i<18)) {
                        short* childPos = calloc(2, sizeof(short));
                        getLastPosition(next_move, childPos);

                        int eval = minMax(my_brett, depth-1, 1);
                        minEval = eval<minEval ? eval : minEval;

                        next_move = possible_moves[++l];
                        free(childPos);
                    }

                    for(int k = 0; k < 18; k++) {
                        free(possible_moves[k]);
                    }
                    free(possible_moves);
                    //printf("\n");
                }
            }
        }

        if (maxEval == INFINITY) {
            return evaluate_position(my_brett);
        } else {
            return minEval;
        }
    }
}

double evaluate_position(char my_brett[9][9][13]){
    int me=0, meD=0, he=0, heD=0;
    for(int i=1;i<=8;i++){
        for(int j=1; j<=8;j++){
            if(my_brett[i][j][0] == colour){me++;}
            if(my_brett[i][j][0] == toupper(colour)){meD++;}
            if(my_brett[i][j][0] == colourEnemy){he++;}
            if(my_brett[i][j][0] == toupper(colourEnemy)){heD++;}
        }
    }
    return 0.6*(me - he) + 0.4*(meD - heD);
}

void getLastPosition(short* next_move, short* childPos) {
    int i = 1;

    do {
        i += 2;
    } while ((i<27) && (next_move[i] > 0) && (next_move[i+1] > 0));
    
    childPos[0] = next_move[i];
    childPos[1] = next_move[i+1];
}
