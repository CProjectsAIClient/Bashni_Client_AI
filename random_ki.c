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

#define JUMP_RATING -1
#define MOVE_RATING -2
#define QUEEN_RATING 3

#define TRUE 1
#define FALSE 0

void getPossibleMovesForPiece(short** possible_moves, short i, short j, char my_brett [9][9][13], int is_jump);
void calculateDame(short** possible_move, short* current_move, short zeile, short spalte, char my_brett[9][9][13]);
void calculateJump(short** possible_moves, short* current_move, char my_brett[9][9][13], short zeile, short spalte, short addZeile, short addSpalte);
void calculateMove(short** possible_moves, short* current_move, short zeile, short spalte, short addZeile, short addSpalte);
void printAllMoves(short*** all_moves);
void printMoves(short** possible_moves);
void printMove(short* move);
char* translateMove(short* moves);


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
char* getMove(char my_brett[9][9][13]){
    //[[[0, C, 4, D, 5, E, 3], [0, C, 4, d, 5]], []]
    //short saveMoves [12][18][27]
    short*** saveMoves = calloc(12, sizeof(short**));
    for(int i = 0; i < 12; i++){
        saveMoves[i] = calloc(18, sizeof(short*));
        for(int j = 0; j < 18; j++){
            saveMoves[i][j] = calloc(27, sizeof(short));
            
        }
    }
    

    int counter=0;
    short i,j,k;
    for(i = 1; i <= 8; i++){
        for(j = 1; j <= 8; j++){
            if(my_brett[i][j][0] == colour || my_brett[i][j][0] == toupper(colour)){
                getPossibleMovesForPiece(saveMoves[counter], i,j, my_brett, FALSE);
                
                if (saveMoves[counter][0][0] != -200) {
                    counter++;
                }
            }
        }
    }

    if (saveMoves[counter][0][0] == -200) {
        counter--;
    }

    int jumps[counter];
    int jump_counter = 0;
    printf("\n\n=== Mögliche Moves: ===\n");
    for (int m = 0; m < counter; m++){
        printf("Stein [%d]: ", m);
        printMoves(saveMoves[m]);

        //Auf jump überprüfen
        if (saveMoves[m][0][0] == JUMP_RATING) {
            jumps[jump_counter] = m;
            printf("FOUND JUMP!\n");
        }
    }
    printf("=======================\n\n");

    printf("Picking random move from saveMoves...\n");
    char* random_move;

    short x = rand() % counter;

    printf("Found possible moves:\n");
    //printMoves(possible_moves);
    
    int possible_moves_counter = 0, l = 0;

    while ((l < counter) && (saveMoves[x][l] != NULL)) {
        possible_moves_counter++;
        l++;
    }

    printf("possible_moves_counter: %d", possible_moves_counter);
    srand(time(NULL));
    short y = rand() % possible_moves_counter;

    printf("Try possible_moves[%d]\n", y);
    //printf("possibleMoves +x: %hn", *(possible_moves + x));
    printf("HELLO MOVE!\n\n");
    printMove(saveMoves[x][y]);
    random_move = translateMove(saveMoves[x][y]);
    printf("this is the move: %s", random_move);
   
    for(int i = 0; i < 18; i++){
        for(int j = 0; j < 27; j++){
            free(saveMoves[i][j]);
        }
        free(saveMoves[i]);
    }
    free(saveMoves);
    //free(next_move);
    return random_move;
}

