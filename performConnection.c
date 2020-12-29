#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//header fuer socket
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "performConnection.h"
#include "config.h"

#define BUF 1024

//liest werte vom Server
char* myread(int *sock, char *buffer);

void mywrite(int * sock, char *buffer);

int makeConnection(game_config game_conf){
    //socket anlegen
    int sock;
    if((sock = socket(AF_INET,SOCK_STREAM,0)) <= 0){
        printf("Socket Fehler: %d!\n", sock);
    }

    //verbindungsadresse eingeben
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(game_conf.portnumber);
   
    //Wandelt Hostname in Adresse um;
    struct hostent *hp;
    hp = gethostbyname(game_conf.hostname);
    if(hp == NULL) {
        fprintf(stderr,"%s unknown host.\n",game_conf.hostname);
        exit(2);
    }
    // copies the internet address to server address
    bcopy(hp->h_addr_list[0], &addr.sin_addr, hp->h_length);

    int connected;
    //Verbindung aufbauen
    printf("Verbinde zum Gameserver...\n");
    if((connected = connect(sock, (struct sockaddr*) &addr, sizeof(addr))) == 0){
        printf("Verbindung aufgebaut!\n");
    } else {
        printf("Verbindungsfehler: %i\n", connected);
    }

    return sock;
}

void doperformConnection(int *sock, char gameid[], int player, game *current_game, struct player* enemy_list){
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
    char *game_name = myread(sock, buffer);

    //Name finden und speichern
    int j = 0;
    char current2;
    game_name += 2;
    current2 = *game_name;
    while (current2 != '\n') {
        current_game->name[j++] = current2;
        game_name++;
        current2 = *game_name;
    } 

    mywrite(sock, playerNr);
    
    //Server schickt die eigene Mitspielernummer + Name
    char * current_player = myread(sock, buffer);
    current_game->player_number = atoi(current_player+5);
    

    //Server schickt die Mitgliederanzahl
    char *total = myread(sock, buffer);
    int count = atoi(total+8);
    current_game->player_count = count;
    
    int a = 0;
    struct player enemies[count];
    while(a < count - 1){
        // int player_id, ready;
        // char player_name[120];

        // char * readed_player = myread(sock, buffer);
        // sscanf(readed_player, "+ %i %s %i", &player_id, player_name, &ready);
        // printf("PlayerId: %i, Name: %s, Bereit: %i\n", player_id, player_name, ready);
        
        // enemies[a].number = player_id;
        // strcpy(enemies[a].name, player_name);
        // //enemies[a].name = player_name;
        // enemies[a].registered = ready;

        //Player info lesen
        char* enemy = myread(sock, buffer);
        enemies[a].number = atoi(enemy+2);
        
        //Name2 array definieren und enemy auf den ersten Buchstabe setzen
        //char name2[BUF];
        int i = 0;
        enemy += 4;
        
        //Name finden und speichern
        char current;
        current = *enemy;
        while (current != ' ') {
            enemies[a].name[i++] = current;
            enemy++;
            current = *enemy;
        } 

        //Name der Struct zuweisen
        //enemies[a].name = name2;
        enemies[a].registered = atoi(enemy+1);
        
        a++;
    }

    enemy_list = enemies;

    //Fehlermeldungen
    char *end = myread(sock, buffer);
    if (*end != '+') {
        printf("Fehler in der Prolog Phase!");
        exit(0);
    }
    
    //free(total);

}

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
        printf("Es gab ein Problem...😭\n");
        printf("\n bei %s\n", b);
        exit(0);
    } else {
        printf("🍕 S: %s", b);
    }

    return buffer;
}

void mywrite(int *sock, char *buffer){
    //Erstellt char Speicher mit der buffer String laenge +1 (für \n)
    char buff[strlen(buffer)+1];
    //Fuegt \n an
    sprintf(buff, "%s\n", buffer);
    //Sendet Nachricht an den Server
    send(*sock, buff,strlen(buff), 0);
    printf("💻 C: %s", buff);
}

//3ua6fayf5s7qz

