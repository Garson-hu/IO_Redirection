CFLAGS += -Wall -Og -g3 -std=c99
EXE = fancy-file-io myio.so
DEBUG = myio
LDLIBS = -ldl

all: $(EXE)

debug: $(DEBUG)
both: $(EXE) $(DEBUG)

myio.so: myio.c
	gcc -Wall -fPIC -shared -o myio.so myio.c -ldl

myio: myio.c
	$(CC) $(CFLAGS) myio.c -o myio -ldl

clean:
	$(RM) $(EXE) $(DEBUG)
