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

void getPossibleMovesForPiece(short** possible_moves, short i, short j, char my_brett[9][9][13], int is_jump);
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
    short*** saveMoves = calloc(12, sizeof(short*));

    for(int i = 0; i < 12; i++){
        saveMoves[i] = calloc(18, sizeof(short*));

        for(int j = 0; j < 18; j++){
            saveMoves[i][j] = calloc(27, sizeof(short));
        }
    }
    
    int jumps[12] = {-100};
    int counter = 0, jmpcounter = 0;
    short i, j, k;

    printf("\n\n=== Mögliche Moves: ===\n");

    for (i = 1; i <= 8; i++) {
        for (j = 1; j <= 8; j++) {
            if (my_brett[i][j][0] == colour || my_brett[i][j][0] == toupper(colour)) {
                short move[12];
                getPossibleMovesForPiece(saveMoves[counter], i, j, my_brett, FALSE);
                
                if (saveMoves[counter][0][0] != -200) {
                    printf("Stein [%d]: ", counter);
                    printMoves(saveMoves[counter]);

                    //Auf jump überprüfen
                    if (saveMoves[counter][0][0] == JUMP_RATING) {
                        jumps[jmpcounter] = counter;
                        jmpcounter++;
                        printf("FOUND JUMP!\n");
                    }

                    counter++;
                }
            }
        }
    }

    printf("=======================\n\n");

    if (saveMoves[counter][0][0] == -200) {
        counter--;
    }

<<<<<<< HEAD
    int jumps[counter];
    int jump_counter = 0;
    printf("\n\n=== Mögliche Moves: ===\n");
    for (int m = 0; m < counter; m++){
        printf("Stein [%d]: ", m);
        printMoves(saveMoves[m]);

        //Auf jump überprüfen
        if (saveMoves[m][0][0] == JUMP_RATING) {
            jumps[jump_counter] = m;
            printf("└> FOUND JUMP!\n");
        }
    }
    printf("=======================\n\n");

    printf("Picking random move from saveMoves...\n");
=======
    printf("Picking random move from saveMoves (%d Jumps found)...\n", jmpcounter);
>>>>>>> 9b68c603e33aec2ffb600df39985aa3d5ee8f00d
    char* random_move;
    
    short x, isjump = FALSE;
    if (jmpcounter == 0) {
        x = rand() % counter;
    } else {
        short z = rand() % jmpcounter;
        x = jumps[z];
        isjump = TRUE;
    }
    
    int possible_moves_counter = 0;
    while (saveMoves[x][possible_moves_counter][0] != 0) {
        possible_moves_counter++;
    }

    printf("Picking random move from piece %d (possible moves: %d)...\n", x, possible_moves_counter);
    srand(time(NULL));
    short y = isjump ? 0 : rand() % possible_moves_counter;

    printf("HELLO MOVE: ");
    printMove(saveMoves[x][y]);

    random_move = translateMove(saveMoves[x][y]);
    printf("Translated move: %s\n", random_move);

    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 18; j++) {
            free(saveMoves[i][j]);
        }

        free(saveMoves[i]);
    }
    free(saveMoves);

    return random_move;
}

