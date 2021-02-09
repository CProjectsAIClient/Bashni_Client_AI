#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>

#include "thinker.h"
#include "random_ki.h"
#include "performConnection.h"

#define DIR_UP_RIGHT 1
#define DIR_UP_LEFT 2
#define DIR_DOWN_RIGHT 3
#define DIR_DOWN_LEFT 4

int getPossibleMovesForPiece(short** possible_moves, short i, short j, char my_brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE], char piece_color, int is_jump, int jump_dir);
void calculateDame(short** possible_moves, short* current_move, short zeile, short spalte, char my_brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE]);
int calculateJump(short** possible_moves, short* current_move, char my_brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE], char piece_color, short zeile, short spalte, short addZeile, short addSpalte);
void calculateMove(short** possible_moves, short* current_move, char piece_color, short zeile, short spalte, short addZeile, short addSpalte);
void calculateDameMove(short** possible_moves, short* current_move, short zeile, short spalte, char my_brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE], short addZeile, short addSpalte);
int calculateDameJump(short** possible_moves, short* current_move, short zeile, short spalte, char my_brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE], char piece_color, short addZeile, short addSpalte);
int jmpPossible(char my_brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE], short zeile, short spalte, short addZeile, short addSpalte, char piece_enemy_colour);
int getDir(short addZeile, short addSpalte, char piece_color);
void printMoves(short** possible_moves);
void printMove(short* move);
char* translateMove(short* moves);

void initialize_random_ki(struct game* game_struct){
    //Farbe zuordnen
    if (game_struct->player_number == 0){
        colour = 'w';
        colourEnemy = 'b';
        //spaeter auch mit toupper checken...
    } else {
        colour = 'b';
        colourEnemy = 'w';
    }
}


