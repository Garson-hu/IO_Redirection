#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    const char *file_path = "/pmem/test_file.txt";
    char buffer[256];
    int fd = open(file_path, O_RDONLY);

    if (fd < 0) {
        perror("Failed to open file");
        return 1;
    }

    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Failed to read data");
        close(fd);
        return 1;
    }

    buffer[bytes_read] = '\0'; // Null-terminate the buffer
    printf("Read %ld bytes: %s\n", bytes_read, buffer);
    close(fd);

    return 0;
}
