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
    //flag fuer GAMEOVER 1 fuer ja 0 fuer nein
    int gameoverFlag;
    //flag fuer WAIT 1 fuer Kommando kam 0 fuer Kommando kam nicht
    int waitFlag;
    //flag fuer MOVE 1 fuer Kommando kam 0 fuer Kommando kam nicht
    int moveFlag;
    //flag fuer Spielbeendung
    int quitFlag;
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

int makeConnection(game_config config);

void doperformConnection(int *sock, char *gameid, int player, game *current_game, struct player* player_list);

int checkWait(char*buffer,int * sock);
void answerWait(int * sock);
int checkMove(char*buffer);
int checkGameover(char*buffer);
int checkQuit(char*buffer);

#endif