#include "thinker.h"


void startThinker(int pipe_fd) {
    
}

void sendToConnector(int pipe_fd[], char* message) {
    int length = strlen(message);

    if (write(pipe_fd[1], message, length) != length) { //In Schreibseite schreiben
        perror("Fehler bei write()");
        exit (-2);
    }
}