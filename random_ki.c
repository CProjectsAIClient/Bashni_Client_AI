#include "thinker.h"
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

void getPossibleMovesForPiece(int i, int j);
void getMove();

char colour, colourEnemy;
//char array[9][9][13];

void initialize_random__ki(game game_struct){
    if (game_struct.player_number == 0){
        colour = 'w';
        colourEnemy = 'b';
        //spaeter auch mit toupper checken...
    } else {
        colour = 'b';
        colourEnemy = 'w';
    }
}


//zusammenfassung aller mgl moves und eintscheidung fuer einen
void getMove(char my_brett[9][9][13]){
    //strcpy(array, my_brett);
    /*
    [1,2,1,5,6]
    [5,9,0,7,8,9,10]
    ...
    */
    moves[]={34,35,65};
    moves_initial[]={23, 24, 54};
    moves[i], moves_inital[i];


    int saveMoves [12][7];
    int i,j,k;
    for(i = 1;i<=8;i++){
        for(j=1;j<=8;j++){
            if(my_brett[i][j][0] == colour || my_brett[i][j][0] == toUpper(colour)){
                getPossibleMovesForPiece(i,j);
            }
        }
    }
}


//berechnet mgl Zuege fuer einen Stein
void getPossibleMovesForPiece(int spalte, int zeile, char array [][]){
    

}
