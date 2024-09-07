#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {

    const char *file_path = "/home/ghu4/filesys/pmem_redirection/PMEM_IO_Redirection/build/test_files/pmem/test_file.txt";
    const char *data = "Hello, Persistent Memory!";
    int fd = open(file_path, O_WRONLY | O_CREAT, 0666);
    // printf("fd = %d\n", fd);
    if (fd < 0) {
        perror("Failed to open file");
        return 1;
    }
    printf("open finished, start write in bin1.c\n");

    ssize_t bytes_written = write(fd, data, strlen(data));
    if (bytes_written < 0) {
        perror("Failed to write data");
        close(fd);
        return 1;
    }

    printf("Wrote %ld bytes to %s\n", bytes_written, file_path);
    close(fd);

    return 0;
}
