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

#define PIPE_BUF 128

void registerFd(int epoll_fd, int fd);
struct epoll_event waitForInput(int epoll_fd);

// epoll(): https://suchprogramming.com/epoll-in-3-easy-steps/
void startConnector(int fd_sock, int fd_pipe) {
    //Create Epoll instance
    int epoll_fd = epoll_create1(0);
    //Create Pipe read buffer
    char read_buffer[PIPE_BUF];

    if(epoll_fd == -1) {
        perror("Failed to create epoll file descriptor\n");
        exit(-1);
    }
    //Add socket File descriptor to epoll for listening
    registerFd(epoll_fd, fd_sock);
    //Add pipe File descriptor to epoll for listening
    registerFd(epoll_fd, fd_pipe);

    //Listening for incoming data
    struct epoll_event event = waitForInput(epoll_fd);

    //Checking from where data came
    if (event.data.fd == fd_sock) {
        //Data came from socket (GameServer)

        //myread
    } else if (event.data.fd == fd_pipe) {
        //Data came from pipe (Thinker)

        read(fd_pipe, read_buffer, PIPE_BUF);
        //mywrite to server
    }
    
    close(epoll_fd);
}

//werte der flags fuer game auslesen
int checkQuit(char*buffer);

void saveAndSendBrett(int* sock, void* shmAddress, int feldgr, struct game* current_game);

void doSpielVerlauf(int *sock, int player, struct game *current_game, int anzahl_Steine) {

    int anzahlSteine = 0, continuee = 1, i = 0;

    char *buffer = malloc(BUF * sizeof(char));
    char *spiel_info = malloc(BUF * sizeof(char));

    //Lesen zum ersten Mal:   + MOVE
    if (strncmp(myread(sock, buffer), "+ MOVE", 6) == 0) {
        printf("Move Geschwindigkeit wurde festgelegt\n");
    } else {
        printf("Fehler beim ersten + MOVE %s\n", buffer);
    }

    sscanf(myread(sock, buffer), "+ PIECESLIST %i", &current_game->pieces_count);
    printf("Connector Piecescount: %i\n", current_game->pieces_count);

    //SHM fuer Spielfeld erstellen
    current_game->shmFieldID = shmget(IPC_PRIVATE, sizeof(char) * (current_game->pieces_count) * 5, IPC_CREAT | 0666);

    void *shmConnectordata = shmat(current_game->shmFieldID,NULL,0);

    //memcpy(shmConnectorData, current_game, sizeof(game));

    if(shmConnectordata == (void *) -1) { //(char *)-1
        printf("Fehler beim Anbinden des SHM fuer das Feld\n");
        exit(-3);
    }
    printf("shmat im Connector funktioniert\n");

    
    saveAndSendBrett(sock, shmConnectordata, current_game->pieces_count, current_game);




    while(continuee){

        spiel_info = myread(sock, buffer);
        
        if (*spiel_info != '+') {
            printf("Fehler in der Spielverlauf Phase!");
            exit(0);
        }

        //Wait Befehlsequenz
        if(strncmp(spiel_info, "+ WAIT", 6) == 0){
            mywrite(sock, "OKWAIT");
            
        }
        else if(strncmp(spiel_info, "+ MOVE", 6) == 0){
            // spiel_info = myread(sock, buffer);

            //lese Anzahl an Steinen
            //anzahlSteine = atoi(spiel_info + 13) + 1;
            //anzahl_Steine = anzahlSteine;

            sscanf(myread(sock, buffer), "+ PIECESLIST %i", &current_game->pieces_count);

            current_game->flag = GAME_MOVE;//1

            //die Positionen wurden gelesen, jetzt sollen wir sie an Thinker übergeben und den Zug berechnen.
            saveAndSendBrett(sock,shmConnectordata,current_game->pieces_count, current_game);

            printf("\nprint10\n");

        } 
        else if(strncmp(spiel_info, "+ GAMEOVER", 10) == 0) {
            //lese Anzahl an Steinen
            anzahlSteine = atoi(spiel_info + 13) + 1;
            anzahl_Steine = anzahlSteine;

            //hier soll ich noch das Brett in 2 Teile trennen: die Farbe und die Position. Das mache ich heute.
            char currentBrett[anzahlSteine + 1][5];
            i = 0;

            //lese den Gewinner und erstell ein Array mit den Spielern und deren Status 
            int nr_spieler = current_game->player_count;
            char whoWonGame[nr_spieler + 1][20];

            i=0;
            while(nr_spieler > 0){
                spiel_info = myread(sock, buffer);
                strncpy(whoWonGame[i], spiel_info + 2, 7);
                if(*(spiel_info + 13) == 'Y'){
                    strcat(whoWonGame[i], " is the winner!"); 
                }
                else{
                    strcat(whoWonGame[i], " has lost!");    
                }
                i++;
                printf("%s/n", whoWonGame[i-1]);
            }

            //lese QUIT
            spiel_info = myread(sock, buffer);

            //current_game->flag = GAME_QUIT;

            // if the game has ended, end the while loop
            continuee = checkQuit(spiel_info);

            current_game->flag = continuee;//0
        } 
        else if(strncmp(spiel_info, "+ OKTHINK", 9) == 0){
            
        } 
        else {
            printf("Fehler beim einlesen der Server Flags. Unbekannter Flag: '%s'", spiel_info);
        }
    }

    free(buffer);
    shmdt(shmConnectordata);
}

void getPositions(game game1,struct player* enemies_list){

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
    printf("Piecescount vor memcpy %i", current_game->pieces_count);
    memcpy(shmAddress, currentBrett, sizeof(char) * feldgr_copy * 5);
    //memcpy(shmConnectorData, current_game, sizeof(game));
    
    
    //printfield(my_brett); kommt in thinker
    printf("\nprint5\n");
    //thinking schicken
    mywrite(sock, "THINKING");
    kill(getppid(), SIGUSR1);  
    printf("\nprint6\n");
    free(buffer);
}

void sendPostitions(){
    
}




//Testet ob die Nachricht quit ist, Rueckgabe der Flag
int checkQuit(char*buffer){
     char quitarr [] = "+ quit";
     char QUITarrGR [] = "+ QUIT";
    for (int i = 0; i <6; i++){
        if ( (buffer[i]!= quitarr[i]) && (buffer[i]!= QUITarrGR[i])){
            return 1;
        } 
    }
    return 0;

    //return strcmp(buffer, "+ QUIT");
}

//1jdk61fquwhw3
void registerFd(int epoll_fd, int fd) {
    //Create Epoll event for listening to incoming data on file descriptor fd
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;

    //Add event to epoll instance
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event)) {
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
