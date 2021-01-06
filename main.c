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

void printWelcome();

int main(int argc, char *argv[]) {
    printWelcome();

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
                break;
            case 'p':
                playerid = atoi(optarg);
                break;
            case 'c':
                konfig = optarg;
                break;
            case ':':
                printf("Wert fehlt fuer g, c oder p❤!\n");
                break;
            case '?':
                printf("Falsches Argument oder Aehnliches...\n");
                break;
            default:
                printf("Irgendwas laeuft schief bei getopt!\n");
                break;
        }
    }

    game_config game_conf = parse_config(konfig);
    printf("Hostname: %s, ", game_conf.hostname);
    printf("Port: %d, ", game_conf.portnumber);
    printf("Type: %s \n", game_conf.gametype);

    //filedeskriptor erstellen
    int* sock = NULL;
    sock = malloc(sizeof(int));
    *sock = makeConnection(game_conf);


    int anzahl_Steine = 25;
    
   
    //Erstellen eines SHM-Bereichs
    int memory_id = shmget(IPC_PRIVATE, sizeof(game), IPC_CREAT | 0666);
    if (memory_id == -1) {
        printf("Fehler beim Erstellen des SHM\n");
        exit(-2);
    }
    printf("shmget funktioniert\n");

    //Erstellen der Pipe
    int pipe_fd[2];

    if (pipe(pipe_fd) < 0) {
        perror("Fehler beim Erstellen der Pipe");
        exit(-2);
    }


    //Erstellen eines weiteren Prozesses
    pid_t pid;
    pid = fork();
    
    void *shmdata = shmat(memory_id,NULL,0);

    if(shmdata == (void *) -1) { //(char *)-1
        printf("Fehler beim Anbinden des SHM\n");
        exit(-3);
    }
    printf("shmat funktioniert\n");

    if (pid < 0) {
        //Error by creating the childprocess
        fprintf(stderr, "Fehler bei fork()\n"); 
    } else if (pid == 0){
        //Childprocess
        //Connector process
        printf("\a\t--- 🧒 CHILD PROCESS [Connector]: ---\n\n");

        game *current_game = (game*) shmdata;
        current_game->connectorID = 0;
        current_game->thinkerID   = 1;

        //Pipe Schreibseite schließen
        close(pipe_fd[1]);
        
        struct player* enemies = malloc(sizeof(player));

        shmdata = (game*) shmdata;

        doperformConnection(sock, gameid, playerid, shmdata, enemies);
        startConnector(*sock, pipe_fd[0]);

        doSpielVerlauf(sock, playerid, shmdata, anzahl_Steine);


        printf("shmdata %s\n", (char * ) shmdata);//koennte gefaehrlich sein bei valgrind

        //shmdata = current_game;
        //current_game = shmdata;

        
        free(enemies);
        //free(current_game);
    
    } else {
        //Parentprocess
        //Thinker process
        printf("\a\t--- 👨 Father PROCESS [Thinker]: ---\n\n");

        //Pipe Leseseite schließen
        close(pipe_fd[0]);

        startThinker(shmdata, pipe_fd[1]);

        //warten auf kindprozess
        if ((waitpid(pid,NULL,0)) < 0){
            perror("Fehler beim Warten auf den Connector\n");
            //exit(EXIT_FAILURE);
        }
    }


    shmdt(shmdata);

    free(game_conf.gametype);
    free(game_conf.hostname);
    free(sock);
    
    return EXIT_SUCCESS;
}
//fancy :)
void printWelcome() {
    printf("\n /$$$$$$$                      /$$                 /$$          /$$$$$$  /$$ /$$                       /$$    ");
    printf("\n| $$__  $$                    | $$                |__/         /$$__  $$| $$|__/                      | $$    ");
    printf("\n| $$  \\ $$  /$$$$$$   /$$$$$$$| $$$$$$$  /$$$$$$$  /$$        | $$  \\__/| $$ /$$  /$$$$$$  /$$$$$$$  /$$$$$$  ");
    printf("\n| $$$$$$$  |____  $$ /$$_____/| $$__  $$| $$__  $$| $$ /$$$$$$| $$      | $$| $$ /$$__  $$| $$__  $$|_  $$_/  ");
    printf("\n| $$__  $$  /$$$$$$$|  $$$$$$ | $$  \\ $$| $$  \\ $$| $$|______/| $$      | $$| $$| $$$$$$$$| $$  \\ $$  | $$    ");
    printf("\n| $$  \\ $$ /$$__  $$ \\____  $$| $$  | $$| $$  | $$| $$        | $$    $$| $$| $$| $$_____/| $$  | $$  | $$ /$$");
    printf("\n| $$$$$$$/|  $$$$$$$ /$$$$$$$/| $$  | $$| $$  | $$| $$        |  $$$$$$/| $$| $$|  $$$$$$$| $$  | $$  |  $$$$/");
    printf("\n|_______/  \\_______/|_______/ |__/  |__/|__/  |__/|__/         \\______/ |__/|__/ \\_______/|__/  |__/   \\___/  \n\n");
}

//3vhxf2pg6851t