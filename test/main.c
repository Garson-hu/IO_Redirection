#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

extern int pmem_randrw();

int main()
{
	int fd = open("/dev/dax1.0", O_RDWR);
	char s[15 + 1] = {};
	read(fd, s, 15);
	printf("%s\n", s);
}

#define NUM_ENTR (1 << 21)
#define FALSE 0

#define handle_error(msg)   \
	do                      \
	{                       \
		perror(msg);        \
		exit(EXIT_FAILURE); \
	} while (0)