//berechnet mgl Zuege fuer einen Stein
void getPossibleMovesForPiece(short** possible_moves, short zeile, short spalte, char my_brett [9][9][13], int is_jump){
    spalte = (short) spalte;
    zeile = (short) zeile;
    short dir = (colour == 'w') ? 1 : -1;
    
    //possible_moves[0] = malloc(sizeof(short));
    possible_moves[0][0] = (short) -200;
    //possible_moves = NULL;
    short current_move = 0;
    //printf("colour in getpossiblemoves %c\n", colour);

    //pruefe auf dame
    if (my_brett[zeile][spalte][0] == toupper(colour)){
        calculateDame(possible_moves,&current_move,zeile,spalte,my_brett);
    }
    //normaler Stein
    else {
        int war_hier = FALSE;
        int dirStein1 = FALSE, dirStein2 = FALSE;

        /*
         * Jump Abfragen
         */
        //springe falls schwarz nach oben rechts
        if ((zeile + dir + dir) <9 && (zeile + dir + dir) >0 && (spalte + dir + dir) <9 && (spalte + dir + dir) >0 
            && (my_brett[zeile + dir][spalte + dir][0] == colourEnemy || my_brett[zeile + dir][spalte + dir][0] == toupper(colourEnemy))){//<= oder <  ??
            printf("in erstem jump ");
            printf("my_brett[%i][%i] = %c\n", zeile + dir, spalte + dir, my_brett[zeile + dir][spalte + dir][0]);
            //return possible_moves;

            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            if(my_brett[zeile + dir + dir][spalte + dir + dir][0] == '-'){
                war_hier = TRUE;

                //hier jumpfkt
                printf("wert von current_move vor calculateJump: %d\n", current_move);
                calculateJump(possible_moves, &current_move, my_brett, zeile, spalte, dir, dir);
                printf("nach calculate jmp\n");
            }

            dirStein1 = TRUE;
            printf("dirStein1 = TRUE\n");
        }
        //springe falls schwarz nach oben links
        if ((zeile +dir +dir) <9 && (zeile + dir + dir) >0 && (spalte - dir -dir) <9 && (spalte - dir - dir) >0 
            && (my_brett[zeile + dir][spalte - dir][0] == colourEnemy || my_brett[zeile + dir][spalte - dir][0] == toupper(colourEnemy))){
            printf("in zweitem jump ");
            printf("my_brett[%i][%i] = %c\n", zeile + dir, spalte - dir, my_brett[zeile + dir][spalte - dir][0]);
            //return possible_moves;


            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            if(my_brett[zeile + dir + dir][spalte - dir - dir][0] == '-'){
                war_hier = TRUE;

                //hier jumpfkt
                calculateJump(possible_moves, &current_move, my_brett, zeile, spalte, dir, -dir);
                printf("nach calculate jmp\n");
            }

            dirStein2 = TRUE;
        }
        //springe falls schwarz nach unten rechts
        if ((zeile - dir -dir) <9 && (zeile - dir - dir) >0 && (spalte + dir + dir) <9 && (spalte + dir + dir) >0 
            && (my_brett[zeile - dir][spalte + dir][0] == colourEnemy || my_brett[zeile - dir][spalte + dir][0] == toupper(colourEnemy))){
            printf("in dritten jump ");
            printf("my_brett[%i][%i] = %c\n", zeile - dir, spalte + dir, my_brett[zeile - dir][spalte + dir][0]);
            //return possible_moves;

            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            if(my_brett[zeile - 2*dir][spalte + 2*dir][0] == '-'){
                war_hier = TRUE;

                //hier jumpfkt
                calculateJump(possible_moves, &current_move, my_brett, zeile, spalte, -dir, dir);
                printf("nach calculate jmp\n");
            }
        }
        //springe falls schwarz nach unten links
        if ((zeile - 2*dir) <9 && (zeile - 2*dir) >0 && (spalte - 2*dir) <9 && (spalte - 2*dir) >0 
            && (my_brett[zeile - dir][spalte - dir][0] == colourEnemy || my_brett[zeile - dir][spalte - dir][0] == toupper(colourEnemy))){
            printf("in vierten jump ");
            printf("my_brett[%i][%i] = %c\n", zeile - dir, spalte - dir, my_brett[zeile - dir][spalte - dir][0]);
            //return possible_moves;

            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            if(my_brett[zeile - 2*dir][spalte - 2*dir][0] == '-'){
                war_hier = TRUE;

                //hier jumpfkt
                calculateJump(possible_moves, &current_move, my_brett, zeile, spalte, -dir, -dir);
                printf("nach calculate jmp\n");
                
            }
        }//movefkt

        //printf("Erfolgreich durch alle ifs\n");
        /*
         * Move Abfragen
         */
        if (war_hier == FALSE && is_jump == FALSE) {

            if (dirStein1 == FALSE){
                //Move falls oben rechts frei
                if ((zeile + dir) >0 && (spalte + dir) <=8 && (spalte + dir) >0 && (zeile + dir) <=8 && my_brett[zeile + dir][spalte + dir][0] == '-'){
                    calculateMove(possible_moves, &current_move, zeile, spalte, dir, dir);
                }
            }

            if (dirStein2 == FALSE){
                //Move falls oben links frei
                if ((zeile + dir) <=8 && (zeile + dir) >0 && (spalte - dir) <=8 && (spalte - dir) >0 && my_brett[zeile + dir][spalte - dir][0] == '-'){
                    calculateMove(possible_moves, &current_move, zeile, spalte, dir, -dir);
                }
            }

        }
    }

    //printf("printing (%d) moves... zeile 223\n", *current_move);
    //printMoves(possible_moves);
    printf("\nReturning possible moves for piece (%d,%d)\n", zeile, spalte);
    //return possible_moves;
}

