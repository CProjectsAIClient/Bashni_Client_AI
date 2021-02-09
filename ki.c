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

//0hdy96hg8ukzm

#include "ki.h"
#include "random_ki.h"
#include "performConnection.h"
#include "thinker.h"



//2nl0hf6wkemk0

double minMax(char brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE], int depth, double alpha, double beta, int maxPlayer);
void getLastPosition(short* next_move, short* childPos);
double evaluate_position();
void simulateMove(char new_brett1 [FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE], short* move);
short finalMove[27];

//2nl0hf6wkemk0
//06egts1t9gfsa

char* getBestMove(char my_brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE]) {
    printf("getBestMove aufgerufen\n");
    double i;
    i = minMax(my_brett, MIN_MAX_DEPTH, -INFINITY, INFINITY, 1);
    printf("nach minMax eval: %f\n",i);
    printf("finalMove: %hn\n", finalMove);
    char* best_move = translateMove(finalMove);
    printf("move: %s\n", best_move);
    return best_move;
}


double minMax(char my_brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE], int depth, double alpha, double beta, int maximizingPlayer){
    printf("\n %d", depth);

    double evaluation = 0;
    double maxEval, minEval;
    //printf("depth: %i\n", depth);
    if (depth == 0){
        //printf("in der if von depth\n");
        evaluation = evaluate_position(my_brett);
        printf(" e: %f", evaluation);
        return evaluation;
    }

    if (maximizingPlayer){
        //printf("minMax: maximizingPlayer, going through pieces\n");
        maxEval = - INFINITY;
        int is_jump = false;
        for (int i = 1; i <= 8; i++) {
            //printf("in erster for: i = %d\n" ,i);
            for (int j = 1; j <= 8; j++) {
                //printf("in zweiter for: j = %d\n" ,j);
                if (my_brett[i][j][0] == colour || my_brett[i][j][0] == toupper(colour)) {
                    printf(" (%d, %d)", i, j);

                    short** possible_moves = calloc(18, sizeof(short*));
                    for (int k = 0; k < 18; k++) {
                        possible_moves[k] = calloc(27, sizeof(short));
                    }
                    
                    is_jump = is_jump || getPossibleMovesForPiece(possible_moves, i, j, my_brett, colour, false, 0);
                    

                    int l = 0;
                    short *next_move = possible_moves[l];

                    char new_brett1[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE];
                    //kopie des bretts
                    for (int m = 0; m < 9; m++) {
                        for (int n = 0; n < 9; n++) {
                            for (int o = 0; o < MAX_TOWER_SIZE; o++) {
                                new_brett1[m][n][o] = my_brett[m][n][o]; 
                            }
                        }
                    }

                    //Überprüfen ob erster Zug von next_possible_moves ein Jump ist (-1)
                    
                    while ((next_move != NULL) && (next_move[0] != -200) && (next_move[0] != 0) && (l<17)) {
                        // simulateMove(new_brett1, next_move);
                        // printf("\nNach SimMove:\n");
                        //printfield(new_brett1);

                        if (is_jump && (next_move[0] == MOVE_RATING || next_move[0] == GET_QUEEN_FROM_MOVE_RATING)) {
                            continue;
                        }
                        double eval = minMax(new_brett1, depth-1, alpha, beta, 0);
                        
                        //kopie des bretts
                        for (int m = 0; m < 9; m++) {
                            for (int n = 0; n < 9; n++) {
                                for (int o = 0; o < MAX_TOWER_SIZE; o++) {
                                    new_brett1[m][n][o] = my_brett[m][n][o]; 
                                }
                            }
                        }

                        maxEval = maxEval<eval ? eval : maxEval;
                        // alpha = alpha > eval ? alpha : eval;
                        // if (beta <= alpha) {
                        //     break;
                        // }


                        if(eval == maxEval && depth == MIN_MAX_DEPTH){
                            for(int p = 0;p<27;p++){
                                finalMove[p] = next_move[p];
                            }
                        }

                        next_move = possible_moves[++l];
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
            printf("maxEval == -INFINITY\n\n");
            return evaluate_position(my_brett);
        } else {
            return maxEval;
        }
    } else {
        //printf("minMax: minimizingPlayer, going through pieces\n");
        minEval = INFINITY;
        int is_jump = 0;
        for (int i = 1; i <= 8; i++) {
            for (int j = 1; j <= 8; j++) {
                if (my_brett[i][j][0] == colourEnemy || my_brett[i][j][0] == toupper(colourEnemy)) {
                    //printf(" (%d, %d)", i, j);

                    short** possible_moves = calloc(18, sizeof(short*));
                    for (int k = 0; k < 18; k++) {
                        possible_moves[k] = calloc(27, sizeof(short));
                    }
                   
                    is_jump = is_jump || getPossibleMovesForPiece(possible_moves, i, j, my_brett, colourEnemy, false, false);

                    int l = 0;
                    short *next_move = possible_moves[l];
                    //printfield(my_brett);
                    char new_brett1[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE];
                    //kopie des bretts
                    for (int m = 0; m < 9; m++) {
                        for (int n = 0; n < 9; n++) {
                            for (int o = 0; o < MAX_TOWER_SIZE; o++) {
                                new_brett1[m][n][o] = my_brett[m][n][o]; 
                            }
                        }
                    }
                    
                    

                    while ((next_move != NULL) && (next_move[0] != -200) && (next_move[0] != 0) && (l<17)) {
                        // printf("\nNach SimMove:\n");
                        // simulateMove(new_brett1, next_move);
                        //printfield(new_brett1);

                        if (is_jump && (next_move[0] == MOVE_RATING || next_move[0] == GET_QUEEN_FROM_MOVE_RATING)) {
                            continue;
                        }
                        double eval = minMax(new_brett1, depth-1, alpha, beta, 1);
                        
                        //kopie des bretts
                        for (int m = 0; m < 9; m++) {
                            for (int n = 0; n < 9; n++) {
                                for (int o = 0; o < MAX_TOWER_SIZE; o++) {
                                    new_brett1[m][n][o] = my_brett[m][n][o]; 
                                }
                            }
                        }
                        
                        minEval = eval<minEval ? eval : minEval;
                        // beta = eval < beta ? eval : beta;
                        // if (beta <= alpha) {
                        //     break;
                        // }

                        next_move = possible_moves[++l];
                    }

                    for(int k = 0; k < 18; k++) {
                        free(possible_moves[k]);
                    }
                    free(possible_moves);
                    //printf("\n");
                }
            }
        }

        if (minEval == INFINITY) {
            printf("minEval == INFINITY\n\n");
            return evaluate_position(my_brett);
        } else {
            return minEval;
        }
    }
}

double evaluate_position(char my_brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE]){
    int me=0, meD=0, he=0, heD=0;
    //printf("colour: %c with ", colour);
    for(int i=1;i<=8;i++){
        for(int j=1; j<=8;j++){
            if(my_brett[i][j][0] == colour){me++;}
            if(my_brett[i][j][0] == toupper(colour)){meD++;}
            if(my_brett[i][j][0] == colourEnemy){he++;}
            if(my_brett[i][j][0] == toupper(colourEnemy)){heD++;}
        }
    }

    double a = 0.6*(me - he) + 0.4*(meD - heD);
    printf("bewertung  %f\n", a);
    return a;
}

void getLastPosition(short* next_move, short* childPos) {
    int i = 1;

    do {
        i += 2;
    } while ((i<27) && (next_move[i] > 0) && (next_move[i+1] > 0));
    
    childPos[0] = next_move[i];
    childPos[1] = next_move[i+1];
}
 

void simulateMove(char new_brett1 [FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE], short* move){
    //turm an der pos von der aus der jump gemacht wird "springer"
    char current_turm[MAX_TOWER_SIZE]; 
    //Art des moves (bewertung), Anfangszeile, Anfangsspalte
    short type = move[0], first_i = move[1], first_j = move[2];
    int towersize = 1;
    //momentane position in move
    int l = 3;

    //zu ueberspringende zeile und spalte
    int zeile;
    int spalte;

    //turm an Anfangspos einspeichern
    for (int i = 0; i < MAX_TOWER_SIZE; i++){
        current_turm[i] = new_brett1[first_i][first_j][i];
        if(new_brett1[first_i][first_j][i] != '-'){
            towersize += 1;
        }
    }
    
    new_brett1[first_i][first_j][0] = '-';

    
    while((move[l] != -200) && (move != NULL) && (move[l] != 0) && l<27){
        // printf("move[%i]: %i\n\n", l, move[l]);
        //printMove(move);

        int isJump = (abs(first_i - move[l]) > 1) || (abs(first_j - move[l+1]) > 1);
        if (isJump) {
            //zu ueberspringende positionen ermitteln
            // printf("position Ziel: [%i][%i]\n", move[l],move[l+1]);
            // printf("Abstand Position: %i\n", move[l]-move[l-2]);
            // printf("Abstand Position: %i\n", move[l+1]-move[l-1]);
            // printf("Abstand Position: %f\n", (1 / (double) abs(move[l]-move[l-2])));
            // printf("Abstand Position: %f\n", ((move[l] - move[l-2]) * (1 / (double) abs(move[l]-move[l-2]))));
            // printf("Abstand Position: %i\n", abs(-10));
            zeile = move[l] - ((move[l] - move[l-2]) * (1 / (double) abs(move[l] - move[l-2])));
            spalte = move[l+1] - ((move[l+1] - move[l-1]) * ((1 / (double) abs(move[l+1] - move[l-1]))));
            // printf("zeile:%i, spalte:%i\n", zeile, spalte);
            if ( (towersize+1) < MAX_TOWER_SIZE){
                current_turm[towersize+1] = new_brett1[zeile][spalte][0];
            }
            //ueberpringen: neuen wert an der uebersprungenen stelle speichern
            for(int i = 0; i < MAX_TOWER_SIZE; i++){
                //printf("new_brett1[%i][%i][%i]: %c\n", spalte, zeile, i, new_brett1[zeile][spalte][i]);
                new_brett1[zeile][spalte][i] = new_brett1[zeile][spalte][i+1];
                
                //printf("neue version von new_brett1[zeile][spalte][%i]: %c\n", i, new_brett1[zeile][spalte][i]);
            }
            new_brett1[zeile][spalte][MAX_TOWER_SIZE-2]='-';
        }

        //unser Turm auf die neue Position setzen
        for (int i = 0; i < MAX_TOWER_SIZE-1; i++){
            new_brett1[move[l]][move[l+1]][i] = current_turm[i];
        }
        // printfield(new_brett1);
        l += 2;
    }
    
    // printf("\nIn SimMove:\n");
    // printfield(new_brett1);
}

//1v9artrkybbyf
