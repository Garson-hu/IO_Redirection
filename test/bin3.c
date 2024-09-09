#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FILE_PATH "/mnt/fsdax/ghu4/test_file.txt"  // File to be accessed
#define NUM_ACCESS 100                       // Number of times to access the file

int main() {
    const char *data = "Test data for persistent memory.";
    char buffer[256];

    for (int i = 0; i < NUM_ACCESS; i++) {
        // Open the file for both reading and writing
        int fd = open(FILE_PATH, O_RDWR | O_CREAT, 0666);
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

        // Seek back to the beginning of the file
        lseek(fd, 0, SEEK_SET);

        // Read data from the file
        ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
        if (bytes_read < 0) {
            perror("Failed to read data");
            close(fd);
            return 1;
        }

        buffer[bytes_read] = '\0';  // Null-terminate the string

        // Print the current iteration and the read data
        printf("Iteration %d: Read data: %s\n", i + 1, buffer);

        // Close the file
        close(fd);
    }

    printf("Completed %d accesses to the file.\n", NUM_ACCESS);
    return 0;
}
