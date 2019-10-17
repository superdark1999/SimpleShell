CC = gcc

shell: main.c string_utils.c
	$(CC) -o shell main.c string_utils.c -I.