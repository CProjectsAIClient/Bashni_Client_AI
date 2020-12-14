#include <stdio.h>
#include<stdlib.h>
#include "performConnection.h"
#include "config.h"

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


//liest werte vom Server
char* myread(int *sock, char *buffer) {
    //Erstellt char Speicher mit Größe BUF zum Lesen vom Server
    char b[BUF] = "";
    int i=0;
    char current;
    //liest Nachricht in einzelnen char ein
    do {
        recv(*sock, &current, 1, 0);
        b[i++] = current;
    } while (current != '\n');
    
    buffer = b;
    //beruecksichtigt moegliche fehler
   if (b[0] == '-'){
        printf("Es gab ein Problem...\n");
        printf("\n bei %s\n", b);
        exit(0);
    } else {
        printf("S: %s", b);
    }

    return buffer;
}

void mywrite(int * sock, char * buffer){
    //Erstellt char Speicher mit der buffer String laenge +1 (für \n)
    char buff[strlen(buffer)+1];
    //Fuegt \n an
    sprintf(buff, "%s\n", buffer);
    //Sendet Nachricht an den Server
    send(*sock, buff,strlen(buff), 0);
    printf("C: %s", buff);
}

int makeConnection(){
    //socket anlegen
    int sock;
    if( (sock = socket(AF_INET,SOCK_STREAM,0))<=0){
        printf("Socket Fehler: %d!\n", sock);
    }

    //verbindungsadresse eingeben
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORTNUMBER);
   
    //Wandelt Hostname in Adresse um;
    struct hostent *hp;
    hp = gethostbyname(HOSTNAME);
    if(hp == NULL) {
        fprintf(stderr,"%s unknown host.\n",HOSTNAME);
        exit(2);
    }
    // copies the internet address to server address
    bcopy(hp->h_addr_list[0], &addr.sin_addr, hp->h_length);

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
    //printf("Chat\n\n\n");
    char *buffer; // = (char*) malloc(sizeof(char) * BUF);
    ssize_t size;

    //Ausgaben des Client in der Kommunikation mit dem Server
    //Ausgabe der GameID des Client
    char gameId[18];//14 fuer gameID und 3 fuer "ID " und 1 fuer \n
    sprintf(gameId, "ID %s", gameid);
    //printf("GameID: %s\n", gameId);

    //Ausgeben der Player ID fuer den Server
    char playerNr[10];
    sprintf(playerNr, "PLAYER %d", player);
    //printf("PlayerID: %s\n\n\n\n", playerNr);

    //client wird nach Version gefragt + rueckgabe der Version
    myread(sock, buffer);
    mywrite(sock,"VERSION 2.3");

    //Client wird nach SpielID gefragt + rueckgabe
    myread(sock, buffer);
    mywrite(sock,gameId);
    

    //Client wird nach gewuenschter Spielernummer gefragt + Antwort
    myread(sock, buffer);
    myread(sock, buffer);
    //mywrite(sock,playerNr);
    mywrite(sock, playerNr);
    
    //Server schickt die eigene Mitspielernummer + Name
    myread(sock, buffer);

    //Server schickt die Mitgliederanzahl
    char *total = myread(sock, buffer);
    int count = atoi(total+8);


    while(count-1){
        count--;
        myread(sock, buffer);
    }

    char *end = myread(sock, buffer);
    if (*end != '+') {
        printf("Fehler in der Prolog Phase!");
        exit(0);
    }
    
    //free(total);
}

