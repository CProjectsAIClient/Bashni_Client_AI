#include <stdio.h>
#include "performConnection.h"

#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#define GAMEKINDNAME "Bashni"
#define PORTNUMBER 1357
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de" 

/* get opt machen methode soll kommandozeilenparameter aus main benutzen*/
void getConnectInfo(char *gameid, int player) {
    printf("playid=%s, player:%i", gameid, player);
}