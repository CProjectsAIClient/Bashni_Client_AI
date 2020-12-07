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

#define BUF 1024

#define GAMEKINDNAME "Bashni"
#define PORTNUMBER 1357
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de" 

char* myread(int *sock, char *buffer) {
    ssize_t size;
    size = recv(*sock, buffer, BUF-1, 0);

    if (size > 0) buffer[size] = '\0';
    printf("S: %s", buffer);

    return buffer;
}

void mywrite(int * sock, char * buffer){
    send(*sock, buffer,strlen(buffer), 0);
    printf("C: %s", buffer);
}



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

    //Ausgaben des Client in der Kommunikation mit dem Server
    //Ausgabe der GameID des Client
    char gameId[18];//14 fuer gameID und 3 fuer "ID " und 1 fuer \n
    sprintf(gameId, "ID %s\n", gameid);
    printf("GameID: %s\n", gameId);

    //Ausgeben der Player ID fuer den Server
    char playerNr[10];
    sprintf(playerNr, "PLAYER %d\n", player);
    printf("PlayerID: %s\n\n\n\n", playerNr);

    //client wird nach Version gefragt + rueckgabe der Version
    myread(sock,buffer);
    mywrite(sock,"VERSION 2.3\n");

    //Client wird nach SpielID gefragt + rueckgabe
    myread(sock,buffer);
    mywrite(sock,gameId);
    

    //Client wird nach gewuenschter Spielernummer gefragt + Antwort
    myread(sock,buffer);
    //mywrite(sock,playerNr);
    mywrite(sock,"PLAYER");
    
    //Server schickt die eigene Mitspielernummer + Name
    myread(sock,buffer);
    //Server schickt die Mitgliederanzahl
    char* total = myread(sock,buffer);
    printf("Total: %s", total);
    int count = atoi(total+6);
    printf("Count %d\n", count);
    free(buffer);
    exit(1);

    while(count - 1){
        count--;
        myread(sock,buffer);
    }
//14c82rbsc4ymz


    // do {
    //     size = recv(*sock, buffer, BUF-1, 0);

    //     if (size > 0) buffer[size] = '\0';
    //     printf("S: %s", buffer);

    //     if ( strcmp(buffer,"quit\n") ) {
    //         printf("C: ");
    //         fgets(buffer, BUF, stdin);
    //         send(*sock, buffer, strlen(buffer), 0);
    //     }
    // } while ( strcmp(buffer,"quit\n") != 0 );

   
}

