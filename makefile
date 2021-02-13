OBJS    = performConnection.o config.o connector.o thinker.o random_ki.o ki.o
OUT     = sysprak-client
CC      = gcc
FLAGS   = -Wall -Wextra -Werror -g
O_FLAGS = -g
BUNDLE  = abgabe.zip

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

ki.o: ki.c
	$(CC) $(O_FLAGS) -c ki.c

clean:
	rm -f $(OBJS) $(OUT) $(BUNDLE)

play:
	./sysprak-client -g $(GAME_ID) -p $(PLAYER) -c $(GAME_CONFIG)

test:
	valgrind --leak-check=full --trace-children=yes ./sysprak-client -g $(GAME_ID) -p $(PLAYER) -c $(GAME_CONFIG)

bundle:
	zip -r $(BUNDLE) ./ -x "*.git*" -x "*.vscode*" -x "*.idea*" -x "*$(BUNDLE)*" -x "*.o" -x "*sysprak-client*"