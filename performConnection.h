#ifndef performConnection
#define performConnection
#define BUF1 128

#define GAME_QUIT 0
#define GAME_MOVE 1

#include "config.h"

typedef struct game {
    //Name des Spiels zB Bashni
    char name[BUF1];
    //Nummer des zugewiesenen Spielers
    int player_number;
    //Anzahl der Spieler innerhalb des Spiels
    int player_count;
    //Prozess ID des Thinkers
    int thinkerID;
    //Prozess ID des Connectors
    int connectorID;
    //flag fuer QUIT 0 und fuer Move 1
    int flag;
    //Adresse des zweiten SHM mit dem Spielfeld
    int shmFieldID;
} game;

typedef struct player {
    //Nummer des Spielers vom Server
    int number;
    //Name des Spielers vom Server
    char name[BUF1];
    //1 für einen registrierten Spieler, 0 ansonsten
    int registered;
} player;

// typedef struct brett {
//     //Farbe des Steins
//     char color;
//     //Column A,B,C,D,E,F,G,H
//     int column;
//     //Row 1,2,3,4,5,6,7,8
//     int row;
// } brett;

int makeConnection(game_config config);

void doperformConnection(int *sock, char *gameid, int player, game *current_game, struct player* player_list);

char* myread(int *sock, char *buffer);

void mywrite(int *sock, char *buffer);

#endif