#include "thinker.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void signal_handler(int signal_key);

void startThinker() {
  printf("\nprint1\n");
  struct sigaction sa = {0};
  sa.sa_handler = &signal_handler;
  sigaction(SIGUSR1, &sa, NULL);
  printf("\nprint2\n");

}

void signal_handler(int signal_key) {
  
  printf("\n\n\n\nincoming signal  \n\n\n\n");
  
}

void printfield(char print[9][9][13]) {
  printf("   A B C D E F G H\n");
  printf(" +-----------------+\n");

  for (int i=8; i>0; i--) {
    printf("%i| ", i);
    for (int j=1; j<=8; j++) {
      if (print[i][j][0] != '-') {
        printf("%c ", print[i][j][0]);
      }
      else {
        printf("_ ");
      }
    }
    printf("|%i\n", i);
  }

  printf(" +-----------------+\n");
  printf("   A B C D E F G H\n\n");

  

  printf("White Pieces\n=======================\n");
    char count;
    for(int i=1; i<=8; i++) {
        for(int z=1; z<=8; z++) {
            if (print[z][i][0] == 'w') {
                count = i + 64;
                printf("%c%i: ", count, z);
                for(int j=0; j<13; j++) {
                    if (print[z][i][j] != '-') {
                        printf("%c", print[z][i][j]);
                    }
                }
                printf("\n");
            }
        }
    }
    printf("\n");
  printf("Black Pieces\n=======================\n");
    for(int i=1; i<=8; i++) {
        for(int z=1; z<=8; z++) {
            if (print[z][i][0] == 'b') {
                count = i + 64;
                printf("%c%i: ", count, z);
                for(int j=0; j<13; j++) {
                    if (print[z][i][j] != '-') {
                        printf("%c", print[z][i][j]);
                    }
                }
                printf("\n");
            }
        }
    }
    printf("\n");
}