#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include "pmem_io_redirection.h"

/*function define*/
int allocate_virtual_fd(void);
off_t select_data_offset_based_on_node(void);
int find_fd_by_pathname(const char *pathname);

/* Error handle function */
#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

/*
    INVALID_FD: Invalid file descriptor constants
*/
#define INVALID_FD -1

// Define the max file number
#define MAX_FDS 1024

/*
    Customizing the File Descriptor Information Structure
    1. offset: Offset of file data in devdax
    2. length: length of the file
    3. is_redirected: Whether or not to redirect to devdax
    4. filename: Filename associated with this fd_info
*/
typedef struct fd_info
{
    off_t offset;
    size_t length;
    int is_redirected;
    char *filename;
} fd_info_t;

// Virtual file descriptor to fd_info mapping
fd_info_t fds[MAX_FDS];

/*
    Metadata data structure
    1. offset: file offset
    2. length: file length

*/
typedef struct{
    off_t offset;
    size_t length;
}metadata_t;

metadata_t metadata[MAX_FDS];

/*
    Define variables:
    base_addr_devdax:
    pmem_fd: used for open pmem later

*/
char *base_addr_devdax = NULL;
int pmem_fd = -1;
struct stat sb;


/* Original Function Pointer */
static int (*original_open)(const char *pathname, int flags, ...) = NULL;
static ssize_t (*original_read)(int fd, void *buf, size_t count) = NULL;
static ssize_t (*original_write)(int fd, const void *buf, size_t count) = NULL;
static int (*original_close)(int fd) = NULL;

/* Initialization function to intercept the original function */
void init(void) __attribute__((constructor));
void init(void)
{

    original_open = dlsym(RTLD_NEXT, "open");
    original_read = dlsym(RTLD_NEXT, "read");
    original_write = dlsym(RTLD_NEXT, "write");
    original_close = dlsym(RTLD_NEXT, "close");

    memset(fds, 0, sizeof(fds)); // make sure all fds is unused at first

    pmem_fd = open("/dev/dax1.0", O_RDWR);
    
    if(pmem_fd == -1)
    {
        perror("open pmem");
        exit(EXIT_FAILURE);
    }

    if (fstat(pmem_fd, &sb) == -1) /* To obtain file size */
		handle_error("fstat");

    // Map the whole devdax region
    // Whole size: Metadata region + Date region (sb.st_size - 2MB)
    base_addr_devdax = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, pmem_fd, 0);
    if (base_addr_devdax == MAP_FAILED) {
        perror("mmap");
        close(pmem_fd);
        exit(EXIT_FAILURE);
    }
    // Initialize metadata array from pmem
    memcpy(metadata, base_addr_devdax + META_OFFSET_C36, sizeof(metadata_t) * MAX_FDS);
}

/*
    If you want to LD_PRELOAD the file, you need to use the following command first:
    export REDIRECT_PATH="/data/redirect_files/FILENAME_DEFINED_BY_YOURSELF"

    LD_PRELOAD open()
    Para:
    1. pathname:
*/
int open(const char *pathname, int flags, ...)
{

    const char *redirect_path = getenv("REDIRECT_PATH");

    if (redirect_path != NULL && strcmp(pathname, redirect_path) == 0)
    {

        // Check if the file is already exists in the fds array
        int existing_fd = find_fd_by_pathname(pathname);
        if (existing_fd != INVALID_FD)
        {
            // File is already exist, return existing virtual FD
            return existing_fd;
        }

        // if file not opened, assigned a new virtual FD
        int virtual_fd = allocate_virtual_fd();

        if (virtual_fd == INVALID_FD)
        {
            return INVALID_FD;
        }

        fds[virtual_fd].offset = select_data_offset_based_on_node();
        fds[virtual_fd].length = 0;
        fds[virtual_fd].is_redirected = 1;
        fds[virtual_fd].filename = strdup(pathname);

        metadata[virtual_fd].offset = fds[virtual_fd].offset;
        metadata[virtual_fd].length = fds[virtual_fd].length;
        memcpy(base_addr_devdax + META_OFFSET_C36, metadata, sizeof(metadata_t) * MAX_FDS);

        return virtual_fd;
    }

    // if don't need to redirect, back to the original open
    va_list args;
    va_start(args, flags);
    mode_t mode = va_arg(args, mode_t);
    va_end(args);
    return original_open(pathname, flags, mode);
}

