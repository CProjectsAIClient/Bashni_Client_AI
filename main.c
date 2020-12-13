#include <stdio.h>
#include <stdlib.h>
#include "performConnection.h"
#include <getopt.h>
#include <string.h>




int main(int argc, char* argv[]) {
    //variablen fuer konsolenparameter
    char *playid = NULL;
    //playid = calloc(14, sizeof(char));
    int player=0;

    //filedeskriptor erstellen
    int* sock = NULL;
    sock = calloc(1,sizeof(int));
    *sock = makeConnection();

    //einlesen der Konsolenwerte mit getopt fuer eingaben -g gameid -p player
    int c;
    while ((c = getopt(argc,argv, "g:p:")) != -1){
        switch(c){
            case 'g':
                playid = optarg;
                break;
            case 'p':
                player = atoi(optarg);
                break;
            case ':':
                printf("Wert fehlt fuer g oder p");
                break;
            case '?':
                printf("Falsches Argument oder Aehnliches...");
                break;
            default:
                printf("Irgendwas laeuft schief bei getopt");
                break;
        }
    }

    //printf("playid=%s, player:%i\n", playid, player);

    doperformConnection(sock, playid, player);

    
    //befreien von allozierten speicherplaetzen
    //free(playid);




    return EXIT_SUCCESS;
}