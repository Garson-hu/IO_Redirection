EXE = main myfopen.so
TEMP = actual.txt
CC = gcc
CFLAGS = -std=c99 -Wall

all: $(EXE)

main: main.c
	gcc -Wall -Wextra main.c -o main

myfopen.so: myfopen.c
	gcc $(CFLAGS) -fPIC -shared -o myfopen.so myfopen.c -ldl

clean:
	$(RM) $(EXE) $(TEMP)
