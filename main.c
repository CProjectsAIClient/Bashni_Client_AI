#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include "performConnection.h"
#include "thinker.h"
#include "connector.h"
#include "config.h"
#include "config.h"

int main(int argc, char *argv[]) {
    //variablen fuer konsolenparameter
    char *gameid = NULL;
    int playerid = 0;
    //konfig ist client.conf, wenn es vom Client nicht anders präzisiert wird
    char *konfig = "client.conf";
    
    //einlesen der Konsolenwerte mit getopt fuer eingaben -g gameid -p player und -c fuer die konfigurationsdatei
    int c;
    while ((c = getopt(argc,argv, "g:p:c:")) != -1){
        switch(c){
            case 'g':
                gameid = optarg;
                printf("reading g in getopt\n");
                break;
            case 'p':
                playerid = atoi(optarg);
                printf("reading p in getopt\n");
                break;
            case 'c':
                konfig = optarg;
                printf("reading c in getopt\n");
                break;
            case ':':
                printf("Wert fehlt fuer g oder p\n");
                break;
            case '?':
                printf("Falsches Argument oder Aehnliches...\n");
                break;
            default:
                printf("Irgendwas laeuft schief bei getopt\n");
                break;
        }
    }

    game_config game_conf = parse_config(konfig);
    printf("Hostname: %s, ", game_conf.hostname);
    printf("Port: %d, ", game_conf.portnumber);
    printf("Type: %s \n", game_conf.gametype);

    //filedeskriptor erstellen
    int* sock = NULL;
    sock = calloc(1, sizeof(int));
    *sock = makeConnection(game_conf);

    game *current_game = malloc(sizeof(game));
    current_game->connectorID = 0;
    current_game->thinkerID   = 1;

    
   
    //Erstellen eines SHM-Bereichs
    int memory_id = shmget(IPC_PRIVATE, sizeof(game), IPC_CREAT | 0666);
    if(memory_id == -1){
        printf("Fehler beim Erstellen des SHM\n");
        exit(-2);
    }printf("shmget funktioniert\n");

    //Erstellen eines weiteren Prozesses
    pid_t pid;
    pid = fork();
    
    void *shmdata = shmat(memory_id,NULL,0);
    if(shmdata == (void *) -1){ //(char *)-1
        printf("Fehler beim Anbinden des SHM\n");
        exit(-3);
    }printf("shmat funktioniert\n");
    
    if(pid < 0){
        //Error by creating the childprocess
        fprintf(stderr, "Fehler bei fork()\n"); 
    }
    else if (pid == 0){
        //Childprocess
        //Connector process
        printf("CHILD PROCESS!!\n\n");
        
        doperformConnection(sock, gameid, playerid, current_game);
        printf("Gamename: %s, ", current_game->name);
        printf("Playernummer: %d, ", current_game->player_number);
        printf("Playeranzahl: %d, ", current_game->player_count);
        printf("ThinkerID: %i, ", current_game->thinkerID);
        printf("ConnectorID: %d\n\n", current_game->connectorID);

        //*shmdata = *current_game;
        
        startConnector(sock);
    }
    else{
        //Parentprocess
        //Thinker process
        startThinker();

        //warten auf kindprozess
        if ((waitpid(pid,NULL,0)) < 0){
            perror("Fehler beim warten auf den Connector\n");
            exit(EXIT_FAILURE);
        }
        printf("Father PROCESS!!\n\n");
    }

    shmdt(shmdata);
    free(current_game);
    return EXIT_SUCCESS;
}