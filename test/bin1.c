#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    const char *file_path = "/mnt/fsdax/test_file.txt";  // Path for FSDAX
    const char *data = "Hello, Persistent Memory in FSDAX!";
    
    // Open the file for writing
    int fd = open(file_path, O_WRONLY | O_CREAT, 0666);
    
    if (fd < 0) {
        perror("Failed to open file");
        return 1;
    }

    // Write data to the file
    ssize_t bytes_written = write(fd, data, strlen(data));
    if (bytes_written < 0) {
        perror("Failed to write data");
        close(fd);
        return 1;
    }

    printf("Wrote %ld bytes to %s\n", bytes_written, file_path);
    
    // Close the file
    close(fd);

    return 0;
}
