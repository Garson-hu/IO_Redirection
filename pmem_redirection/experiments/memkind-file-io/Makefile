CFLAGS = -Wall -Wextra -g3 -std=c99
EXE = fake-malloc mymalloc.so optanemalloc.so exp/empty fancy-file-io myio.so
DEBUG = mymalloc optanemalloc
TEST_FILES = out*.txt

MY_SO_FLAGS = -Wall -fPIC -shared
MY_INCLUDE_PATH = $(HOME)/.local/include
MY_LIB_PATH = $(HOME)/.local/lib

all: $(EXE)
debug: $(DEBUG)
both: $(EXE) $(DEBUG)

mymalloc.so: mymalloc.c
	gcc $(MY_SO_FLAGS) -o mymalloc.so mymalloc.c -ldl

optanemalloc.so: optanemalloc.c
	gcc -I $(MY_INCLUDE_PATH) $(MY_SO_FLAGS) -o optanemalloc.so optanemalloc.c -lmemkind -L$(MY_LIB_PATH)

myio.so: myio.c
	gcc -I $(MY_INCLUDE_PATH) $(MY_SO_FLAGS) -o myio.so myio.c -lmemkind -L$(MY_LIB_PATH)

mymalloc: mymalloc.c
	$(CC) $(CFLAGS) mymalloc.c -o mymalloc -ldl

optanemalloc: optanemalloc.c
	$(CC) $(CFLAGS) optanemalloc.c -o optanemalloc -ldl

clean:
	$(RM) $(EXE) $(DEBUG) $(TEST_FILES)