//zusammenfassung aller mgl moves und eintscheidung fuer einen
char* getMove(char my_brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE]){
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

    // printf("\n\n=== Mögliche Moves: ===\n");

    for (i = 1; i <= 8; i++) {
        for (j = 1; j <= 8; j++) {
            if (my_brett[i][j][0] == colour || my_brett[i][j][0] == toupper(colour)) {
                short move[12];
                getPossibleMovesForPiece(saveMoves[counter], i, j, my_brett, colour, false, 0);

                
                if (saveMoves[counter][0][0] != -200) {

                    //printMoves(saveMoves[counter]);

                    //Auf jump überprüfen
                    if (saveMoves[counter][0][0] == JUMP_RATING || saveMoves[counter][0][0] == JUMP_QUEEN_RATING) {
                        jumps[jmpcounter] = counter;
                        jmpcounter++;

                    }

                    counter++;
                }
            }
        }
    }
    


    if (saveMoves[counter-1][0][0] == -200) {
        counter--;
    }
    char* random_move;
    
    short x, isjump = false;
    if (jmpcounter == 0) {
        x = rand() % counter;
    } else {
        short z = rand() % jmpcounter;
        x = jumps[z];
        isjump = true;
    }
    
    int possible_moves_counter = 0;
    while (saveMoves[x][possible_moves_counter][0] != 0) {
        possible_moves_counter++;
    }
    srand(time(NULL));
    short y = isjump ? 0 : rand() % possible_moves_counter;

    random_move = translateMove(saveMoves[x][y]);


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
int getPossibleMovesForPiece(short** possible_moves, short zeile, short spalte, char my_brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE], char piece_color, int is_jump, int jump_dir){
    short dir = (piece_color == 'w') ? 1 : -1, current_move = 0;
    possible_moves[0][0] = (short) -200;

    //pruefe auf dame
    if (my_brett[zeile][spalte][0] == toupper(piece_color)){
        printf("dir == %d\n", jump_dir);
        if (!is_jump || jump_dir != DIR_DOWN_LEFT) {

            //Überprüfe auf Jump nach rechts oben
            is_jump = calculateDameJump(possible_moves, &current_move, zeile, spalte, my_brett, piece_color, dir, dir) || is_jump;
        }
        if (!is_jump || jump_dir != DIR_DOWN_RIGHT) {

            //Überprüfe auf Jump nach links oben
            is_jump = calculateDameJump(possible_moves, &current_move, zeile, spalte, my_brett, piece_color, dir, -dir) || is_jump;
        }
        if (!is_jump || jump_dir != DIR_UP_LEFT) {

            //Überprüfe auf Jump nach rechts unten
            is_jump = calculateDameJump(possible_moves, &current_move, zeile, spalte, my_brett, piece_color, -dir, dir) || is_jump;
        }
        if (!is_jump || jump_dir != DIR_UP_RIGHT) {

            //Überprüfe auf Jump nach links unten
            is_jump = calculateDameJump(possible_moves, &current_move, zeile, spalte, my_brett, piece_color, -dir, -dir) || is_jump;
        }
        
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

        return is_jump;
    }
    //normaler Stein
    else {
        int didJump = false, dirUpperRight = false, dirUpperLeft = false;
        char piece_enemy_colour = (piece_color == 'w' ? 'b' : 'w');

        /*
         * Jump Abfragen
         */
        //springe falls schwarz nach oben rechts
        if (jmpPossible(my_brett, zeile, spalte, dir, dir, piece_enemy_colour)){
            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            didJump = calculateJump(possible_moves, &current_move, my_brett, piece_color, zeile, spalte, dir, dir);
            dirUpperRight = true;
        }
        //springe falls schwarz nach oben links
        if (jmpPossible(my_brett, zeile, spalte, dir, -dir, piece_enemy_colour)){
            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            didJump = calculateJump(possible_moves, &current_move, my_brett, piece_color, zeile, spalte, dir, -dir);
            dirUpperLeft = true;
        }
        //springe falls schwarz nach unten rechts
        if (jmpPossible(my_brett, zeile, spalte, -dir, dir, piece_enemy_colour)){
            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            didJump = calculateJump(possible_moves, &current_move, my_brett, piece_color, zeile, spalte, -dir, dir);
        }
        //springe falls schwarz nach unten links
        if (jmpPossible(my_brett, zeile, spalte, -dir, -dir, piece_enemy_colour)){
            //Abfragen ob nächstes Feld hinter dem gegnerischen Stein frei ist
            didJump = calculateJump(possible_moves, &current_move, my_brett, piece_color, zeile, spalte, -dir, -dir);
        }

        /*
         * Move Abfragen
         */
        if (didJump == false && is_jump == false) {
            if (dirUpperRight == false){
                //Move falls oben rechts frei
                if ((zeile + dir) >0 && (spalte + dir) <=8 && (spalte + dir) >0 && (zeile + dir) <=8 && my_brett[zeile + dir][spalte + dir][0] == '-'){
                    calculateMove(possible_moves, &current_move, piece_color, zeile, spalte, dir, dir);
                }
            }

            if (dirUpperLeft == false){
                //Move falls oben links frei
                if ((zeile + dir) <=8 && (zeile + dir) >0 && (spalte - dir) <=8 && (spalte - dir) >0 && my_brett[zeile + dir][spalte - dir][0] == '-'){
                    calculateMove(possible_moves, &current_move, piece_color, zeile, spalte, dir, -dir);
                }
            }
        }

        return didJump;
    }
}

