#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    const char *file_path = "/mnt/fsdax/test_file.txt";  // Path for FSDAX
    char buffer[256];  // Buffer to hold read data

    // Open the file for reading
    int fd = open(file_path, O_RDONLY);

    if (fd < 0) {
        perror("Failed to open file");
        return 1;
    }

    // Read data from the file
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Failed to read data");
        close(fd);
        return 1;
    }

    buffer[bytes_read] = '\0';  // Null-terminate the string

    printf("Read %ld bytes: %s\n", bytes_read, buffer);
    
    // Close the file
    close(fd);

    return 0;
}
