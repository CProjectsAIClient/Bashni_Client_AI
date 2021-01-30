#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//header fuer socket
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "connector.h"
#include "performConnection.h"
#include "thinker.h"

//File deskriptor für epoll registrieren
void registerFd(int epoll_fd, int fd);
//Warten auf input von pipe und socket
struct epoll_event waitForInput(int epoll_fd);
//Brett in SHM speichern und signal an thinker senden
void saveAndSendBrett(int* sock, void* shmAddress, int feldgr, struct game* current_game);
//fkt fuer gameover
void onGameover(int* sock, game* current_game, char* buffer);

int epoll_fd, pipe_fd;

// epoll(): https://suchprogramming.com/epoll-in-3-easy-steps/
void startConnector(int fd_sock, int fd_pipe) {
    //Create Epoll instance
    epoll_fd = epoll_create1(0);
    pipe_fd = fd_pipe; 
    //Create Pipe read buffer

    if(epoll_fd == -1) {
        perror("Failed to create epoll file descriptor\n");
        exit(-1);
    }

    //Add socket File descriptor to epoll for listening
    registerFd(epoll_fd, fd_sock);
    //Add pipe File descriptor to epoll for listening
    registerFd(epoll_fd, fd_pipe);
}

void doSpielVerlauf(int *sock, int player, struct game *current_game) {
    int continue_run = 1, i = 0;
    char *buffer = malloc(BUF * sizeof(char));
    
    //Lesen zum ersten Mal:   + MOVE
    myread(sock, buffer);
    if (strncmp(buffer, "+ MOVE", 6) == 0) {
        printf("Move Geschwindigkeit wurde festgelegt\n");
        current_game->flag = 1;
    } else if (strcmp(buffer, "+ GAMEOVER") == 0) {
        current_game->flag = 0;

        onGameover(sock, current_game, buffer);
        free(buffer);
        return;
    } else {
        printf("Warten auf Spielzug des Gegners...\n");

        while (strcmp(buffer, "+ WAIT") == 0){
            mywrite(sock, "OKWAIT");
            myread(sock, buffer);
        }
    }

    //Spielstein Anzahl lesen
    sscanf(myread(sock, buffer), "+ PIECESLIST %i", &current_game->pieces_count);

    //SHM fuer Spielfeld erstellen
    current_game->shmFieldID = shmget(IPC_PRIVATE, sizeof(char) * (current_game->pieces_count) * 5, IPC_CREAT | 0666);

    //Spielfeld SHM Anbinden
    void *shmConnectordata = shmat(current_game->shmFieldID,NULL,0);
    if(shmConnectordata == (void *) -1) { //(char *)-1
        printf("Fehler beim Anbinden des SHM fuer das Feld\n");
        exit(-3);
    }   
    
    current_game->flag = continue_run;//1
    saveAndSendBrett(sock, shmConnectordata, current_game->pieces_count, current_game);


    while(continue_run){
        myread(sock, buffer);

        //Wait Befehlsequenz
        if(strcmp(buffer, "+ WAIT") == 0){
            mywrite(sock, "OKWAIT");
        }
        else if(strncmp(buffer, "+ MOVE", 6) == 0){
            //Anzahl der Spielsteine lesen
            sscanf(myread(sock, buffer), "+ PIECESLIST %i", &current_game->pieces_count);

            current_game->flag = continue_run;//1
            printf("wert von continue_run %i\n", continue_run);
            printf("wert der flag %i\n", current_game->flag);

            //die Positionen wurden gelesen, jetzt sollen wir sie an Thinker übergeben und den Zug berechnen.
            saveAndSendBrett(sock,shmConnectordata, current_game->pieces_count, current_game);
        }
        //ACHTUNG auf Gameover, wenn wir da ankommen
        else if(strcmp(buffer, "+ GAMEOVER") == 0) {
            onGameover(sock, current_game, buffer);

            break;
        } 
        else if(strcmp(buffer, "+ OKTHINK") == 0){
            //Listening for incoming data
            struct epoll_event event = waitForInput(epoll_fd);
            
            //Checking from where data came
            if (event.data.fd == *sock) {
                //Data came from socket (GameServer)
                continue;
            }
            if (event.data.fd == pipe_fd) {
                char* read_buffer = calloc(BUF , sizeof(char));
                //Data came from pipe (Thinker)
                read(pipe_fd, read_buffer, BUF);
                
                //write PLAY to Server
                mywrite(sock, read_buffer);
                //read + MOVEOK
                myread(sock, buffer);
                
                free(read_buffer);
            }
        } 
        else {
            printf("Fehler beim einlesen der Server Flags. Unbekannter Flag: '%s'\n", buffer);\
            break;
        }
        
    }

    free(buffer);
    shmdt(shmConnectordata);
}

