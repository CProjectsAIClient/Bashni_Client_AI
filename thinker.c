#include "thinker.h"
#include <stdio.h>
#include <stdlib.h>

void startThinker() {

}

void printfield(char print[9][9][13]) {
  printf("   A B C D E F G H\n");
  printf(" +-----------------+\n");

  for (int i=8; i>0; i--) {
    printf("%i| ", i);
    for (int j=1; j<=8; j++) {
      if (print[i][j-1][0] == 'b' || print[i][j-1][0] == 'w') {
        printf("%c ", print[i][j-1][0]);
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
            if (print[i][z][0] == 'w') {
                count = i + 65;
                printf("%c%i: ", count, z+1);
                for(int j=0; j<13; j++) {
                    if (print[i][z][j] == 'w') {
                        printf("%c", print[i][z][j]);
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
            if (print[i][z][0] == 'b') {
                count = i + 65;
                printf("%c%i: ", count, z+1);
                for(int j=0; j<13; j++) {
                    if (print[i][z][j] == 'b') {
                        printf("%c", print[i][z][j]);
                    }
                }
                printf("\n");
            }
        }
    }
    printf("\n");
}
