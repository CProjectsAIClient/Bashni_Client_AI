#include <stdio.h>
#include <performConnection.h>

#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#define GAMEKINDNAME "Bashni"
#define PORTNUMBER 1357
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de" 

/* get opt machen methode soll kommandozeilenparameter aus main benutzen*/
void getConnectInfo(int argc, char* argv[]){
    char *playid =NULL;//vlt int
    playid = calloc(13,sizeof(char));//evtl auch 14 bei testen probieren...
    int player = 0;
    int c;
    
    while ((c = getopt(argc,argv, "g:p:")) != -1){
        switch(c){
            case 'g':
                playid = optarg;
                break;
            case 'p':
                player = atoi(optarg);
                break;
            case ':':
                printf("Wert fehlt fuer g oder p");
                break;
            case '?':
                printf("Falsches Argument oder Aehnliches...");
                break;
            default:
                printf("Irgendwas laeuft schief bei getopt");
                break;
        }
    }

    printf("playid=%s, player:%i", *playid, player);
}