#include <stdio.h>
#include "performConnection.h"

#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
//header fuer socket
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define GAMEKINDNAME "Bashni"
#define PORTNUMBER 1357
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de" 

/* get opt machen methode soll kommandozeilenparameter aus main benutzen*/
void getConnectInfo(char *gameid, int player) {
    printf("playid=%s, player:%i\n", gameid, player);
}

void doperformConnection(){
    //socket anlegen
    int sock;
    if( (sock = socket(PF_INET,SOCK_STREAM,0))>0){
        printf("Socket steht...\n");
    }

    //verbindungsadresse eingeben
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORTNUMBER);
    inet_aton(HOSTNAME, &addr.sin_addr);

    //addr.sin_addr.s_addr = inet_addr(HOSTNAME);
    //server.sin_port = htons(PORTNUMBER);

    //Verbindung aufbauen
    if(connect(sock, (struct sockaddr*) &addr, sizeof(addr))== 0){
        printf("connect() war erfolgreich");
    } else {
        printf("Error");
    }



}