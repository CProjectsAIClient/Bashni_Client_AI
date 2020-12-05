#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define GAMEKINDNAME "Bashni"
#define PORTNUMBER 1357
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de" 

/* get opt machen methode soll kommandozeilenparameter aus main benutzen*/
void getConnectInfo(void){
    char *playid =NULL;//vlt int
    playid = calloc(13,sizeof(char));//evtl auch 14 bei testen probieren...
    int player = 0;
    int c;
    while (c = getopt(argc,argv, "g:p:")){
        switch(c){
            case 'g':
            playid = optarg;
            break;
            case 'p':
            player = atoi(optarg);
            break;
            case ':':
            printf("wert fehlt fuer g oder p");
            break;
            case '?':
            printf("falsches argument oder aehnliches...");
            break;
            default:
            printf("irgendwas laeuft schief bei getopt");
            break;
        }
    }
}