void saveAndSendBrett(int* sock, void* shmAddress, int feldgr, struct game * current_game) {
    char* buffer = malloc(BUF * sizeof(char));
    printf("\nprint2\n");
    char currentBrett[feldgr][5];
    //lese die 24 Steine
    printf("\n%i felder total\n", feldgr);
    int i = 0;
    int feldgr_copy = feldgr;
    while(feldgr>0){
        myread(sock, buffer);
        strcpy(currentBrett[i], buffer + 2);
        i++;
        feldgr--;
    }
    //lese ENDPIECELIST
    myread(sock, buffer);

    //currentBrett in SHM schreiben
    memcpy(shmAddress, currentBrett, sizeof(char) * feldgr_copy * 5);

    //thinking schicken
    mywrite(sock, "THINKING");
    kill(getppid(), SIGUSR1);  

    free(buffer);
}

void registerFd(int epoll_fd, int fd) {
    //Create Epoll event for listening to incoming data on file descriptor fd
    struct epoll_event ev = {
        .events = EPOLLIN,
        .data.fd = fd
    };

    //Add event to epoll instance
    int x = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    if(x == -1) {
        perror("Failed to add file descriptor to epoll\n");
        close(epoll_fd);
        exit(-1);
    }
}

struct epoll_event waitForInput(int epoll_fd) {
    struct epoll_event events[1];
    // Wait for incoming data (1 event) on all registered file descriptors with a 20 seconds timeout
    int event_count = epoll_wait(epoll_fd, events, 1, 20000);

    //Check if event count is greater than 0. Otherwise the timout was exceeded which means an error
    if (event_count > 0) {
        return events[0];
    } else {
        perror("Timeout of 20 seconds from epoll reached");
        exit(-3);
    }
}

void onGameover(int* sock, struct game* current_game, char* buffer) {

    //Anzahl der Spielsteine lesen
    sscanf(myread(sock, buffer), "+ PIECESLIST %i", &current_game->pieces_count);

    int i = current_game->pieces_count;
    while(i>0){
        myread(sock, buffer);
        i--;
    }
    //End pieces list
    myread(sock, buffer);

    current_game->flag = 0;
    //saveAndSendBrett(sock, shmConnectordata, current_game->pieces_count, current_game);

    //lese den Gewinner und erstell ein Array mit den Spielern und deren Status 
    int nr_spieler = current_game->player_count;
    
    
    
    i=0;
    while(nr_spieler > 0){
        char whoWonGame[nr_spieler + 1][40];
        buffer = myread(sock, buffer);
        strncpy(whoWonGame[i], buffer + 2, 7);
        whoWonGame[i][6]++;
        whoWonGame[i][7] = '\0';
        if(*(buffer + 13) == 'Y'){
            strcat(whoWonGame[i], " is the winner!\0"); 
        }
        else{
            strcat(whoWonGame[i], " has lost!\0");    
        }
        i++;
        printf("%s\n", whoWonGame[i-1]);
        nr_spieler--;
        
    }

    //Quit
    myread(sock, buffer);
    //free(buffer);
    //char player0Won[BUF] = sscanf(buffer+13, "");

    // if the game has ended, end the while loop
    //continue_run = (strcmp(myread(sock, buffer), "+ QUIT") == 0);
    //continue_run = 0;
}

//2rrsbr7jnmnjv