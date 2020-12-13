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
    //free(buffer);
    //buffer = calloc(1024, sizeof(char));
    //printf("S: ");
    char b[1024] = "";
    int i=0;
    char current;
    do {
        recv(*sock, &current, 1, 0);
        //(*buffer)++ = current;
        b[i++] = current;
        
        //printf("current: %c\n", *current);
        //buffer[i] = cu
        //*(buffer+i) = *current;
        // printf("buffer+i:%c \n",*(buffer+i));
        // i++;
        //sprintf(buffer, "%c", current);
        //printf("%c",current);
    } while (current != '\n');
    
    buffer = b;
    

    //ssize_t size;
    //size = recv(*sock, buffer, BUF-1, 0);

    //if (size > 0) buffer[size] = '\0';
    

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
    char buff[strlen(buffer)+1];
    sprintf(buff, "%s\n", buffer);


    send(*sock, buff,strlen(buff), 0);
    printf("C: %s", buff);
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
    sprintf(gameId, "ID %s", gameid);
    printf("GameID: %s\n", gameId);

    //Ausgeben der Player ID fuer den Server
    char playerNr[10];
    sprintf(playerNr, "PLAYER %d", --player);
    printf("PlayerID: %s\n\n\n\n", playerNr);

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
    char* total = myread(sock, buffer);
    printf("Total: %s\n", total);
    //int count = strtol(total, &total, 8);
    //int count = atoi(*(total[8]));
    int count = atoi(total+8);
    printf("Count %d\n", count);


    while(count-1){
        count--;
        myread(sock, buffer);
    }

    char *end = myread(sock, buffer);
    if (*end != '+') {
        printf("Fehler in der Prolog Phase!");
        exit(0);
    }
    
    free(buffer);
    exit(1);
//27zw8h8snq5at


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

