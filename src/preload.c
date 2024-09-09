#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdarg.h>
#include <sys/stat.h>

#define MAX_FILES 1024  // Maximum number of files to track

// Structure to store file path and access count
typedef struct {
    char filepath[256];  // File path
    int access_count;    // Number of accesses
} FileAccess;

// Array to track file accesses
FileAccess file_accesses[MAX_FILES];
int file_count = 0;  // Current number of tracked files

// Function pointers to store original system calls
static int (*real_open)(const char *pathname, int flags, ...) = NULL;
static ssize_t (*real_write)(int fd, const void *buf, size_t count) = NULL;
static ssize_t (*real_read)(int fd, void *buf, size_t count) = NULL;
static int (*real_close)(int fd) = NULL;

// Initialize original system calls
void init() {
    if (!real_open) {
        real_open = dlsym(RTLD_NEXT, "open");
        real_write = dlsym(RTLD_NEXT, "write");
        real_read = dlsym(RTLD_NEXT, "read");
        real_close = dlsym(RTLD_NEXT, "close");
    }
}

// Helper function to find or add a file in the tracking array
int track_file(const char *pathname) {
    // Check if the file is already being tracked
    for (int i = 0; i < file_count; i++) {
        if (strcmp(file_accesses[i].filepath, pathname) == 0) {
            file_accesses[i].access_count++;  // Increment access count
            if(file_accesses[i].access_count == 50)
                file_accesses[i].access_count = 0;
            return file_accesses[i].access_count;
        }
    }

    // If file is not found and we have space, add a new entry
    if (file_count < MAX_FILES) {
        strncpy(file_accesses[file_count].filepath, pathname, sizeof(file_accesses[file_count].filepath));
        file_accesses[file_count].access_count = 1;
        file_count++;
        return 1;  // Initial access count
    }

    // If the table is full, return -1 (couldn't track new file)
    fprintf(stderr, "Error: Maximum number of files reached. Cannot track new file: %s\n", pathname);
    return -1;
}

// Hook for open system call
int open(const char *pathname, int flags, ...) {
    init();  // Initialize original open function

    // Track the file access count
    int access_count = track_file(pathname);
    if (access_count != -1) {
        printf("File opened: %s, total access count: %d\n", pathname, access_count);
    }

    // Call the original open function
    va_list args;
    mode_t mode = 0;
    va_start(args, flags);
    if (flags & O_CREAT) {
        mode = va_arg(args, mode_t); // Get mode argument if O_CREAT is used
    }
    int fd = (mode) ? real_open(pathname, flags, mode) : real_open(pathname, flags);
    va_end(args);

    return fd;
}

// Hook for read system call
ssize_t read(int fd, void *buf, size_t count) {
    init();  // Initialize original read function

    // We could potentially track file descriptor-based access here if needed
    printf("File read: fd = %d\n", fd);

    // Call the original read function
    return real_read(fd, buf, count);
}

// Hook for write system call
ssize_t write(int fd, const void *buf, size_t count) {
    init();  // Initialize original write function

    // We could potentially track file descriptor-based access here if needed
    printf("File written: fd = %d\n", fd);

    // Call the original write function
    return real_write(fd, buf, count);
}

// Hook for close system call
int close(int fd) {
    init();  // Initialize original close function

    // Call the original close function
    int result = real_close(fd);
    printf("File closed: fd = %d\n", fd);

    return result;
}