ssize_t read(int fd, void *buf, size_t count)
{
    // First, check if the fd is a virtual one and if it has been redirected.
    if (fd >= 0 && fd < MAX_FDS && fds[fd].is_redirected)
    {
        // Calculate the actual memory address in devdax based on the offset and length.
        // Here, we assume you have mapped the entire devdax region into your process space
        // at some base address `base_addr_devdax`. You need to implement this mapping.
        // char *base_addr_devdax; // Assume this is initialized elsewhere.
        off_t offset_in_devdax = fds[fd].offset;
        char *target_addr = base_addr_devdax + offset_in_devdax;

        // Ensure the read does not go past the end of the allocated region for this file.
        if (count > fds[fd].length)
        {
            count = fds[fd].length;
        }

        // Perform the memory copy from the calculated address to the buffer.
        memcpy(buf, target_addr, count);

        // Update the offset to simulate file read progress.
        // Depending on your implementation, you may need to track read progress separately.
        fds[fd].offset += count;

        // Update metadata
        metadata[fd].offset = fds[fd].offset;
        metadata[fd].length = fds[fd].length;
        memcpy(base_addr_devdax + META_OFFSET_C36, metadata, sizeof(metadata_t) * MAX_FDS);

        // Return the number of bytes read.
        return count;
    }
    else
    {
        // If not redirected, call the original read function.
        return original_read(fd, buf, count);
    }
}

ssize_t write(int fd, const void *buf, size_t count)
{
    // First, check if the fd is a virtual one and if it has been redirected.
    if (fd >= 0 && fd < MAX_FDS && fds[fd].is_redirected)
    {
        // Calculate the actual memory address in devdax based on the offset.
        // Again, we assume the entire devdax region is mapped into your process space
        // at some base address `base_addr_devdax`. You need to implement this mapping.
        // char *base_addr_devdax; // Assume this is initialized elsewhere.
        off_t offset_in_devdax = fds[fd].offset;
        char *target_addr = base_addr_devdax + offset_in_devdax;

        // Perform the memory copy from the buffer to the calculated address.
        memcpy(target_addr, buf, count);

        // Update the fd_info to reflect the change.
        fds[fd].offset += count;
        if (fds[fd].length < fds[fd].offset)
        {
            fds[fd].length = fds[fd].offset;
        }

        // Update metadata
        metadata[fd].offset = fds[fd].offset;
        metadata[fd].length = fds[fd].length;
        memcpy(base_addr_devdax + META_OFFSET_C36, metadata, sizeof(metadata_t) * MAX_FDS);


        // Return the number of bytes written.
        return count;
    }
    else
    {
        // If not redirected, call the original write function.
        return original_write(fd, buf, count);
    }
}

int close(int fd) {
    // First, check if the fd is a virtual one and if it has been redirected.
    if (fd >= 0 && fd < MAX_FDS && fds[fd].is_redirected) {
        // Perform any necessary cleanup for the virtual FD.
        // For example, if you have associated dynamic memory with this FD:
        if (fds[fd].filename) {
            free(fds[fd].filename);  // Free the dynamically allocated filename.
            fds[fd].filename = NULL;
        }

        // Reset the fd_info structure to indicate that the FD is no longer in use.
        fds[fd].is_redirected = 0;
        fds[fd].offset = 0;
        fds[fd].length = 0;

        // Reset metadata
        metadata[fd].offset = 0;
        metadata[fd].length = 0;
        memcpy(base_addr_devdax + META_OFFSET_C36, metadata, sizeof(metadata_t) * MAX_FDS);

        // Since this is a virtual FD, there's no real file descriptor to close.
        // Return 0 to indicate success.
        return 0;
    } else {
        // If not redirected, call the original close function.
        return original_close(fd);
    }
}

/*---------------------------------------- Here are some of the functions ---------------------------------------- */

// Find and initialize an unused virtual FD
int allocate_virtual_fd(void)
{
    for (int i = 0; i < MAX_FDS; ++i)
    {
        if (fds[i].is_redirected == 0)
        {                             // unused item
            fds[i].is_redirected = 1; // label as used
            fds[i].offset = 0;
            fds[i].length = 0;
            return i;
        }
    }
    return INVALID_FD;
}

// select DATA offset according to the node name
off_t select_data_offset_based_on_node(void)
{
    const char *node_name = getenv("SLURMD_NODENAME");

    if (node_name != NULL)
    {
        if (strcmp(node_name, "C35") == 0)
        {
            return DATA_OFFSET_C35;
        }
        else if (strcmp(node_name, "C36") == 0)
        {
            return DATA_OFFSET_C36;
        }
        else
        {
            fprintf(stderr, "Unknown node name: %s\n", node_name);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(stderr, "SLURMD_NODENAME environment variable is not set.\n");
        exit(EXIT_FAILURE);
    }
}

// find fd according to the pathname of the file
int find_fd_by_pathname(const char *pathname)
{
    for (int i = 0; i < MAX_FDS; ++i)
    {
        if (fds[i].is_redirected && fds[i].filename != NULL && strcmp(fds[i].filename, pathname) == 0)
        {
            // Found an existing entry with the same pathname
            return i;
        }
    }
    return INVALID_FD;
}