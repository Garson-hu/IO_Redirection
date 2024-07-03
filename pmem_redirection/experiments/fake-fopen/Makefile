CFLAGS += -Wall -Wextra -g3 -std=c99
EXE = fake-fopen myfopen.so #myfopen
# LDLIBS = -ldl

all: $(EXE)

myfopen.so: myfopen.c
	gcc -Wall -fPIC -shared -o myfopen.so myfopen.c -ldl

clean:
	$(RM) $(EXE)
