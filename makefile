OBJS    = performConnection.o config.o connector.o thinker.o random_ki.o
OUT     = sysprak-client
CC      = gcc
FLAGS   = -Wall -Wextra -Werror -std=c99 -g
O_FLAGS = -std=c99 -g

all: main.c $(OBJS)
	$(CC) -o $(OUT) $(FLAGS) main.c $(OBJS)

performConnection.o: performConnection.c
	$(CC) $(O_FLAGS) -c performConnection.c

config.o: config.c
	$(CC) $(O_FLAGS) -c config.c

connector.o: connector.c
	$(CC) $(O_FLAGS) -c connector.c

thinker.o: thinker.c
	$(CC) $(O_FLAGS) -c thinker.c

random_ki.o: random_ki.c
	$(CC) $(O_FLAGS) -c random_ki.c

clean:
	rm -f $(OBJS) $(OUT)

play:
	./sysprak-client -g $(GAME_ID) -p $(PLAYER) -c $(GAME_CONFIG)

test:
	valgrind --leak-check=full --trace-children=yes ./sysprak-client -g $(GAME_ID) -p $(PLAYER) -c $(GAME_CONFIG)

