#ifndef connector
#define connector
#include "performConnection.h"

char my_brett[9][9][13];

void startConnector(int *sock);

void doSpielVerlauf(int *sock, int player, game *current_game, int anzahl_Steine);

int checkWait(char*buffer,int * sock);
void answerWait(int * sock);
int checkMove(char*buffer);
int checkGameover(char*buffer);
int checkQuit(char*buffer);

#endif