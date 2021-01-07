#ifndef performConnection
#define performConnection

#include "config.h"

#define BUF 128
#define GAME_QUIT 0
#define GAME_MOVE 1

typedef struct game {
    //Name des Spiels zB Bashni
    char name[BUF];
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
    //Anzahl der Steine
    int pieces_count;
} game;

typedef struct player {
    //Nummer des Spielers vom Server
    int number;
    //Name des Spielers vom Server
    char name[BUF];
    //1 für einen registrierten Spieler, 0 ansonsten
    int registered;
} player;


int makeConnection(game_config config);

void doperformConnection(int *sock, char *gameid, int player, game *current_game);

char* myread(int *sock, char* buffer);

void mywrite(int *sock, char *buffer);

#endif