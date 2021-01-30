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

//liest Werte vom Server
char* myread(int* sock, char* buffer);
//schreibt Werte zum Server
void mywrite(int* sock, char* buffer);

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

void doperformConnection(int *sock, char gameid[], int player, game *current_game){
    char* buffer = malloc(BUF * sizeof(char));

    //Ausgaben des Client in der Kommunikation mit dem Server
    //Ausgabe der GameID des Client
    char gameId[18];//14 fuer gameID und 3 fuer "ID " und 1 fuer \n
    sprintf(gameId, "ID %s", gameid);
    //printf("GameID: %s\n", gameId);

    //Ausgeben der Player ID fuer den Server
    char playerNr[10];
    sprintf(playerNr, "PLAYER %d", player);
    //printf("PlayerID: %s\n\n\n\n", playerNr);

    //Server schickt Willkommensnachticht
    myread(sock, buffer);

    //Client schickt Versionsnummer
    mywrite(sock,"VERSION 2.3");
    //Server validiert Versionsnummer
    myread(sock, buffer);

    //Client schickt Game ID des gewünschten Spiels
    mywrite(sock,gameId);
    //Server schickt Spieletyp (Bashni)
    myread(sock, buffer);
    //Gamename einlesen und in game struct abspeichern
    sscanf(myread(sock, buffer), "+ %[^\n]", current_game->name);

    //Client schickt Nummer des Spielers, den er übernhemen möchte
    mywrite(sock, playerNr);
    
    //Server schickt die eigene Mitspielernummer + Name
    sscanf(myread(sock, buffer), "+ YOU %i", &current_game->player_number);
    
    //Server schickt die Mitgliederanzahl
    sscanf(myread(sock, buffer), "+ TOTAL %i", &current_game->player_count);
    
    //Gegner abspeichern
    int a = 0;
    struct player enemies[current_game->player_count];
    while(a < (current_game->player_count - 1)){
        //Player info lesen
        char* enemy = myread(sock, buffer);

        //Gegner Spielernummer abspeichern
        enemies[a].number = atoi(enemy+2);

        //Gegner Spielername abspeichern
        int enemy_length = strlen(enemy);
        enemy += 4;
        //enemy_length -4 (wegen Spielernummer '+ 1 ' am Anfang) und -2 (wegen Bereit flag ' 1' am Ende)
        for (int i = 0; i < (enemy_length - 4 - 2); i++) {
            enemies[a].name[i] = *enemy;
            enemy++;
        }
        
        //Gegner Bereit abspeichern
        enemies[a].registered = atoi(enemy+1);
        
        a++;
    }

    //Ende der Gegner
    myread(sock, buffer);
    free(buffer);
}

char* myread(int *sock, char* buffer) {
    //Erstellt char Speicher mit Größe BUF zum Lesen vom Server
    int i=0;
    char current;
    recv(*sock, &current, 1, 0);
    //liest Nachricht in einzelnen char ein
    while (current != '\n') {
        buffer[i++] = current;
        recv(*sock, &current, 1, 0);
    };
    buffer[i] = '\0';
    
    //beruecksichtigt moegliche fehler
    if (buffer[0] == '-'){
        printf("\n============================\n");
        printf("Es gab ein Problem 😭 (Der Server gibt einen Fehler aus)...\n");
        printf("Nachricht: '%s'\n", buffer);
        printf("============================\n\n");
        exit(0);
    } else {
        printf("🍕S: %s\n", buffer);
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
