sysprak-client: main.c
	gcc -o sysprak-client -Wall -Wextra -Werror main.c

play:
	./sysprak-client