#ifndef connector
#define connector
#include "performConnection.h"

void startConnector(int fd_sock, int fd_pipe);

void doSpielVerlauf(int *sock, int player, game *current_game, int anzahl_Steine);

int checkWait(char*buffer,int * sock);
void answerWait(int * sock);
int checkMove(char*buffer);
int checkGameover(char*buffer);
int checkQuit(char*buffer);

#endif