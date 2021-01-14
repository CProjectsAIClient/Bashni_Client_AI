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

short** getPossibleMovesForPiece(int i, int j, char my_brett [9][9][13], int is_jump);
void calculateDame(short** possible_move, short* current_move, int zeile, int spalte, char my_brett[9][9][13]);
void calculateJump(short** possible_moves, short* current_move, char my_brett[9][9][13], int zeile, int spalte, short addZeile, short addSpalte);
void calculateMove(short** possible_moves, short* current_move, int zeile, int spalte, short addZeile, short addSpalte);
void printAllMoves(short*** all_moves);
void printMoves(short**possible_moves);
void getMove(char my_brett[9][9][13]);


char colour, colourEnemy;
//char array[9][9][13];

void initialize_random_ki(struct game* game_struct){
    printf("farbe zuordnen\n");
    if (game_struct->player_number == 0){
        colour = 'w';
        colourEnemy = 'b';
        //spaeter auch mit toupper checken...
    } else {
        colour = 'b';
        colourEnemy = 'w';
    }
    printf("farbe zugeordnet: %c, Enemy: %c\n", colour, colourEnemy);
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
    //short saveMoves [12][18][27];
    short*** saveMoves = calloc(12, sizeof(short));

    printf("color: %c\n", colour);
    int i,j,k,l=0;
    for(i = 1;i<=8;i++){
        for(j=1;j<=8;j++){
            if(my_brett[i][j][0] == colour || my_brett[i][j][0] == toupper(colour)){
                printf("in der schleife von getMoves...\n");
                saveMoves[l++] = getPossibleMovesForPiece(i,j, my_brett, FALSE);
                //printf("saveMoves[%i] : %i\n", l, saveMoves[l-1]);
                printf("\n");
            }
        }
    }
    //printAllMoves(saveMoves);

    printf("getMove zu ende.\n");

    free(saveMoves);
}


//berechnet mgl Zuege fuer einen Stein
short** getPossibleMovesForPiece(int zeile, int spalte, char my_brett [9][9][13], int is_jump){
    short dir = (colour == 'w') ? 1 : -1;
    short ** possible_moves = calloc(18, sizeof(short));
    short* current_move = malloc(sizeof(short));
    *current_move = 0;
    printf("colour in getpossiblemoves %c\n", colour);

    //pruefe auf dame
    if (my_brett[zeile][spalte][0] == toupper(colour)){
        calculateDame(possible_moves,current_move,zeile,spalte,my_brett);
    }
    //normaler Stein
    else {
        int war_hier = FALSE;
        int dirStein1 = FALSE, dirStein2 = FALSE;

        /*
         * Jump Abfragen
         */
        //springe falls schwarz nach oben rechts
        if ((my_brett[zeile + dir][spalte + dir][0] == colourEnemy || my_brett[zeile + dir][spalte + dir][0] == toupper(colourEnemy))
            && (zeile + 2*dir) <9 && (zeile + 2*dir) >0 && (spalte + 2*dir) <9 && (spalte + 2*dir) >0){//<= oder <  ??
            printf("in erstem jump");
            printf("my_brett[%i][%i] =%c",zeile + dir,spalte + dir,my_brett[zeile + dir][spalte + dir][0] );
            //return possible_moves;

            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            if(my_brett[zeile + 2*dir][spalte + 2*dir][0] == '-'){
                war_hier = TRUE;

                //hier jumpfkt
                calculateJump(possible_moves, current_move, my_brett, zeile, spalte, dir, dir);
            }

            dirStein1 = TRUE;
        }
        //springe falls schwarz nach oben links
        if ((my_brett[zeile + dir][spalte - dir][0] == colourEnemy || my_brett[zeile + dir][spalte - dir][0] == toupper(colourEnemy))
            && (zeile + 2*dir) <9 && (zeile + 2*dir) >0 && (spalte - 2*dir) <9 && (spalte - 2*dir) >0){
            printf("in zweitem jump");
            //return possible_moves;


            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            if(my_brett[zeile + 2*dir][spalte - 2*dir][0] == '-'){
                war_hier = TRUE;

                //hier jumpfkt
                calculateJump(possible_moves, current_move, my_brett, zeile, spalte, dir, -dir);
            }

            dirStein2 = TRUE;
        }
        //springe falls schwarz nach unten rechts
        if ((my_brett[zeile - dir][spalte + dir][0] == colourEnemy || my_brett[zeile - dir][spalte + dir][0] == toupper(colourEnemy))
            && (zeile - 2*dir) <9 && (zeile - 2*dir) >0 && (spalte + 2*dir) <9 && (spalte + 2*dir) >0){
            printf("in dritten jump");
            //return possible_moves;

            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            if(my_brett[zeile - 2*dir][spalte + 2*dir][0] == '-'){
                war_hier = TRUE;
                
                //hier jumpfkt
                calculateJump(possible_moves, current_move, my_brett, zeile, spalte, -dir, dir);
            }
        }
        //springe falls schwarz nach unten links
        if ((my_brett[zeile - dir][spalte - dir][0] == colourEnemy || my_brett[zeile - dir][spalte - dir][0] == toupper(colourEnemy))
            && (zeile - 2*dir) <9 && (zeile - 2*dir) >0 && (spalte - 2*dir) <9 && (spalte - 2*dir) >0){
            printf("in vierten jump");
            //return possible_moves;

            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            if(my_brett[zeile - 2*dir][spalte - 2*dir][0] == '-'){
                war_hier = TRUE;
                
                //hier jumpfkt
                calculateJump(possible_moves, current_move, my_brett, zeile, spalte, -dir, -dir);
            }
        }//movefkt

        //printf("Erfolgreich durch alle ifs\n");
        /*
         * Move Abfragen
         */
        if (war_hier == FALSE) {


            if (dirStein1 == FALSE){
                //Move falls oben rechts frei
                if (my_brett[zeile + dir][spalte + dir][0] == '-' && (zeile + dir) <=8 && (zeile + dir) >0 && (spalte + dir) <=8 && (spalte + dir) >0){
                     printf("zeile+dir:%i , spalte-dir:%i\n", zeile+dir, spalte-dir);
                    calculateMove(possible_moves, current_move, zeile, spalte, dir, dir);
                    printf("nach calculate move1: zeile+dir:%i , spalte-dir:%i\n", zeile+dir, spalte-dir);
                    //printf("Move1: %i, %i\n",possible_moves[3], move[4]);
                }
            }

            if (dirStein2 == FALSE){
                //Move falls oben links frei
                if (my_brett[zeile + dir][spalte - dir][0] == '-' && (zeile + dir) <=8 && (zeile + dir) >0 && (spalte - dir) <=8 && (spalte - dir) >0){
                    printf("in move2: zeile+dir:%i , spalte-dir:%i\n", zeile+dir, spalte-dir);
                    calculateMove(possible_moves, current_move, zeile, spalte, dir, -dir);
                    printf("nach calculate move2: zeile+dir:%i , spalte-dir:%i\n", zeile+dir, spalte-dir);
                }
            }
        }


    }

   // printMoves(possible_moves);
    printf("Returning possible moves for piece (%i,%i)", zeile, spalte);
    free(current_move);
    return possible_moves;
}

