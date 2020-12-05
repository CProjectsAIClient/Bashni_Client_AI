#include <stdio.h>
#include <stdlib.h>
#include "performConnection.h"
#include <getopt.h>
#include <string.h>

int main(int argc, char* argv[]) {
    printf("Hello World\n");

    char *playid;
    int player;
    int c;
    
    while ((c = getopt(argc,argv, "g:p:")) != -1){
        switch(c){
            case 'g':
                playid = optarg;
                break;
            case 'p':
                player = atoi(optarg);
                // if(player != 1 && player != 2 && player != 0) {
                //     perror("Bitte maximal 2 Players einfuegen.\n\n");
                //     return 1;
                // }
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

    getConnectInfo(playid, player);




    return EXIT_SUCCESS;
}