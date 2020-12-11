OBJS    = performConnection.o
OUT     = sysprak-client
CC  = gcc
FLAGS   = -Wall -Wextra -Werror

all: main.c performConnection.o
	$(CC) -o $(OUT) $(FLAGS) main.c $(OBJS)

performConnection.o: performConnection.c
	$(CC) -c performConnection.c

clean:
	rm -f $(OBJS) $(OUT)

play:
	./sysprak-client -g $(GAME_ID) -p $(PLAYER)

test:
	valgrind --leak-check=full --trace-children=yes ./sysprak-client