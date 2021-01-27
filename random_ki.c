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

#define MOVE_RATING -3
#define JUMP_RATING -2
#define GET_QUEEN_RATING -1
#define JUMP_QUEEN_RATING 3

#define TRUE 1
#define FALSE 0

void getPossibleMovesForPiece(short** possible_moves, short i, short j, char my_brett[9][9][13], int is_jump);
void calculateDame(short** possible_move, short* current_move, short zeile, short spalte, char my_brett[9][9][13]);
int calculateJump(short** possible_moves, short* current_move, char my_brett[9][9][13], short zeile, short spalte, short addZeile, short addSpalte);
void calculateMove(short** possible_moves, short* current_move, short zeile, short spalte, short addZeile, short addSpalte);
void calculateDameMove(short** possible_move, short* current_move, short zeile, short spalte, char my_brett[9][9][13], short addZeile, short addSpalte);
int calculateDameJump(short** possible_move, short* current_move, short zeile, short spalte, char my_brett[9][9][13], short addZeile, short addSpalte);
int jmpPossible(char my_brett [9][9][13], short zeile, short spalte, short addZeile, short addSpalte);
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
                    if (saveMoves[counter][0][0] == JUMP_RATING || saveMoves[counter][0][0] == JUMP_QUEEN_RATING) {
                        jumps[jmpcounter] = counter;
                        jmpcounter++;
                        printf("└> FOUND JUMP!\n");
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

    printf("Picking random move from saveMoves (%d Jumps found)...\n", jmpcounter);
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
        //Überprüfe auf Jump nach rechts oben
        is_jump = calculateDameJump(possible_moves, &current_move, zeile, spalte, my_brett, dir, dir) || is_jump;
        //Überprüfe auf Jump nach links oben
        is_jump = calculateDameJump(possible_moves, &current_move, zeile, spalte, my_brett, dir, -dir) || is_jump;
        //Überprüfe auf Jump nach rechts unten
        is_jump = calculateDameJump(possible_moves, &current_move, zeile, spalte, my_brett, -dir, dir) || is_jump;
        //Überprüfe auf Jump nach links unten
        is_jump = calculateDameJump(possible_moves, &current_move, zeile, spalte, my_brett, -dir, -dir) || is_jump;

        //calculateDame(possible_moves,&current_move,zeile,spalte,my_brett);
        //ueberpruefe ob es einen jump gab, wenn nicht gehe in calculateDameMove:
        if (!is_jump){
            //berechnet mgl Moves nach oben rechts
            calculateDameMove(possible_moves, &current_move, zeile, spalte, my_brett, dir, dir);
            //berechnet mgl Moves nach oben links
            calculateDameMove(possible_moves, &current_move, zeile, spalte, my_brett, dir, -dir);
            //berechnet mgl Moves nach unten rechts
            calculateDameMove(possible_moves, &current_move, zeile, spalte, my_brett, -dir, dir);
            //berechnet mgl Moves nach unten links
            calculateDameMove(possible_moves, &current_move, zeile, spalte, my_brett, -dir, -dir);
        }
    }
    //normaler Stein
    else {
        int didJump = FALSE, dirUpperRight = FALSE, dirUpperLeft = FALSE;

        /*
         * Jump Abfragen
         */
        //springe falls schwarz nach oben rechts
        if (jmpPossible(my_brett, zeile, spalte, dir, dir)){//<= oder <  ??
            printf("in erstem jump ");
            printf("my_brett[%i][%i] = %c\n", zeile + dir, spalte + dir, my_brett[zeile + dir][spalte + dir][0]);

            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            didJump = calculateJump(possible_moves, &current_move, my_brett, zeile, spalte, dir, dir);
            dirUpperRight = TRUE;
        }
        //springe falls schwarz nach oben links
        if (jmpPossible(my_brett, zeile, spalte, dir, -dir)){
            printf("in zweitem jump ");
            printf("my_brett[%i][%i] = %c\n", zeile + dir, spalte - dir, my_brett[zeile + dir][spalte - dir][0]);

            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            didJump = calculateJump(possible_moves, &current_move, my_brett, zeile, spalte, dir, -dir);
            dirUpperLeft = TRUE;
        }
        //springe falls schwarz nach unten rechts
        if (jmpPossible(my_brett, zeile, spalte, -dir, dir)){
            printf("in dritten jump ");
            printf("my_brett[%i][%i] = %c\n", zeile - dir, spalte + dir, my_brett[zeile - dir][spalte + dir][0]);
            //return possible_moves;

            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            didJump = calculateJump(possible_moves, &current_move, my_brett, zeile, spalte, -dir, dir);
        }
        //springe falls schwarz nach unten links
        if (jmpPossible(my_brett, zeile, spalte, -dir, -dir)){
            printf("in vierten jump ");
            printf("my_brett[%i][%i] = %c\n", zeile - dir, spalte - dir, my_brett[zeile - dir][spalte - dir][0]);

            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            didJump = calculateJump(possible_moves, &current_move, my_brett, zeile, spalte, -dir, -dir);
        }

        /*
         * Move Abfragen
         */
        if (didJump == FALSE && is_jump == FALSE) {
            if (dirUpperRight == FALSE){
                //Move falls oben rechts frei
                if ((zeile + dir) >0 && (spalte + dir) <=8 && (spalte + dir) >0 && (zeile + dir) <=8 && my_brett[zeile + dir][spalte + dir][0] == '-'){
                    calculateMove(possible_moves, &current_move, zeile, spalte, dir, dir);
                }
            }

            if (dirUpperLeft == FALSE){
                //Move falls oben links frei
                if ((zeile + dir) <=8 && (zeile + dir) >0 && (spalte - dir) <=8 && (spalte - dir) >0 && my_brett[zeile + dir][spalte - dir][0] == '-'){
                    calculateMove(possible_moves, &current_move, zeile, spalte, dir, -dir);
                }
            }
        }
    }
}

