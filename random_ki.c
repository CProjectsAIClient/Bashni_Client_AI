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

short* getPossibleMovesForPiece(int i, int j, char my_brett [9][9][13], int is_jump);
void calculateJump(short** possible_moves, int current_move, char my_brett[9][9][13], int zeile, int spalte, short addZeile, short addSpalte);

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
    short dir = (colour == 'w') ? 1 : -1;
    short ** possible_moves = calloc(18, sizeof(short));
    short current_move = 0;
    // for(int i = 0; i<4; i++){
    //     possible_moves[i] = calloc(27, sizeof(int));
    // }

    //pruefe auf dame
    if ( (my_brett[zeile][spalte][0]) == toUpper(colour)){

    }
    //normaler Stein
    else {
        int war_hier = FALSE;
        int dirStein1 = FALSE, dirStein2 = FALSE;

        /*
         * Jump Abfragen
         */
        //springe falls schwarz nach oben rechts
        if (my_brett[zeile + dir][spalte + dir][0] == colourEnemy || my_brett[zeile + dir][spalte + dir][0] == toUpper(colourEnemy)
            && (zeile + 2*dir) <9 && (zeile + 2*dir) >0 && (spalte + 2*dir) <9 && (spalte + 2*dir) >0){//<= oder <  ??
            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            if(my_brett[zeile + 2*dir][spalte + 2*dir][0] == '-'){
                war_hier = TRUE;
                //hier jumpfkt
                
                calculateJump(possible_moves, current_move, my_brett, zeile, spalte, dir, dir);

                
                
            }

            dirStein1 = TRUE;
            possible_moves[0][0]=0;
        }
        //springe falls schwarz nach oben links
        if (my_brett[zeile + dir][spalte - dir][0] == colourEnemy || my_brett[zeile + dir][spalte - dir][0] == toUpper(colourEnemy)
            && (zeile + 2*dir) <9 && (zeile + 2*dir) >0 && (spalte - 2*dir) <9 && (spalte - 2*dir) >0){
            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            if(my_brett[zeile + 2*dir][spalte - 2*dir][0] == '-'){
                war_hier = TRUE;
                //hier jumpfkt
                char new_brett [9][9][13];
                strcpy(new_brett, my_brett);
                new_brett[zeile + dir][spalte - dir][0] == '-';


                getPossibleMovesForPiece(zeile + 2*dir, spalte - 2*dir, new_brett, TRUE);
            }

            dirStein2 = TRUE;
        }
        //springe falls schwarz nach unten rechts
        if (my_brett[zeile - dir][spalte + dir][0] == colourEnemy || my_brett[zeile - dir][spalte + dir][0] == toUpper(colourEnemy)
            && (zeile - 2*dir) <9 && (zeile - 2*dir) >0 && (spalte + 2*dir) <9 && (spalte + 2*dir) >0){
            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            if(my_brett[zeile - 2*dir][spalte + 2*dir][0] == '-'){
                war_hier = TRUE;
                //hier jumpfkt
                char new_brett [9][9][13];
                strcpy(new_brett, my_brett);
                new_brett[zeile - dir][spalte + dir][0] == '-';

                getPossibleMovesForPiece(zeile -  2* dir, spalte + 2*dir, new_brett, TRUE);
            }
        }
        //springe falls schwarz nach unten links
        if (my_brett[zeile - dir][spalte - dir][0] == colourEnemy || my_brett[zeile - dir][spalte - dir][0] == toUpper(colourEnemy)
            && (zeile - 2*dir) <9 && (zeile - 2*dir) >0 && (spalte - 2*dir) <9 && (spalte - 2*dir) >0){
            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            if(my_brett[zeile - 2*dir][spalte - 2*dir][0] == '-'){
                war_hier = TRUE;
                //hier jumpfkt
                char new_brett [9][9][13];
                strcpy(new_brett, my_brett);
                new_brett[zeile - dir][spalte - dir][0] == '-';


                getPossibleMovesForPiece(zeile -  2* dir, spalte + 2* dir, new_brett, TRUE);
            }
        }//movefkt

        /*
         * Move Abfragen
         */
        if (war_hier == FALSE) {


            if (dirStein1 == FALSE){
                if (my_brett[zeile + dir][spalte + dir][0] == '-' && (zeile + dir) <=8 && (zeile + dir) >=0 && (spalte + dir) <=8 && (spalte + dir) >=0){
                    //hier movefkt
                }
            }

            if (dirStein2 == FALSE){
                if (my_brett[zeile + dir][spalte - dir][0] == '-' && (zeile + dir) <=8 && (zeile + dir) >=0 && (spalte - dir) <=8 && (spalte - dir) >=0){
                    //hier movefkt
                }
            }
        }


    }

}

void calculateJump(short** possible_moves, int current_move, char my_brett[9][9][13], int zeile, int spalte, short addZeile, short addSpalte) {
    char new_brett [9][9][13];
    //kopie des bretts
    strcpy(new_brett, my_brett);
    //stein den wir ueberspringen loeschen
    new_brett[zeile + addZeile][spalte + addSpalte][0] = '-';
    
    short* move = possible_moves[current_move];
    current_move++;
    move = calloc(27, sizeof(short));

    move[0] = -1; // jump
    move[1] = zeile; //alte zeile
    move[2] = spalte; //alte spalte

    move[3] = zeile + addZeile + addZeile; //neue zeile
    move[4] = spalte + addSpalte + addSpalte; //neue spalte

    //[[-1, neue_zeile, neue_spalte, ...], [-1, neue_zeile, neue_spalte, ...], [-1, neue_zeile, neue_spalte, ...]
    short** next_possible_moves = getPossibleMovesForPiece(zeile + addZeile + addZeile, spalte + addSpalte + addSpalte, new_brett, TRUE);

    // -1,  5, 6,  7, 8                 // -1  7, 8,  8, 9          // -1  8, 9,  6, 5
    // -1,  5, 6,  7, 8,  8, 9,  6, 5   <- -1  7, 8,  8, 9,  6, 5   <-
    
    int i = 0;// 12 34 78 910 spielbrett // -1 12 34 78 ??// -1 34 78 910// -1 78 910
    short *next_move = next_possible_moves[i];

    //Überprüfen ob erster Zug von next_possible_moves ein Jump ist (-1)
    while (next_move[0] == -1) {
        int j = 3, k = 5;
        short *next_move = next_possible_moves[0];

        //Gehe durch alle Teilzüge von next_move
        while (next_move[j] > 0)
        {
            //Speichere Teilzug in move
            move[k] = next_move[j];
            k++; 
            j++;
        }

        //Setze next_move auf den nächsten Zug in next_possible_moves
        *next_move = next_possible_moves[++i];
        //Erstelle einen neuen Zug
        move = possible_moves[current_move];
        current_move++;

        //setze die default Werte für den neuen Zug
        move[0] = -1; // jump
        move[1] = zeile; //alte zeile
        move[2] = spalte; //alte spalte

        move[3] = zeile + addZeile + addZeile; //neue zeile
        move[4] = spalte + addSpalte + addSpalte; //neue spalte
    }
}