//berechnet mgl Zuege fuer einen Stein
void getPossibleMovesForPiece(short** possible_moves, short zeile, short spalte, char my_brett[9][9][13], int is_jump){
    short dir = (colour == 'w') ? 1 : -1, current_move = 0;
    possible_moves[0][0] = (short) -200;

    //pruefe auf dame
    if (my_brett[zeile][spalte][0] == toupper(colour)){
        calculateDame(possible_moves,&current_move,zeile,spalte,my_brett);
    }
    //normaler Stein
    else {
        int war_hier = FALSE, dirStein1 = FALSE, dirStein2 = FALSE;

        /*
         * Jump Abfragen
         */
        //springe falls schwarz nach oben rechts
        if ((zeile + dir + dir) <9 && (zeile + dir + dir) >0 && (spalte + dir + dir) <9 && (spalte + dir + dir) >0 
            && (my_brett[zeile + dir][spalte + dir][0] == colourEnemy || my_brett[zeile + dir][spalte + dir][0] == toupper(colourEnemy))){//<= oder <  ??
            printf("in erstem jump ");
            printf("my_brett[%i][%i] = %c\n", zeile + dir, spalte + dir, my_brett[zeile + dir][spalte + dir][0]);

            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            if(my_brett[zeile + dir + dir][spalte + dir + dir][0] == '-'){
                war_hier = TRUE;

                //hier jumpfkt
                calculateJump(possible_moves, &current_move, my_brett, zeile, spalte, dir, dir);
            }

            dirStein1 = TRUE;
            printf("dirStein1 = TRUE\n");
        }
        //springe falls schwarz nach oben links
        if ((zeile +dir +dir) <9 && (zeile + dir + dir) >0 && (spalte - dir -dir) <9 && (spalte - dir - dir) >0 
            && (my_brett[zeile + dir][spalte - dir][0] == colourEnemy || my_brett[zeile + dir][spalte - dir][0] == toupper(colourEnemy))){
            printf("in zweitem jump ");
            printf("my_brett[%i][%i] = %c\n", zeile + dir, spalte - dir, my_brett[zeile + dir][spalte - dir][0]);

            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            if(my_brett[zeile + dir + dir][spalte - dir - dir][0] == '-'){
                war_hier = TRUE;

                //hier jumpfkt
                calculateJump(possible_moves, &current_move, my_brett, zeile, spalte, dir, -dir);
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
            }
        }
        //springe falls schwarz nach unten links
        if ((zeile - 2*dir) <9 && (zeile - 2*dir) >0 && (spalte - 2*dir) <9 && (spalte - 2*dir) >0 
            && (my_brett[zeile - dir][spalte - dir][0] == colourEnemy || my_brett[zeile - dir][spalte - dir][0] == toupper(colourEnemy))){
            printf("in vierten jump ");
            printf("my_brett[%i][%i] = %c\n", zeile - dir, spalte - dir, my_brett[zeile - dir][spalte - dir][0]);

            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            if(my_brett[zeile - 2*dir][spalte - 2*dir][0] == '-'){
                war_hier = TRUE;

                //hier jumpfkt
                calculateJump(possible_moves, &current_move, my_brett, zeile, spalte, -dir, -dir);
            }
        }

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
}

void calculateDame(short** possible_move, short *current_move, short zeile, short spalte, char my_brett[9][9][13]) {
    printf("HALLO meine liebe Dame!\n\n");
}

void calculateMove(short** possible_moves, short *current_move, short zeile, short spalte, short addZeile, short addSpalte) {
    short* move = possible_moves[(*current_move)++];

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
    
    //kopie des bretts
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            for (int k = 0; k < 13; k++) {
               new_brett[i][j][k] = my_brett[i][j][k]; 
            }
        }
    }

    //stein den wir ueberspringen loeschen
    new_brett[zeile + addZeile][spalte + addSpalte][0] = '-';
    //eigenen Stein neu platzieren
    new_brett[zeile + addZeile + addZeile][spalte + addSpalte + addSpalte][0] = my_brett[zeile][spalte][0];
    //alte Position des eigenen Steins löschen
    new_brett[zeile][spalte][0] = '-';

    short* move = possible_moves[(*current_move)++];//calloc(27, sizeof(short));    
    move[0] = JUMP_RATING; // jump
    move[1] = zeile; //alte zeile
    move[2] = spalte; //alte spalte

    move[3] = zeile  + addZeile  + addZeile; //neue zeile
    move[4] = spalte + addSpalte + addSpalte; //neue spalte

    printf("recursive call of getPossibleMovesForPiece...\n");
    //[[-1, neue_zeile, neue_spalte, ...], [-1, neue_zeile, neue_spalte, ...], [-1, neue_zeile, neue_spalte, ...]
    short** next_possible_moves = calloc(18, sizeof(short*));
    for (int i = 0; i < 18; i++) {
        next_possible_moves[i] = calloc(27, sizeof(short));
    }
    getPossibleMovesForPiece(next_possible_moves, zeile + addZeile + addZeile, spalte + addSpalte + addSpalte, new_brett, TRUE);
    
    // -1,  5, 6,  7, 8                 // -1  7, 8,  8, 9          // -1  8, 9,  6, 5
    // -1,  5, 6,  7, 8,  8, 9,  6, 5   <- -1  7, 8,  8, 9,  6, 5   <-

    int i = 0;// 12 34 78 910 spielbrett // -1 12 34 78 ??// -1 34 78 910// -1 78 910
    short *next_move = next_possible_moves[i];

    //Überprüfen ob erster Zug von next_possible_moves ein Jump ist (-1)
    while ((next_move != NULL) && (next_move[0] != -200) && (next_move[0] == JUMP_RATING)) {
        int j = 3, k = 5;
        next_move = next_possible_moves[i++];

        //Gehe durch alle Teilzüge von next_move
        while (next_move[j] > 0)
        {
            //Speichere Teilzug in move
            move[k] = next_move[j];
            k++;
            j++;
        }

        //Erstelle einen neuen Zug
        move = possible_moves[(*current_move)++];

        //setze die default Werte für den neuen Zug
        move[0] = JUMP_RATING; // jump
        move[1] = zeile; //alte zeile
        move[2] = spalte; //alte spalte

        move[3] = zeile + addZeile + addZeile; //neue zeile
        move[4] = spalte + addSpalte + addSpalte; //neue spalte
    }

    //printMoves(possible_moves);
    for (int i = 0; i < 18; i++) {
        free(next_possible_moves[i]);
    }
    free(next_possible_moves);
}

void printAllMoves(short*** all_moves) {
    printf("All Moves: [\n");

    for (short i = 0; i < 12; i++) {
        printMoves(all_moves[i]);
    }

    printf("]\n");
    free(all_moves);
}

void printMoves(short** possible_moves) {
    printf("Possible Moves: [\n");

    for (short i = 0; i < 4; i++) {
        short* moves = possible_moves[i];
        
        if (moves != NULL && *moves != 0) {
            printMove(moves);
        }
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
    char* arr = malloc(40*sizeof(char));
    //arr = "PLAY ";
    strcpy(arr, "PLAY ");
    short i = 1, j = 5;
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

//1vrh39df3qaaq