#include <stdio.h>
#include<stdlib.h>
#include "performConnection.h"

#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
//header fuer socket
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define BUF 256

#define GAMEKINDNAME "Bashni"
#define PORTNUMBER 1357
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de" 

/* get opt machen methode soll kommandozeilenparameter aus main benutzen*/
void getConnectInfo(char *gameid, int player) {
    // char v[15];
    // int k=0;
    // while(gameid){
    //     v[k++] = (char)gameid++;
    // }


    printf("playid=%s, player:%i\n", gameid, player);
}

int makeConnection(){
    //socket anlegen
    int sock;
    if( (sock = socket(AF_INET,SOCK_STREAM,0))>0){
        printf("Socket steht...: %d\n", sock);
    }

    //verbindungsadresse eingeben
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORTNUMBER);
    /*int cont;
    cont =inet_aton(HOSTNAME, &addr.sin_addr);
    printf("HOSTNAME existiert: %i\n", cont);//0 wenn es nicht funktioniert, was anderes wenn es funktioniert...*/

    //Wandelt Hostname in Adresse um;
    struct hostent *hp;
    hp = gethostbyname(HOSTNAME);
    if(hp == NULL) {
        fprintf(stderr,"%s unknown host.\n",HOSTNAME);
        exit(2);
    }
    /* copies the internet address to server address */
    bcopy(hp->h_addr_list[0], &addr.sin_addr, hp->h_length);

    //addr.sin_addr.s_addr = inet_addr(HOSTNAME);
    //server.sin_port = htons(PORTNUMBER);

    int connected;
    //Verbindung aufbauen
    if((connected = connect(sock, (struct sockaddr*) &addr, sizeof(addr)))== 0){
        printf("connect() war erfolgreich\n");
    } else {
        printf("Error: %i\n", connected);
    }

    return sock;
}

void doperformConnection(int *sock,char gameid[],  int player){
    printf("Chat\n\n\n");
    char *buffer = (char*) malloc(sizeof(char) * BUF);
    ssize_t size;
    char **clientsays;
    clientsays = calloc(3,sizeof(char));
    for(int i = 0; i<3; i++){
        *(clientsays+i) = calloc(BUF,sizeof(char));
    }

    // char *version;
    // char *id_game;
    // char *iwas;


    char gameId[3+sizeof(gameid)];
    sprintf(&gameId, "ID %s", gameid);
    printf("GameID: %s\n", gameId);

    char playerNr[8];
    sprintf(&playerNr, "PLAYER %d", player);
    printf("PlayerID: %s\n", playerNr);

    *(clientsays) = "VERSION 2.3";
    *(clientsays+1) = gameId;
    *(clientsays+2) = playerNr;

    

    do {
        size = recv(*sock, buffer, BUF-1, 0);

        if (size > 0) buffer[size] = '\0';
        printf("S: %s", buffer);

        if ( strcmp(buffer,"quit\n") ) {
            printf("C: ");
            fgets(buffer, BUF, stdin);
            send(*sock, buffer, strlen(buffer), 0);
        }
    } while ( strcmp(buffer,"quit\n") != 0 );

    for(int i = 0; i<3; i++){
        free(*(clientsays+i));
    }
    free(clientsays);
}