void calculateDameMove(short** possible_moves, short* current_move, short zeile, short spalte, char my_brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE], short addZeile, short addSpalte) {
    printf("trying to calculate Dame Move from (%d, %d) with addZeile: %d, addSpalte %d\n", zeile, spalte, addZeile, addSpalte);
    short i = zeile, j = spalte;

    while((my_brett[i+addZeile][j+addSpalte][0] == '-') && (addZeile > 0 ? (i<=7) : (i>=2)) && (addSpalte > 0 ? (j<=7) : (j>=2))){
        
        //printf("i: %d, j: %d\n", i, j);
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

int calculateDameJump(short** possible_moves, short* current_move, short zeile, short spalte, char my_brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE], char piece_color, short addZeile, short addSpalte) {
    printf("trying calculate Dame Jump from (%d, %d) with addZeile: %d, addSpalte: %d\n", zeile, spalte, addZeile, addSpalte);
    short i = zeile + addZeile, j = spalte + addSpalte;
    // if(i>8 || i<1){
    //     return;
    // }
    while((addZeile > 0 ? (i<=6) : (i>=3)) && (addSpalte > 0 ? (j<=6) : (j>=3)) && (my_brett[i][j][0] == '-')){
        //printf("i: %d und j: %d\n", i,j);
        i += addZeile;
        j += addSpalte;
    }

    char piece_enemy_colour = (piece_color == 'w' ? 'b' : 'w');
    if ((i<=7 && j<=7 && i>=2 && j>=2) && (my_brett[i][j][0] == piece_enemy_colour || my_brett[i][j][0] == toupper(piece_enemy_colour)) && (my_brett[i + addZeile][j + addSpalte][0] == '-')) {
        short* move = possible_moves[(*current_move)++];
        if(my_brett[i][j][0] == piece_enemy_colour){
            move[0] = JUMP_RATING;
        }else {
            move[0] = JUMP_QUEEN_RATING;
        }
        
        move[1] = zeile;
        move[2] = spalte;
        move[3] = i+addZeile;
        move[4] = j+addSpalte;
        
        //kopie des bretts
        char new_brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE];
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                for (int k = 0; k < MAX_TOWER_SIZE; k++) {
                    new_brett[i][j][k] = my_brett[i][j][k];
                }
            }
        }

        //Alte Position leeren
        new_brett[zeile][spalte][0] = '-';
        //Übersprungene Position leeren
        new_brett[i][j][0] = '-';
        //Aktuellen Turm zu neuer Position kopieren
        // for (int i = 0; i < MAX_TOWER_SIZE; i++) {
        //     new_brett[i+addZeile][j+addSpalte][i] = my_brett[zeile][spalte][i];
        // }
        new_brett[i+addZeile][j+addSpalte][0] = my_brett[zeile][spalte][0];

        short** next_possible_moves = calloc(18, sizeof(short*));
        for (int i = 0; i < 18; i++) {
            next_possible_moves[i] = calloc(27, sizeof(short));
        }
        getPossibleMovesForPiece(next_possible_moves, i+ addZeile, j + addSpalte, new_brett, piece_color, true, getDir(addZeile, addSpalte, piece_color));


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

            //printMove(move);
            //Erstelle einen neuen Zug
            move = possible_moves[(*current_move)++];

            //setze die default Werte für den neuen Zug
            move[0] = JUMP_RATING; // jump
            move[1] = zeile; //alte zeile
            move[2] = spalte; //alte spalte

            move[3] = zeile + addZeile + addZeile; //neue zeile
            move[4] = spalte + addSpalte + addSpalte; //neue spalte
        }

        for(int k = 0; k < 18; k++) {
            free(next_possible_moves[k]);
        }
        free(next_possible_moves);

        return true;
    }
    
    return false;
}
void calculateMove(short** possible_moves, short *current_move, char piece_color, short zeile, short spalte, short addZeile, short addSpalte) {
    short* move = possible_moves[(*current_move)++];

    if ((zeile + addZeile == 8 && piece_color == 'w') || (zeile + addZeile == 1 && piece_color == 'b')){
        move[0] = GET_QUEEN_FROM_MOVE_RATING;//ist Dame geworden
    } else {
        move[0] = MOVE_RATING;//move
    }

    move[1] = zeile;
    move[2] = spalte;

    move[3] = zeile + addZeile;
    move[4] = spalte + addSpalte;
}

