#include "thinker.h"
#include "performConnection.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/shm.h>
#include <string.h>

void signal_handler(int signal_key);

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

    //reinitialize_brett_with_null();
    //save_brett_in_matrix(currentBrett[i][0], currentBrett[i][2] - 'A'+1, currentBrett[i][3] - '0');
    //
}

void* shmdata;
void *shmThinkerdata;

void startThinker(void * shmdata1) {
    // printf("Starting Thinker...\n");

    // struct game* current_game;
    // current_game = shmdata1;
    
    // memcpy(current_game, shmdata1, sizeof(game));
    // shmThinkerdata = shmat(current_game->shmFieldID,NULL,0);

    // if(shmThinkerdata == (void *) -1) { //(char *)-1
    //     printf("Fehler beim Anbinden des SHM fuer das Feld im Thinker\n");
    //     exit(-3);
    // }
    // printf("shmat im Thinker funktioniert\n");


    shmdata = shmdata1;
    printf("\nprint1\n");

    signal(SIGUSR1, signal_handler);

    printf("\nprint2\n");

}


void signal_handler(int signal_key) {
    printf("\n\n\n\nincoming signal %i \n\n\n\n", signal_key);  
    struct game* current_game = shmdata;
    
    //memcpy(current_game, shmdata1, sizeof(game));
    shmThinkerdata = shmat(current_game->shmFieldID,NULL,0);

    if(shmThinkerdata == (void *) -1) { //(char *)-1
        printf("Fehler beim Anbinden des SHM fuer das Feld im Thinker\n");
        exit(-3);
    }
    printf("shmat im Thinker funktioniert\n");


    //shmdata = shmdata1;


    //struct game* current_game;
    memcpy(current_game, shmdata, sizeof(game));

    printf("PiecesCount: %i\n", current_game->pieces_count);
    printf("Gamename: %s, ", current_game->name);
    printf("Playernummer: %d, ", current_game->player_number);
    printf("Playeranzahl: %d, ", current_game->player_count);
    printf("ThinkerID: %i, ", current_game->thinkerID);
    printf("ConnectorID: %d\n\n", current_game->connectorID);
    int i = 0;  

    //spiel_info = myread(sock, buffer);
    int anzahlSteine = current_game->pieces_count;
    printf("AnzahlSteine: %i\n", anzahlSteine);

    //char currentBrett[][]  = *shmThinkerData;
    char currentBrett[anzahlSteine][5];
    memcpy(currentBrett, shmThinkerdata, sizeof(char) * anzahlSteine * 5);



    while(anzahlSteine > 0){
        printf("Going through Stein %i\n", i);
        
        // spiel_info = myread(sock, buffer);
        // strcpy(currentBrett[i], spiel_info + 2);

        reinitialize_brett_with_null();// eingentlich mit '-'
        save_brett_in_matrix(currentBrett[i][0], currentBrett[i][2] - 'A' + 1, currentBrett[i][3] - '0');

        anzahlSteine--; 
        i++;
    }

    //Spielbrett ausgeben
    printf("Printing Brett...\n");
    printfield(my_brett);// kommt in thinker

    //start komplizierte KI Berechnung
    //read shm
}

void printfield(char print[9][9][13]) {
  printf("   A B C D E F G H\n");
  printf(" +-----------------+\n");

  for (int i=8; i>0; i--) {
    printf("%i| ", i);
    for (int j=1; j<=8; j++) {
      if (print[i][j][0] != '-') {
        printf("%c ", print[i][j][0]);
      }
      else {
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
            if (print[z][i][0] == 'w') {
                count = i + 64;
                printf("%c%i: ", count, z);
                for(int j=0; j<13; j++) {
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
            if (print[z][i][0] == 'b') {
                count = i + 64;
                printf("%c%i: ", count, z);
                for(int j=0; j<13; j++) {
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