CFLAGS += -Wall -Wextra -g3 -std=c99
EXE = hello-ld-preload myfopen.so

all: $(EXE)

myfopen.so: myfopen.c
	gcc -Wall -fPIC -shared -o myfopen.so myfopen.c

clean:
	$(RM) $(EXE)