void calculateDame(short** possible_move, short *current_move, short zeile, short spalte, char my_brett[9][9][13]) {

}

void calculateMove(short** possible_moves, short *current_move, short zeile, short spalte, short addZeile, short addSpalte) {
    short* move = possible_moves[(*current_move)++];//calloc(6, sizeof(short));
    //possible_moves[(*current_move)++] = move;

    if ((zeile + addZeile == 8 && colour == 'w') || (zeile + addZeile == 1 && colour == 'b')){
        move[0] = QUEEN_RATING;//ist Dame geworden
    } else {
        move[0] = MOVE_RATING;//move
    }

    move[1] = zeile;
    move[2] = spalte;

    move[3] = zeile + addZeile;
    move[4] = spalte + addSpalte;
}

void calculateJump(short** possible_moves, short *current_move, char my_brett[9][9][13], short zeile, short spalte, short addZeile, short addSpalte) {
    printf("in jump function zeile: %d, spalte: %d, addZeile: %d, addSpalte: %d\n", zeile, spalte, addZeile, addSpalte);
    char new_brett [9][9][13];
    printf("current_move am anfang von calculateJump: %d\n", *current_move);
    //kopie des bretts
    // strcpy(new_brett, my_brett);
    //memcpy(&my_brett, &new_brett, sizeof(char)*9*9*13);
    for(int i = 0;i<9;i++){
        for(int j = 0; j < 9; j++){
            for(int k = 0; k < 13; k++){
               new_brett[i][j][k] = my_brett[i][j][k]; 
            }
        }
    }
    

    //stein den wir ueberspringen loeschen
    new_brett[zeile + addZeile][spalte + addSpalte][0] = '-';
    //printf("new_brett[%i][%i][0] = '%c'\n",zeile + addZeile, spalte + addSpalte, new_brett[zeile + addZeile][spalte + addSpalte][0]);
    //eigenen Stein neu platzieren
    new_brett[zeile + addZeile + addZeile][spalte + addSpalte + addSpalte][0] = my_brett[zeile][spalte][0];
    //printf("new_brett[%i][%i][0] = '%c'\n",zeile + addZeile + addZeile, spalte + addSpalte + addSpalte, new_brett[zeile + addZeile + addZeile][spalte + addSpalte + addSpalte][0]);
    //alte Position des eigenen Steins löschen
    new_brett[zeile][spalte][0] = '-';
    //printf("new_brett[%i][%i][0] = '%c'\n", zeile, spalte, new_brett[zeile][spalte][0]);

    printf("Alloc memory for possible move...\n");
    short* move = possible_moves[(*current_move)++];//calloc(27, sizeof(short));
    //possible_moves[(*current_move)++] = move;
    printf("position zum einspeichern festlegen: %d\n", *current_move);
    

    printf("Setting default values...\n");
    move[0] = JUMP_RATING; // jump
    move[1] = zeile; //alte zeile
    move[2] = spalte; //alte spalte

    move[3] = zeile  + addZeile  + addZeile; //neue zeile
    move[4] = spalte + addSpalte + addSpalte; //neue spalte

    printf("recursive call of getPossibleMovesForPiece...\n");
    //[[-1, neue_zeile, neue_spalte, ...], [-1, neue_zeile, neue_spalte, ...], [-1, neue_zeile, neue_spalte, ...]
    //printMove(move);
    
    short** next_possible_moves = calloc(18, sizeof(short*));
    for (int i = 0; i < 18; i++) {
        next_possible_moves[i] = calloc(27, sizeof(short));
    }
    getPossibleMovesForPiece(next_possible_moves, zeile + addZeile + addZeile, spalte + addSpalte + addSpalte, new_brett, TRUE);
    
    printf("nach rekursionsaufruf zeile 301\n");
    // -1,  5, 6,  7, 8                 // -1  7, 8,  8, 9          // -1  8, 9,  6, 5
    // -1,  5, 6,  7, 8,  8, 9,  6, 5   <- -1  7, 8,  8, 9,  6, 5   <-

    int i = 0;// 12 34 78 910 spielbrett // -1 12 34 78 ??// -1 34 78 910// -1 78 910
    short *next_move = next_possible_moves[i];

    printf("injecting received possible moves from recursive call...\n");
    //Überprüfen ob erster Zug von next_possible_moves ein Jump ist (-1)
    while ((next_move != NULL) && (next_move[0] != -200) && (next_move[0] == JUMP_RATING)) {
        printf("next_move[0] = %d\n", next_move[0]);
        int j = 3, k = 5;
        next_move = next_possible_moves[i++];
        printf(" NextMove : %d\n", *next_move);

        //Gehe durch alle Teilzüge von next_move
        while (next_move[j] > 0)
        {
            //Speichere Teilzug in move
            move[k] = next_move[j];
            printf("next_move[%d]:%d\n", j, next_move[j]);
            k++;
            j++;
        }

        //Erstelle einen neuen Zug
        //move = calloc(27, sizeof(short));
        move = possible_moves[(*current_move)++];

        //setze die default Werte für den neuen Zug
        move[0] = JUMP_RATING; // jump
        move[1] = zeile; //alte zeile
        move[2] = spalte; //alte spalte

        move[3] = zeile + addZeile + addZeile; //neue zeile
        move[4] = spalte + addSpalte + addSpalte; //neue spalte
    }

    printf("returning calculateJump fkt...zeile 340\n");
    //printMoves(possible_moves);
    for (int i = 0; i < 18; i++) {
        free(next_possible_moves[i]);
    }
    free(next_possible_moves);
}   printf("Possible Moves: [\n");
                printMove(moves);
            }
            //printf("  nach if\n");
        //}
    }

    printf("]\n");
}

void printMove(short* move) {
    printf("  [");

    //Gehe durch alle Teilzüge von next_move
    short j = 0;
    while (move+j != NULL && move[j] != 0 && j < 18)
    {
        if (j != 0) {
            printf(", ");
        }


        printf("%d", move[j]);
        j++;
    }

    printf("]\n");
}

char* translateMove(short* moves) {
    printf("Starting translation...\n");
    char* arr = malloc(40*sizeof(char));
    //arr = "PLAY ";
    strcpy(arr, "PLAY ");
    short i = 1, j = 5;
    printf("Starting translation while...\n");
    while(moves[i] != 0){
        printf("translate while through (%d, %d)\n", moves[i], moves[i+1]);
        
        arr[j]   = 'A' - 1 + moves[i+1];
        arr[j+1] = '0'     + moves[i];
        arr[j+2] = ':';
        i += 2;
        j += 3;
    }
    j-=3;
    arr[j+2] = '\0';
    
    return arr;
}

//289fkuxzkgkq7