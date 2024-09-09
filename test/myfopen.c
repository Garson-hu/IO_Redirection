#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

char file[] = "mello from pmem";

#define NUM_ENTR (1 << 21)
#define FALSE 0

// int open(const char *pathname, int flags, ...)
// {
//     // printf("my open\n");
//     return 0;
// }

ssize_t read(int fd, void *buf, size_t count)
{
   	// int fd;
	char *addr;
	off_t offset = 0, pa_offset;
	size_t length;
	ssize_t s;
	struct stat sb;
	int is_write = FALSE;

	// fd = open("/dev/dax1.0", O_RDWR);
	if (fstat(fd, &sb) == -1) /* To obtain file size */
		handle_error("fstat");

	// printf("max size %d\n", sb.st_size);

	offset = atoi("2097152");

	if (offset % NUM_ENTR != 0)
		printf("offset 0x%lx not 2MB aligned, error for daxmem may occur in mmap!!\n", offset % NUM_ENTR);

	pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);

	length = atoi("2097152");

	// if (length > 13)
	// 	length = 13;

	is_write = strcmp("0", "0");
	printf("is_write:%d\n", is_write);

	addr = mmap(NULL, length + offset - pa_offset, PROT_READ | PROT_WRITE,
				MAP_SYNC | MAP_SHARED, // MAP_SYNC|MAP_SHARED_VALIDATE also works for devdax
				fd, pa_offset);
	if (length > 13)
		length = 13;
	if (addr == MAP_FAILED)
		handle_error("mmap");
	

	if (is_write)
	{
		char str[] = "hello pmemm \n";
		str[strlen(str) - 2] = '0' + is_write % 10;
		strcpy(addr, str);
		msync(addr, strlen(str), MS_SYNC); // after write w/ MAP_SHARED
		printf("%s\n", str);
	}

	s = write(STDOUT_FILENO, addr + offset - pa_offset, length);

	munmap(addr, length + offset - pa_offset);
	// close(fd);

	exit(EXIT_SUCCESS);
}