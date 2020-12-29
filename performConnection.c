    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    //header fuer socket
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <stdbool.h>
    #include <sys/epoll.h>

    #include "performConnection.h"
    #include "config.h"

    #define BUF 1024

    //liest werte vom Server
    char* myread(int *sock, char *buffer);

    void mywrite(int * sock, char *buffer);

    void save_brett_in_matrix(char color, int column, int row);

    int makeConnection(game_config game_conf){
        //socket anlegen
        int sock;
        if((sock = socket(AF_INET,SOCK_STREAM,0)) <= 0){
            printf("Socket Fehler: %d!\n", sock);
        }

        //verbindungsadresse eingeben
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(game_conf.portnumber);
    
        //Wandelt Hostname in Adresse um;
        struct hostent *hp;
        hp = gethostbyname(game_conf.hostname);
        if(hp == NULL) {
            fprintf(stderr,"%s unknown host.\n",game_conf.hostname);
            exit(2);
        }
        // copies the internet address to server address
        bcopy(hp->h_addr_list[0], &addr.sin_addr, hp->h_length);

        int connected;
        //Verbindung aufbauen
        printf("Verbinde zum Gameserver...\n");
        if((connected = connect(sock, (struct sockaddr*) &addr, sizeof(addr))) == 0){
            printf("Verbindung aufgebaut!\n");
        } else {
            printf("Verbindungsfehler: %i\n", connected);
        }

        return sock;
    }

    void doperformConnection(int *sock, char gameid[], int player, game *current_game, struct player* enemy_list){
        //printf("Chat\n\n\n");
        char *buffer; // = (char*) malloc(sizeof(char) * BUF);
        ssize_t size;

        //Ausgaben des Client in der Kommunikation mit dem Server
        //Ausgabe der GameID des Client
        char gameId[18];//14 fuer gameID und 3 fuer "ID " und 1 fuer \n
        sprintf(gameId, "ID %s", gameid);
        //printf("GameID: %s\n", gameId);

        //Ausgeben der Player ID fuer den Server
        char playerNr[10];
        sprintf(playerNr, "PLAYER %d", player);
        //printf("PlayerID: %s\n\n\n\n", playerNr);

        //client wird nach Version gefragt + rueckgabe der Version
        myread(sock, buffer);
        mywrite(sock,"VERSION 2.3");

        //Client wird nach SpielID gefragt + rueckgabe
        myread(sock, buffer);
        mywrite(sock,gameId);
        

        //Client wird nach gewuenschter Spielernummer gefragt + Antwort
        myread(sock, buffer);
        char *game_name = myread(sock, buffer);

        //Name finden und speichern
        int j = 0;
        char current2;
        game_name += 2;
        current2 = *game_name;
        while (current2 != '\n') {
            current_game->name[j++] = current2;
            game_name++;
            current2 = *game_name;
        } 

        mywrite(sock, playerNr);
        
        //Server schickt die eigene Mitspielernummer + Name
        char * current_player = myread(sock, buffer);
        current_game->player_number = atoi(current_player+5);
        

        //Server schickt die Mitgliederanzahl
        char *total = myread(sock, buffer);
        int count = atoi(total+8);
        current_game->player_count = count;
        
        int a = 0;
        struct player enemies[count];
        while(a < count - 1){
            //Player info lesen
            char* enemy = myread(sock, buffer);
            enemies[a].number = atoi(enemy+2);
            
            //Name2 array definieren und enemy auf den ersten Buchstabe setzen
            //char name2[BUF];
            int i = 0;
            enemy += 4;
            
            //Name finden und speichern
            char current;
            current = *enemy;
            while (current != ' ') {
                enemies[a].name[i++] = current;
                enemy++;
                current = *enemy;
            } 
            
            //Name der Struct zuweisen
            //enemies[a].name = name2;
            enemies[a].registered = atoi(enemy+1);
            
            a++;
        }

        enemy_list = enemies;

        //Fehlermeldungen
        char *end = myread(sock, buffer);
        if (*end != '+') {
            printf("Fehler in der Prolog Phase!");
            exit(0);
        }
        
        //free(total);    
    }



    void doSpielVerlauf(int *sock, char gameid[], int player, game *current_game, int anzahl_Steine){


        //Erstellen von epoll()
        int epoll_fd = epoll_create1(0);
        int running = 1, event_count, i;
        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = 0;// hier vielleicht kommt die Pipe anstatt 0, wie unten in Zeile 165

        if(epoll_fd == -1) {
            fprintf(stderr, "Failed to create epoll file descriptor!/n");
            exit(-1);
        }
        //anstatt 0 kommt die Pipe
        int epoll_control = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event);
        if(epoll_control == -1){
            fprintf(stderr, "Failed to add fd to epoll!/n");
            exit(-1);
        }




        int anzahlSteine = 0, continuee = 1;
        i = 0;
        char *buffer;
        int ok = 1;
        while(continuee){

            char *spiel_info = myread(sock, buffer);
            
            if (*spiel_info != '+') {
                printf("Fehler in der Spielverlauf Phase!");
                exit(0);
            }

            //Wait Befehlsequenz
            if(*(spiel_info+2) == 'W'){
                mywrite(sock, "OKWAIT");
                // //ich bin nicht sicher, ob man epoll() so implementiert und verwendet. Wir sollen uns das auch zusammen anschauen.
                // while(running){
                //     epoll_wait(epoll_fd, &event, sizeof(event), 3000);
                //     spiel_info = myread(sock, buffer);
                //     if(*spiel_info == -1){
                //         printf("HELLO thERE< FEhler!");
                //         exit(10);
                //     }
                //     break;
                // }
            }

            //Move Befehlsequenz
            if(*(spiel_info+2) == 'M'){
            spiel_info = myread(sock, buffer);

            //lese Anzahl an Steinen
            anzahlSteine = atoi(spiel_info + 13) + 1;
            anzahl_Steine = anzahlSteine;

            //hier soll ich noch das Brett in 2 Teile trennen: die Farbe und die Position. Das mache ich heute.
            char currentBrett[anzahlSteine + 1][5];
            i = 0;
            // struct brett brett_table[anzahl_Steine];

            //lese die Steinpositionen und speichere sie

            //spiel_info = myread(sock, buffer);
            while(anzahlSteine > 0){
                spiel_info = myread(sock, buffer);
                strcpy(currentBrett[i], spiel_info + 2);
                save_brett_in_matrix(currentBrett[i][0], currentBrett[i][2] - 'A'+1, currentBrett[i][3] - '0');
                
                if (*spiel_info != '+') {
                    printf("Fehler in der Spielverlauf Phase!");
                    exit(0);
                    }
                anzahlSteine--; 
                i++;
            }
            //strcpy(brett, currentBrett);

            //die Positionen wurden gelesen, jetzt sollen wir sie an Thinker übergeben und den Zug berechnen.
            mywrite(sock, "THINKING");
        }

            //Game or Befehlsequenz
            if(*(spiel_info+2) == 'G')
            {
                //lese Anzahl an Steinen
                anzahlSteine = atoi(spiel_info + 13) + 1;
                anzahl_Steine = anzahlSteine;

                //hier soll ich noch das Brett in 2 Teile trennen: die Farbe und die Position. Das mache ich heute.
                char currentBrett[anzahlSteine + 1][5];
                i = 0;

                //lese die Steinpositionen und speichere sie
                while(anzahlSteine > 0){
                    spiel_info = myread(sock, buffer);
                    strcpy(currentBrett[i++], spiel_info + 2);
                    if (*spiel_info != '+') {
                        printf("Fehler in der Spielverlauf Phase!");
                        exit(0);
                        }
                    anzahlSteine--; 
            }


            //lese den Gewinner und erstell ein Array mit den Spielern und deren Status 
            int nr_spieler = current_game->player_count;
            char whoWonGame[nr_spieler + 1][20];

            i=0;
            while(nr_spieler > 0){
                spiel_info = myread(sock, buffer);
                strncpy(whoWonGame[i], spiel_info + 2, 7);
                if(*(spiel_info + 13) == 'Y'){
                    strcat(whoWonGame[i], " is the winner!"); 
                }
                else{
                    strcat(whoWonGame[i], " has lost!");    
                }
                i++;
                printf("%s/n", whoWonGame[i-1]);
            }

            // if the game has ended, end the while loop
            continuee = 0;
            }
            
            //fiktiver Spielzug - OKTHINK
            if(*(spiel_info+2) == 'O'){
                switch(ok){
                    case 1: 
                        mywrite(sock, "PLAY G3:H4");
                        ok++;
                        break;
                    case 2: 
                        mywrite(sock, "PLAY C3:D4");
                        ok++;
                        break;
                    case 3:
                        mywrite(sock, "PLAY D4:B6");
                        ok++;
                        break;
                    default:
                        break;

                }
                spiel_info = myread(sock, buffer);
            }
    }
}

    char* myread(int *sock, char *buffer) {
        //Erstellt char Speicher mit Größe BUF zum Lesen vom Server
        char b[BUF] = "";
        int i=0;
        char current;
        //liest Nachricht in einzelnen char ein
        do {
            recv(*sock, &current, 1, 0);
            b[i++] = current;
        } while (current != '\n');
        
        buffer = b;
        //beruecksichtigt moegliche fehler
    if (b[0] == '-'){
            printf("Es gab ein Problem...😭\n");
            printf("\n bei %s\n", b);
            exit(0);
        } else {
            printf("🤐🍕🧷S: %s", b);
        }

        return buffer;
    }

    void mywrite(int *sock, char *buffer){
        //Erstellt char Speicher mit der buffer String laenge +1 (für \n)
        char buff[strlen(buffer)+1];
        //Fuegt \n an
        sprintf(buff, "%s\n", buffer);
        //Sendet Nachricht an den Server
        send(*sock, buff,strlen(buff), 0);
        printf("💻 C: %s", buff);
    }

    void save_brett_in_matrix(char color, int column, int row){
        int i = 0;
        while(my_brett[row][column][i] == 'b' || my_brett[row][column][i] == 'w' || my_brett[row][column][i] == 'B' || my_brett[row][column][i] == 'W'){
            printf(" this is i:  %d  ", i);
            i++;
        }

        my_brett[row][column][i] = color;
        printf("my_brett[%i][%i] has %c  \n", row, column, my_brett[row][column][i]);
    }



    //2rayczltiahmv