#include "thinker.h"
#include "performConnection.h"
#include "random_ki.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>

//ruft think auf
void signal_handler(int signal_key);
//Berechnet Zug
void think();
//Sendet berechnete Zug Daten über die Pipe zum Connector
void sendToConnector(char* message);
//Setzt die Brett Matrix zurück
void reinitialize_brett_with_null();
//Speichert die Daten eines Spielsteins in der Brett Matrix
void save_brett_in_matrix(char color, int column, int row);

void* shmdata;
void *shmThinkerdata;
int pipe_fd;
int ok = 1;
int turn = 0;

void startThinker(void * shmdata1, int pipe) {
    shmdata = shmdata1;
    pipe_fd = pipe;
    
    //Signal Handler registrieren
    signal(SIGUSR1, signal_handler);
}

void think() {
    printf("Berechne Spielzug...");
    getMove(my_brett);
}

void sendToConnector(char* message) {
    int length = strlen(message);

    if (write(pipe_fd, message, length) != length) {
        perror("Fehler bei write() in pipe");
        exit (-2);
    }
}

void signal_handler(int signal_key) {
    struct game* current_game = shmdata;
    
    //SHM für Spielbrett
    shmThinkerdata = shmat(current_game->shmFieldID,NULL,0);
    if(shmThinkerdata == (void *) -1) { //(char *)-1
        printf("Fehler beim Anbinden des SHM fuer das Feld im Thinker\n");
        exit(-3);
    }
    printf("shmat im Thinker funktioniert\n");

    memcpy(current_game, shmdata, sizeof(game));

    printf("PiecesCount: %i, ", current_game->pieces_count);
    printf("Gamename: %s, ", current_game->name);
    printf("Playernummer: %d, ", current_game->player_number);
    printf("Playeranzahl: %d, ", current_game->player_count);
    printf("ThinkerID: %i, ", current_game->thinkerID);
    printf("ConnectorID: %d\n\n", current_game->connectorID);
     
    int anzahlSteine = current_game->pieces_count;
    printf("AnzahlSteine: %i\n", anzahlSteine);

    char currentBrett[anzahlSteine][5];
    memcpy(currentBrett, shmThinkerdata, sizeof(char) * anzahlSteine * 5);


    
    reinitialize_brett_with_null();

    int i = 0;
    while(anzahlSteine > 0){
        //printf("Going through Stein %i, with value: %s\n", i, currentBrett[i]);
        
        save_brett_in_matrix(currentBrett[i][0], currentBrett[i][2] - 'A' + 1, currentBrett[i][3] - '0');

        anzahlSteine--; 
        i++;
    }
    
    //Spielbrett ausgeben
    printf("Printing Brett (Turn: %i)...\n", turn);
    printfield(my_brett);// kommt in thinker

    //start komplizierte KI Berechnung
    printf("flag: %i\n", current_game->flag);
    if ((current_game->flag) == 1){
        initialize_random_ki(current_game);
        think();
    }
    else{
        //GAME OVER
        printf("flag ist %i, gell?\n", current_game->flag);
        exit(10);
    }
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
    int i = 0, check = 0;
    while(my_brett[row][column][i] == 'b' || my_brett[row][column][i] == 'w' || my_brett[row][column][i] == 'B' || my_brett[row][column][i] == 'W'){
        i++;
        check = 1;
    }
    if(check == 1)
        for(int k=i-1;k>=0;k--){
            my_brett[row][column][k + 1] = my_brett[row][column][k];
        }

    my_brett[row][column][0] = color;

}

void printfield(char print[9][9][13]) {

    turn++;
    printf("   A B C D E F G H\n");
    printf(" +-----------------+\n");

    for (int i=8; i>0; i--) {
        printf("%i| ", i);

        for (int j=1; j<=8; j++) {
            if (print[i][j][0] != '-') {
                printf("%c ", print[i][j][0]);
            } else {
                printf("_ ");
            }
        }
        printf("|%i\n", i);
    }

    printf(" +-----------------+\n");
    printf("   A B C D E F G H\n\n");

  

    printf("White Pieces\n=======================\n");
    char count;
    for(int i=1; i<=8; i++) {
        for(int z=1; z<=8; z++) {
            if (print[z][i][0] == 'w' || print[z][i][0] == 'W') {
                count = i + 64;
                printf("%c%i: ", count, z);
                for(int j=12; j>=0; j--) {
                    if (print[z][i][j] != '-') {
                        printf("%c", print[z][i][j]);
                    }
                }
                printf("\n");
            }
        }
    }
    printf("\n");
    printf("Black Pieces\n=======================\n");
    for(int i=1; i<=8; i++) {
        for(int z=1; z<=8; z++) {
            if (print[z][i][0] == 'b' || print[z][i][0] == 'B') {
                count = i + 64;
                printf("%c%i: ", count, z);
                for(int j=12; j>=0; j--) {
                    if (print[z][i][j] != '-') {
                        printf("%c", print[z][i][j]);
                    }
                }
                printf("\n");
            }
        }
    }
    printf("\n");
}

//39gtsqqzfypr5