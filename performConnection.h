#ifndef performConnection
#define performConnection
#define BUF1 128

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
} game;

typedef struct player {
    //Nummer des Spielers vom Server
    int number;
    //Name des Spielers vom Server
    char name[BUF1];
    //1 für einen registrierten Spieler, 0 ansonsten
    int registered;
} player;

int makeConnection(game_config config);

void doperformConnection(int *sock, char *gameid, int player, game *current_game, struct player* player_list);

void doSpielVerlauf(int *sock, char *gameid, int player, game *current_game, int anzahl_Steine);

#endif