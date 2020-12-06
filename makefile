performConnection.a: performConnection.o
	ar -q performConnection.a performConnection.o

performConnection.o: performConnection.c
	gcc -c performConnection.c

sysprak-client: main.c
	gcc -o sysprak-client -Wall -Wextra -Werror main.c performConnection.a

play:
	./sysprak-client