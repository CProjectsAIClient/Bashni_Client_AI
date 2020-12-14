#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//einlesen eines int-werts aus dem config-file
char *read_string(FILE *file, char const *desired_name);
//einlesen eines string (char*) aus dem config-file
int read_int(FILE *file, char const *desired_name, int *ret);

game_config parse_config(char *file_name) {
    printf("Opening file %s...\n", file_name);

    FILE *datei = fopen(file_name, "r");
    if(NULL == datei) {
        printf("Konnte Datei %s nicht öffnen!😢\n", file_name);
        datei = fopen("client.conf", "r");

        if(NULL != datei) {
            printf("Aber client.conf wurde geöffnet 🤞\n");
        } else {
            exit(-1);
        }
   }

    game_config game_conf;
    game_conf.hostname = read_string(datei, "hostname");

    int *port = malloc(sizeof(int));
    if (read_int(datei, "portnumber", port) != -1) {
        game_conf.portnumber = *port;
    } else {
        printf("Fehler beim einlesen des Ports aus der Config!");
    }
    free(port);

    game_conf.gametype = read_string(datei, "gametype");

    fclose(datei);
    return game_conf;
}


char* read_string(FILE *file, char const *desired_name) { 
    char name[128];
    char val[128];

    while (fscanf(file, "%127[^ = ] = %127[^\n]%*c", name, val) == 2) {
        if (0 == strcmp(name, desired_name)) {
            return strdup(val);
        }
    }
    return NULL;
}

int read_int(FILE *file, char const *desired_name, int *ret) {
    char *temp = read_string(file, desired_name);

    char *stop;
    *ret = strtol(temp, &stop, 10);
    int ret_val = stop == NULL || *stop != '\0';
    free(temp);
    return ret_val;
}