int calculateJump(short** possible_moves, short *current_move, char my_brett[FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE], char piece_color, short zeile, short spalte, short addZeile, short addSpalte) {
    if(my_brett[zeile + 2*addZeile][spalte + 2*addSpalte][0] != '-'){
        return false;
    }

    char new_brett [FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE];
    
    //kopie des bretts
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            for (int k = 0; k < MAX_TOWER_SIZE; k++) {
               new_brett[i][j][k] = my_brett[i][j][k]; 
            }
        }
    }

    //stein den wir ueberspringen loeschen
    new_brett[zeile + addZeile][spalte + addSpalte][0] = '-';
    //alte Position des eigenen Steins löschen
    new_brett[zeile][spalte][0] = '-';

    //Aktuellen Turm zu neuer Position kopieren
    for (int i = 0; i < MAX_TOWER_SIZE; i++) {
        new_brett[zeile + 2*addZeile][spalte + 2*addSpalte][i] = my_brett[zeile][spalte][i];
    }
    //eigenen Stein neu platzieren und checken wir ob wir eine Dame werden
    if((zeile + addZeile + addZeile == 8 && piece_color == 'w') || (zeile + addZeile + addZeile == 1 && piece_color == 'b')){
        new_brett[zeile + addZeile + addZeile][spalte + addSpalte + addSpalte][0] = my_brett[zeile][spalte][0] - 32;
    }
    

    short* move = possible_moves[(*current_move)++];//calloc(27, sizeof(short));    
    move[0] = JUMP_RATING; // jump
    move[1] = zeile; //alte zeile
    move[2] = spalte; //alte spalte

    move[3] = zeile  + addZeile  + addZeile; //neue zeile
    move[4] = spalte + addSpalte + addSpalte; //neue spalte
    //[[-1, neue_zeile, neue_spalte, ...], [-1, neue_zeile, neue_spalte, ...], [-1, neue_zeile, neue_spalte, ...]
    short** next_possible_moves = calloc(18, sizeof(short*));
    for (int i = 0; i < 18; i++) {
        next_possible_moves[i] = calloc(27, sizeof(short));
    }
    int getdir = getDir(addZeile, addSpalte, piece_color);
    getPossibleMovesForPiece(next_possible_moves, zeile + addZeile + addZeile, spalte + addSpalte + addSpalte, new_brett, piece_color, true, getdir);
    
    // -1,  5, 6,  7, 8                 // -1  7, 8,  8, 9          // -1  8, 9,  6, 5
    // -1,  5, 6,  7, 8,  8, 9,  6, 5   <- -1  7, 8,  8, 9,  6, 5   <-

    int i = 0;// 12 34 78 910 spielbrett // -1 12 34 78 ??// -1 34 78 910// -1 78 910
    short *next_move = next_possible_moves[i];

    //Überprüfen ob erster Zug von next_possible_moves ein Jump ist (-1)
    while ((next_move != NULL) && (next_move[0] != -200) && (next_move[0] == JUMP_RATING)) {
        int j = 3, k = 5;
        next_move = next_possible_moves[i];

        //Gehe durch alle Teilzüge von next_move
        while (next_move[j] > 0)
        {
            //Speichere Teilzug in move
            move[k] = next_move[j];
            //next_move[j] = -200;
            k++;
            j++;
        }

        printf("i: %d\n", i);
        int isContained = 1;
        if (i > 0) {
            short* oldMove = possible_moves[(*current_move)-1];
            int l = 1;
            while (move[l] > 0 && l < 27)
            {
                if (oldMove[l] != move[l]) {
                    isContained = 0;
                }
                l++;
            } 
        } else {
            isContained = 0;
        }

        if (!isContained) {
            //Erstelle einen neuen Zug
            move = possible_moves[(*current_move)++];

            //setze die default Werte für den neuen Zug
            move[0] = JUMP_RATING; // jump
            move[1] = zeile; //alte zeile
            move[2] = spalte; //alte spalte

            move[3] = zeile + addZeile + addZeile; //neue zeile
            move[4] = spalte + addSpalte + addSpalte; //neue spalte 
        } else {
            move[0] = -200;
            for (int n = 1; n < 27; n++){
                move[n] = 0;
            }
        }

        i++;
    }

    //printMoves(possible_moves);
    for (int i = 0; i < 18; i++) {
        free(next_possible_moves[i]);
    }
    free(next_possible_moves);

    return true;
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

int jmpPossible(char my_brett [FIELD_SIZE][FIELD_SIZE][MAX_TOWER_SIZE], short zeile, short spalte, short addZeile, short addSpalte, char piece_enemy_colour){
    return (zeile + addZeile + addZeile) <9 
        && (zeile + addZeile + addZeile) >0 
        && (spalte + addSpalte + addSpalte) <9 
        && (spalte + addSpalte + addSpalte) >0 
        && (
            my_brett[zeile + addZeile][spalte + addSpalte][0] == piece_enemy_colour 
            || my_brett[zeile + addZeile][spalte + addSpalte][0] == toupper(piece_enemy_colour)
        );
}

int getDir(short addZeile, short addSpalte, char piece_color) {
    if (addZeile > 0 && addSpalte > 0) {
        return (piece_color == 'w') ? DIR_UP_RIGHT : DIR_DOWN_LEFT;
    } else if (addZeile > 0 && addSpalte < 0) {
        return (piece_color == 'w') ? DIR_UP_LEFT : DIR_DOWN_RIGHT;
    } else if (addZeile < 0 && addSpalte > 0) {
        return (piece_color == 'w') ? DIR_DOWN_RIGHT : DIR_UP_LEFT;
    } else if (addZeile < 0 && addSpalte < 0) {
        return (piece_color == 'w') ? DIR_DOWN_LEFT : DIR_UP_RIGHT;
    }
}

//3amwjujtainsg

//./sysprak-client -g  -p 1 -c client.conf
//valgrind --leak-check=full --trace-children=yes ./sysprak-client -g  -p 1 -c client.conf