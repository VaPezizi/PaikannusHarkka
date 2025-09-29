main: main.c
	$(CC) main.c endians.c -o main.o -Wall -Wextra -std=c99
