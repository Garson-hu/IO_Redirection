#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "pmem_io_redirection.h"

extern char *base_addr_devdax; // This should be defined in interception library
extern int pmem_fd; // This should be defined in interception library

int main() {
    // setup environment variable
    setenv("REDIRECT_PATH", "/tmp/redirect_test_file", 1);
    setenv("SLURMD_NODENAME", "C35", 1);

    if (base_addr_devdax == NULL) 
        fprintf(stderr, "base_addr_devdax is not initialized.\n");
        exit(EXIT_FAILURE);
    }
    printf("1");
    // open file
    int fd = open("/tmp/redirect_test_file", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    printf("Opened virtual FD: %d\n", fd);

    // write test data
    const char *test_data = "Hello, devdax!";
    ssize_t bytes_written = write(fd, test_data, strlen(test_data));
    if (bytes_written == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    printf("Written bytes: %ld\n", bytes_written);

    // read test data
    char buffer[20];
    lseek(fd, 0, SEEK_SET); 
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    buffer[bytes_read] = '\0';
    printf("Read bytes: %ld, Data: %s\n", bytes_read, buffer);

    // close file
    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }
    printf("Closed virtual FD: %d\n", fd);

    munmap(base_addr_devdax, META_SIZE + (DATA_OFFSET_C36 - META_OFFSET_C36));
    close(pmem_fd);

    return 0;
}
