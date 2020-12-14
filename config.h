#ifndef config
#define config

#define GAMEKINDNAME "Bashni"
#define PORTNUMBER 1357
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de" 

typedef struct game_config {
    char *hostname;
    int portnumber;
    char *gametype;
} game_config;

game_config parse_config(char *file_name);

#endif