#include <stdio.h>
#include <stdlib.h>
#include "performConnection.h"
#include "thinker.h"
#include "connector.h"
#include "config.h"
#include "config.h"
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 


//test
int main(int argc, char* argv[]) {
    //variablen fuer konsolenparameter
    char *playid = NULL;
    //playid = calloc(14, sizeof(char));
    int player=0;
    //konfig ist client.conf, wenn es vom Client nicht anders präzisiert wird
    char defaultConfig[] = "client.conf";
    char *konfig = NULL;
    konfig = defaultConfig;
    printf("konfig %s\n", konfig);
    printf("reading bevor2 in getopt\n");
    //einlesen der Konsolenwerte mit getopt fuer eingaben -g gameid -p player und -c fuer die konfigurationsdatei
    int c;
    while ((c = getopt(argc,argv, "g:p:c:")) != -1){
        switch(c){
            case 'g':
                playid = optarg;
                printf("reading g in getopt\n");
                break;
            case 'p':
                player = atoi(optarg);
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

   //game_config game_conf;
    printf("konfig %s", konfig);
    if (konfig == NULL) {
        game_config game_conf = parse_config("client.conf");
        printf("Hostname: %s ,", game_conf.hostname);
    } else {
        // game_config game_conf = parse_config("client.conf");
        // printf("Hostname: %s ,", game_conf.hostname);
        // printf("Port: %d ,", game_conf.portnumber);
        // printf("Type: %s \n", game_conf.gametype);
    }

    //filedeskriptor erstellen
    int* sock = NULL;
    sock = calloc(1,sizeof(int));
    *sock = makeConnection();

    doperformConnection(sock, playid, player);

    //Erstellen eines weiteren Prozesses
    // pid_t pid;
    // pid = fork();

    // if(pid < 0){
    //     //Error by creating the childprocess
    //     fprintf(stderr, "Fehler bei fork()\n");
    // }
    // else if (pid == 0){
    //     //Childprocess
    //     //Connector process
    //     doperformConnection(sock, playid, player);
        
    // }
    // else{
    //     //Parentprocess
    //     //Thinker process

    //     //warten auf kindprozess
    //     if ((waitpid(pid,NULL,0)) < 0){
    //         perror("Fehler beim warten auf den Connector\n");
    //         exit(EXIT_FAILURE);
    //     }
    // }




    return EXIT_SUCCESS;
}