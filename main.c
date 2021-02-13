#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>

#include "performConnection.h"
#include "thinker.h"
#include "connector.h"
#include "config.h"

void printWelcome();

int main(int argc, char *argv[]) {
    //variablen fuer konsolenparameter
    char *gameid = NULL;
    int playerid = 0;
    //konfig ist client.conf, wenn es vom Client nicht anders präzisiert wird
    char *konfig = "client.conf";
    //int gameid_gegeben = 0;
    //einlesen der Konsolenwerte mit getopt fuer eingaben -g gameid -p player und -c fuer die konfigurationsdatei
    int c;
    while ((c = getopt(argc,argv, "g:p:c:")) != -1){
        switch(c){
            case 'g':
                gameid = optarg;
                break;

            case 'p':
                playerid = atoi(optarg) - 1;
                break;

            case 'c':
                konfig = optarg;
                break;

            case ':':
                printf("Wert fehlt fuer g, c oder p❤!\n");
                return EXIT_FAILURE;

            case '?':
                printf("\nID, gewuenschte Spielernummer oder config name fehlt.\n\n👽 unidentified lifeform instead of or in -g or -p... pls identify yourself 👽\n\n");
                break;

            default:
                printf("Irgendwas laeuft schief bei getopt!\n");
                return EXIT_FAILURE;
        }
    }
    game_config game_conf;
    game_conf.hostname   = "sysprak.priv.lab.nm.ifi.lmu.de";
    game_conf.portnumber = 1357;
    game_conf.gametype   = "Bashni";
    
    printWelcome();

    game_conf = parse_config(konfig);
    printf("Hostname: %s, ", game_conf.hostname);
    printf("Port: %d, ", game_conf.portnumber);
    printf("Type: %s \n", game_conf.gametype);

    //filedeskriptor erstellen
    int* sock = malloc(sizeof(int));
    *sock = makeConnection(game_conf);
   
    //Erstellen eines SHM-Bereichs
    int memory_id = shmget(IPC_PRIVATE, sizeof(game), IPC_CREAT | 0666);
    if (memory_id == -1) {
        printf("Fehler beim Erstellen des SHM\n");
        exit(-2);
    }

    //Erstellen der Pipe
    int pipe_fd[2];
    if (pipe(pipe_fd) < 0) {
        perror("Fehler beim Erstellen der Pipe");
        exit(-2);
    }

    //Erstellen eines weiteren Prozesses
    pid_t pid;
    pid = fork();
    
    //SHM Anbinden in beiden P
    void *shmdata = shmat(memory_id,NULL,0);
    if(shmdata == (void *) -1) { //(char *)-1
        printf("Fehler beim Anbinden des SHM\n");
        exit(-3);
    }

    if (pid < 0) {
        //Error by creating the childprocess
        fprintf(stderr, "Fehler bei fork()\n"); 
    } else if (pid == 0){
        /*
         * Childprocess 
         * (Connector process)
         */

        //Pipe Schreibseite schließen
        close(pipe_fd[1]);

        //Game Struktur initialisieren
        game *current_game = (game*) shmdata;
        current_game->connectorID = getpid();
        current_game->thinkerID   = getppid();
    
        //Prolog Phase
        doperformConnection(sock, gameid, playerid, shmdata);
        
        //Epoll für pipe und socket initialisieren
        startConnector(sock, &pipe_fd[0]);
        
        //Spielverlauf Phase + kommuniktion zwischen server und thinker herstellen
        doSpielVerlauf(sock, playerid, shmdata);
    } else {
        /*
         * Parentprocess 
         * (Thinker process)
         */

        //Pipe Leseseite schließen
        close(pipe_fd[0]);

        //Signal Handler initialisieren
        startThinker(shmdata, pipe_fd[1]);

        //Auf Connector Prozess warten, falls noch nicht terminiert
        waitpid(pid, NULL, 0);
    }

    shmdt(shmdata);

    free(game_conf.gametype);
    free(game_conf.hostname);
    free(sock);

    return EXIT_SUCCESS;
}

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
