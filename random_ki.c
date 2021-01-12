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
#define TRUE 1
#define FALSE 0

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

    //[[0, C, 4, D, 5, E, 3], [0, C, 4, d, 5, ], []]
    short saveMoves [12][18][27];
    int i,j,k;
    for(i = 1;i<=8;i++){
        for(j=1;j<=8;j++){
            if(my_brett[i][j][0] == colour || my_brett[i][j][0] == toUpper(colour)){
                getPossibleMovesForPiece(i,j, my_brett, FALSE);
            }
        }
    }
}


//berechnet mgl Zuege fuer einen Stein
short* getPossibleMovesForPiece(int spalte, int zeile, char my_brett [9][9][13], int is_jump){
    int dir = (colour == 'w') ? 1 : -1;

    //pruefe auf dame
    if ( (my_brett[zeile][spalte][0]) == toUpper(colour)){

    }
    //normaler Stein
    else {
        bool war_hier = FALSE;
        int dirStein1 = FALSE, dirStein2 = FALSE, dirStein3 = FALSE, dirStein4 = FALSE;
        //springe falls schwarz nach oben rechts
        if (my_brett[zeile + dir][spalte + dir][0] == colourEnemy || my_brett[zeile + dir][spalte + dir][0] == toUpper(colourEnemy)
            && (zeile + dir) <=8 && (zeile + dir) >=0 && (spalte + dir) <=8 && (spalte + dir) >=0){//<= oder <  ??; für jumps < i think YES! :) wie die Russen sagen: senks
            //hier jumpfkt
            char new_brett [9][9][13];
            strcpy(new_brett, my_brett);
            new_brett[zeile + dir][spalte + dir][0] == '-';

            war_hier = TRUE;
            getPossibleMovesForPiece(zeile + dir + dir, spalte + dir + dir, new_brett, TRUE);
        }
        if (my_brett[zeile + dir][spalte - dir][0] == colourEnemy || my_brett[zeile + dir][spalte - dir][0] == toUpper(colourEnemy)){
            //hier jumpfk
            dirStein1 = TRUE;
        }//hier movefkt
        if (my_brett[zeile - dir][spalte + dir][0] == colourEnemy || my_brett[zeile - dir][spalte + dir][0] == toUpper(colourEnemy)){
            //hier jumpfk
            war_hier = TRUE;
        }
        if (my_brett[zeile - dir][spalte - dir][0] == colourEnemy || my_brett[zeile - dir][spalte - dir][0] == toUpper(colourEnemy)){
            //hier jumpfk
            war_hier = TRUE;
        }//movefkt


        if (war_hier == FALSE)
            if (my_brett[zeile + dir][spalte + dir][0] == '-' && (zeile + dir) <=8 && (zeile + dir) >=0 && (spalte + dir) <=8 && (spalte + dir) >=0){
                //hier movefkt
            }
            
    }


    int ** piece = calloc(18, sizeof(int));
    for(int i = 0; i<4; i++){
        int * piece = calloc(27, sizeof(int));
    }

}
