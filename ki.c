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

#define MIN_MAX_DEPTH 10

void minMax(short zeile, short spalte, int depth, char colour);
void getLastPosition(short* next_move, short* childPos);
double evaluate_position();


char* getBestMove(char my_brett[9][9][13]) {
    //[[[0, C, 4, D, 5, E, 3], [0, C, 4, d, 5]], []]
    short*** saveMoves = calloc(12, sizeof(short*));

    for(int i = 0; i < 12; i++){
        saveMoves[i] = calloc(18, sizeof(short*));

        for(int j = 0; j < 18; j++){
            saveMoves[i][j] = calloc(27, sizeof(short));
        }
    }
    
    int counter;
    short i, j, k;

    for (i = 1; i <= 8; i++) {
        for (j = 1; j <= 8; j++) {
            if (my_brett[i][j][0] == colour || my_brett[i][j][0] == toupper(colour)) {                
                if (saveMoves[counter][0][0] != -200) {
                    printf("Stein [%d]: ", counter);
                    minMax(i, j, MIN_MAX_DEPTH, 1);
                    //printMoves(saveMoves[counter]);

                    //Auf jump überprüfen
                    counter++;
                }
            }
        }
    }

    //char* best_move = translateMove(saveMoves[x][y]);
    //printf("Translated move: %s\n", best_move);

    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 18; j++) {
            free(saveMoves[i][j]);
        }

        free(saveMoves[i]);
    }
    free(saveMoves);

    //spaeter noch richtigen best move einspeichern
     char* best_move = "w";

    return best_move;
}

short* finalMove;

int minMax(short zeile, short spalte, char my_brett[9][9][13], int depth, int maximizingPlayer){
    double evaluation = 0;
    double maxEval, minEval;
                    

    if (depth == 0){
        // Bewertung
        game over in pos;
        evaluation = evaluate_position(my_brett);
        return evaluation;
    }

    short** possible_moves = calloc(18, sizeof(short*));
    for (int i = 0; i < 18; i++) {
        possible_moves[i] = calloc(27, sizeof(short));
    }
    getPossibleMovesForPiece(possible_moves, zeile, spalte, my_brett, false, 0);

    if (possible_moves[0][0] != -200) {
        //...

        if (maximizingPlayer){
            maxEval = - INFINITY;

            int i = 0;
            short *next_move = possible_moves[i];

            //Überprüfen ob erster Zug von next_possible_moves ein Jump ist (-1)
            while ((next_move != NULL) && (next_move[0] != -200) && (i<18)) {
                short* childPos = calloc(2, sizeof(short));
                getLastPosition(next_move, childPos);

                int eval = minMax(childPos[0], childPos[1], my_brett, depth-1, 0);
                maxEval = fmax(maxEval,eval);

                next_move = possible_moves[++i];
                free(childPos);
            }

            return maxEval;

        } else {
            minEval = INFINITY;

            int i = 0;
            short *next_move = possible_moves[i];

            while ((next_move != NULL) && (next_move[0] != -200) && (i<18)) {
                short* childPos = calloc(2, sizeof(short));
                getLastPosition(next_move, childPos);

                int eval = minMax(childPos[0], childPos[1], my_brett, depth-1, 1);
                minEval = fmin(minEval,eval);

                next_move = possible_moves[++i];
                free(childPos);
            }

            return minEval;
        }
    } else {
        return 0;//?
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