void calculateDameMove(short** possible_moves, short* current_move, short zeile, short spalte, char my_brett[9][9][13], short addZeile, short addSpalte) {
    printf("HALLO meine liebe Dame!\n\n");
    printf("Ma'Lady\n");
    short i = zeile, j = spalte;

    while(( my_brett[i][j][0] == '-') && (addZeile > 0 ? (i<=8) : (i>=1)) && (addSpalte > 0 ? (j<=7) : (j>=2))){
        i += addZeile;
        j += addSpalte;
        
        short* move = possible_moves[(*current_move)++];
        move[0] = MOVE_RATING;
        move[1] = zeile;
        move[2] = spalte;
        move[3] = i;
        move[4] = j;
    }
}

int calculateDameJump(short** possible_moves, short* current_move, short zeile, short spalte, char my_brett[9][9][13], short addZeile, short addSpalte) {
    printf("trying calculate Dame Jump from (%d, %d) with addZeile: %d, addSpalte: %d\n", zeile, spalte, addZeile, addSpalte);

    short i = zeile, j = spalte;
    while(( my_brett[i][j][0] == '-') && (addZeile > 0 ? (i<=6) : (i>=3)) && (addSpalte > 0 ? (j<=6) : (j>=3))){
        i += addZeile;
        j += addSpalte;
    }
    
    if ((my_brett[i][j][0] == colourEnemy || my_brett[i][j][0] == toupper(colourEnemy)) && (my_brett[i + addZeile][j + addSpalte][0] == '-') && (i<=7 && j<=7 && i>=2 && j>=2)) {
        printf("in erster if in calculateDameJmp\n");
        short* move = possible_moves[(*current_move)++];
        if(my_brett[i][j][0] == colourEnemy){
            move[0] = JUMP_RATING;
        }else {
            move[0] = JUMP_QUEEN_RATING;
        }
        
        move[1] = zeile;
        move[2] = spalte;
        move[3] = i+addZeile;
        move[4] = j+addSpalte;
        
        //kopie des bretts
        char new_brett[9][9][13];
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                for (int k = 0; k < 13; k++) {
                    new_brett[i][j][k] = my_brett[i][j][k];
                }
            }
        }

        //Alte Position leeren
        new_brett[zeile][spalte][0] = '-';
        //Übersprungene Position leeren
        new_brett[i][j][0] = '-';
        //neue position einspeichern
        new_brett[i+addZeile][j+addSpalte][0] = my_brett[zeile][spalte][0];

        short** next_possible_moves = calloc(18, sizeof(short*));
        for (int i = 0; i < 18; i++) {
            next_possible_moves[i] = calloc(27, sizeof(short));
        }
        getPossibleMovesForPiece(next_possible_moves, i+ addZeile, j + addSpalte, new_brett, TRUE);


        int i = 0;// 12 34 78 910 spielbrett // -1 12 34 78 ??// -1 34 78 910// -1 78 910
        short *next_move = next_possible_moves[i];

        //Überprüfen ob erster Zug von next_possible_moves ein Jump ist (-1)
        while ((next_move != NULL) && (next_move[0] != -200) && (next_move[0] == JUMP_RATING || next_move[0] == JUMP_QUEEN_RATING)) {
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

        for(int k = 0; i < 18; k++) {
            free(next_possible_moves[k]);
        }
        free(next_possible_moves);

        return TRUE;
    }
    
    return FALSE;
}
void calculateMove(short** possible_moves, short *current_move, short zeile, short spalte, short addZeile, short addSpalte) {
    short* move = possible_moves[(*current_move)++];

    if ((zeile + addZeile == 8 && colour == 'w') || (zeile + addZeile == 1 && colour == 'b')){
        move[0] = GET_QUEEN_RATING;//ist Dame geworden
    } else {
        move[0] = MOVE_RATING;//move
    }

    move[1] = zeile;
    move[2] = spalte;

    move[3] = zeile + addZeile;
    move[4] = spalte + addSpalte;
}

int calculateJump(short** possible_moves, short *current_move, char my_brett[9][9][13], short zeile, short spalte, short addZeile, short addSpalte) {
    if(my_brett[zeile - 2*addZeile][spalte - 2*addSpalte][0] != '-'){
        return FALSE;
    }

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

    return TRUE;
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

int jmpPossible(char my_brett [9][9][13], short zeile, short spalte, short addZeile, short addSpalte){
    return (zeile + addZeile + addZeile) <9 
        && (zeile + addZeile + addZeile) >0 
        && (spalte + addSpalte + addSpalte) <9 
        && (spalte + addSpalte + addSpalte) >0 
        && (
            my_brett[zeile + addZeile][spalte + addSpalte][0] == colourEnemy 
            || my_brett[zeile + addZeile][spalte + addSpalte][0] == toupper(colourEnemy)
        );
}

//1vrh39df3qaaq
//HellO!