void calculateDame(short** possible_move, short *current_move, int zeile, int spalte, char my_brett[9][9][13]) {

}

void calculateMove(short** possible_moves, short *current_move, int zeile, int spalte, short addZeile, short addSpalte) {
    short* move = calloc(5, sizeof(char));
    possible_moves[(*current_move)++] = move;

    if ((zeile + addZeile == 8 && colour == 'w') || (zeile + addZeile == 1 && colour == 'b')){
        move[0] = 3;//ist Dame geworden
    } else {
        move[0] = 0;
    }

    move[1] = zeile;
    move[2] = spalte;

    move[3] = zeile + addZeile;
    move[4] = spalte + addSpalte;
}

void calculateJump(short** possible_moves, short *current_move, char my_brett[9][9][13], int zeile, int spalte, short addZeile, short addSpalte) {
    char new_brett [9][9][13];
    //kopie des bretts
    // strcpy(new_brett, my_brett);
    memcpy(&my_brett, &new_brett, sizeof(char)*9*9*13);

    //stein den wir ueberspringen loeschen 
    new_brett[zeile + addZeile][spalte + addSpalte][0] = '-';
    //eigenen Stein neu platzieren
    new_brett[zeile + addZeile + addZeile][spalte + addSpalte + addSpalte][0] = my_brett[zeile][spalte][0];
    //alte Position des eigenen Steins löschen
    new_brett[zeile][spalte][0] = '-';
    
    
    short* move = calloc(27, sizeof(short));
    possible_moves[(*current_move)++] = move;

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
            short *next_move = next_possible_moves[i++];

            //Gehe durch alle Teilzüge von next_move
            while (next_move[j] > 0)
            {
                //Speichere Teilzug in move
                move[k] = next_move[j];
                k++; 
                j++;
            }

            //Erstelle einen neuen Zug
            move = calloc(27, sizeof(short));
            possible_moves[(*current_move)++] = move;

            //setze die default Werte für den neuen Zug
            move[0] = -1; // jump
            move[1] = zeile; //alte zeile
            move[2] = spalte; //alte spalte

            move[3] = zeile + addZeile + addZeile; //neue zeile
            move[4] = spalte + addSpalte + addSpalte; //neue spalte
        }

    free(next_possible_moves);
}

void printAllMoves(short*** all_moves) {
    printf("All Moves: [\n");
    
    for (int i = 0; i < 12; i++) {
        printMoves(all_moves[i]);
    }

    printf("]\n");
    free(all_moves);
}

void printMoves(short** possible_moves) {
    printf("Possible Moves: [\n");

    for (int i = 0; i < 18; i++) {
        printf("  [");

        short *move = possible_moves[i];
        //Gehe durch alle Teilzüge von next_move
        int j = 0;
        while (move[j] > 0)
        {
            if (j != 0) {
                printf(", ");
            }

            printf("%i", move[j]);
        }

        printf("]\n");
    }

    printf("]\n");
}