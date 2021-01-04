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

void save_brett_in_matrix(char color, int column, int row);
void reinitialize_brett_with_null();

//werte der flags fuer game auslesen
int checkWait(char*buffer,int * sock);
int checkMove(char*buffer);
int checkGameover(char*buffer);
int checkQuit(char*buffer);

void doSpielVerlauf(int *sock, int player, struct game *current_game, int anzahl_Steine) {
    //Erstellen von epoll()
    int epoll_fd = epoll_create1(0);
    int running = 1, event_count, i;
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = 0;// hier vielleicht kommt die Pipe anstatt 0, wie unten in Zeile 165

    if(epoll_fd == -1) {
        fprintf(stderr, "Failed to create epoll file descriptor!/n");
        exit(-1);
    }
    //anstatt 0 kommt die Pipe
    int epoll_control = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event);
    if(epoll_control == -1){
        fprintf(stderr, "Failed to add fd to epoll!/n");
        exit(-1);
    }




    int anzahlSteine = 0, continuee = 1;
    i = 0;
    char *buffer = malloc(BUF * sizeof(char));
    int ok = 1;
    while(continuee){

        char *spiel_info = myread(sock, buffer);
        
        if (*spiel_info != '+') {
            printf("Fehler in der Spielverlauf Phase!");
            exit(0);
        }

        //Wait Befehlsequenz
        if(strncmp(spiel_info, "+ WAIT", 6) == 0){
            mywrite(sock, "OKWAIT");
            // //ich bin nicht sicher, ob man epoll() so implementiert und verwendet. Wir sollen uns das auch zusammen anschauen.
            // while(running){
            //     epoll_wait(epoll_fd, &event, sizeof(event), 3000);
            //     spiel_info = myread(sock, buffer);
            //     if(*spiel_info == -1){
            //         printf("HELLO thERE< FEhler!");
            //         exit(10);
            //     }
            //     break;
            // }
        }else if(strncmp(spiel_info, "+ MOVE", 6) == 0){
            spiel_info = myread(sock, buffer);

            //lese Anzahl an Steinen
            anzahlSteine = atoi(spiel_info + 13) + 1;
            anzahl_Steine = anzahlSteine;

            //hier soll ich noch das Brett in 2 Teile trennen: die Farbe und die Position. Das mache ich heute.
            char currentBrett[anzahlSteine + 1][5];
            i = 0;

            //spiel_brett auf 0 setzen
            reinitialize_brett_with_null();// eingentlich mit '-'

            //spiel_info = myread(sock, buffer);
            while(anzahlSteine > 0){
                spiel_info = myread(sock, buffer);
                strcpy(currentBrett[i], spiel_info + 2);
                save_brett_in_matrix(currentBrett[i][0], currentBrett[i][2] - 'A'+1, currentBrett[i][3] - '0');

                //an thinker schicken

                if (*spiel_info != '+') {
                    printf("Fehler in der Spielverlauf Phase!");
                    exit(0);
                    }
                anzahlSteine--; 
                i++;
            }

            //Spielbrett ausgeben
            printfield(my_brett);
            //die Positionen wurden gelesen, jetzt sollen wir sie an Thinker übergeben und den Zug berechnen.
            mywrite(sock, "THINKING");

    

            kill(1, SIGUSR1);
            
        } else if(strncmp(spiel_info, "+ GAMEOVER", 10) == 0) {
            //lese Anzahl an Steinen
            anzahlSteine = atoi(spiel_info + 13) + 1;
            anzahl_Steine = anzahlSteine;

            //hier soll ich noch das Brett in 2 Teile trennen: die Farbe und die Position. Das mache ich heute.
            char currentBrett[anzahlSteine + 1][5];
            i = 0;

            //lese die Steinpositionen und speichere sie
            while(anzahlSteine > 0){
                spiel_info = myread(sock, buffer);
                strcpy(currentBrett[i++], spiel_info + 2);
                if (*spiel_info != '+') {
                    printf("Fehler in der Spielverlauf Phase!");
                    exit(0);
                    }
                anzahlSteine--; 
            }


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

            // if the game has ended, end the while loop
            continuee = checkQuit(spiel_info);
        } else if(strncmp(spiel_info, "+ OKTHINK", 9) == 0){
            switch(ok){
                case 1: 
                    mywrite(sock, "PLAY G3:H4");
                    ok++;
                    break;
                case 2: 
                    mywrite(sock, "PLAY C3:D4");
                    ok++;
                    break;
                case 3:
                    mywrite(sock, "PLAY D4:B6");
                    ok++;
                    break;
                default:
                    break;

            }
            

            spiel_info = myread(sock, buffer);
        } else {
            printf("Fehler beim einlesen der Server Flags. Unbekannter Flag: '%s'", spiel_info);
        }
    }

    free(buffer);
}

void getPositions(game game1,struct player* enemies_list){

}

void sendPostitions(){
    
}

void reinitialize_brett_with_null(){
    for(int i=0;i<=8;i++){
        for(int j=0;j<=8;j++){
            for (int x=0;x<=12;x++){
                my_brett[i][j][x] = '-';
            }
        }
    }
}

void save_brett_in_matrix(char color, int column, int row){
    int i = 0;
    while(my_brett[row][column][i] == 'b' || my_brett[row][column][i] == 'w' || my_brett[row][column][i] == 'B' || my_brett[row][column][i] == 'W'){
        printf(" this is i:  %d  ", i);
        i++;
    }

    my_brett[row][column][i] = color;
}


//Testet ob die Nachricht wait ist und Antwortet dem Server falls ja + Rueckgabe der Flag
int checkWait(char*buffer,int * sock){
    char waitarr [] = "+ wait";
    char WaitarrGR [] = "+ WAIT";
    for (int i = 0; i <4; i++){
        if ( buffer[i]!= waitarr[i] && (buffer[i]!= WaitarrGR[i])){
            return 0;
        }
    }

    char * answerWait = "OKWAIT";
    mywrite(sock, answerWait);
    return 1;
}
//Testet ob die Nachricht move ist, Rueckgabe der Flag
int checkMove(char*buffer){
     char movearr [] = "+ move";
     char MOVEarrGR [] = "+ MOVE";
    for (int i = 0; i <6; i++){
        if ( (buffer[i]!= movearr[i]) && (buffer[i]!= MOVEarrGR[i])){
            return 0;
        } 
    }
    
    return 1;
}

//Testet ob die Nachricht gameover ist, Rueckgabe der Flag
int checkGameover(char*buffer){
    char Gameoverarr [] = "+ gameover";
     char GameoverarrGR [] = "+ GAMEOVER";
    for (int i = 0; i < 10; i++){
        if ( (buffer[i]!= Gameoverarr[i]) && (buffer[i]!= GameoverarrGR[i])){
            return 0;
        } 
    }
    return 1;
}


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
