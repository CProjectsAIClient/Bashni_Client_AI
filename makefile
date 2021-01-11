OBJS    = performConnection.o config.o connector.o thinker.o ki.c
OUT     = sysprak-client
CC      = gcc
FLAGS   = -Wall -Wextra -Werror

all: main.c $(OBJS)
	$(CC) -g -o $(OUT) $(FLAGS) main.c $(OBJS)

performConnection.o: performConnection.c
	$(CC) -g -c performConnection.c

config.o: config.c
	$(CC) -g -c config.c

connector.o: connector.c
	$(CC) -g -c connector.c

thinker.o: thinker.c
	$(CC) -g -c thinker.c

ki.o: ki.c
	$(CC) -g -c ki.c

clean:
	rm -f $(OBJS) $(OUT)

play:
	./sysprak-client -g $(GAME_ID) -p $(PLAYER) -c $(GAME_CONFIG)

test:
	valgrind --leak-check=full --trace-children=yes ./sysprak-client -g $(GAME_ID) -p $(PLAYER) -c $(GAME_CONFIG)