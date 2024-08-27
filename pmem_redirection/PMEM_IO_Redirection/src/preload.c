#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "metadata.h"

static int (*real_open)(const char *pathname, int flags, ...) = NULL;
static int (*real_close)(int fd) = NULL;
static ssize_t (*real_read) (int fd, void *buf, size_t count) = NULL;
static ssize_t (*real_write) (int fd, void *buf, size_t count) = NULL;

static int initialized = 0;

// This function is used to get all the original function of (r/o/w/c)
void init(){

    if(initialized) return;

    real_open = dlsym(RTLD_NEXT, "open");
    real_write = dlsym(RTLD_NEXT, "write");
    real_close = dlsym(RTLD_NEXT, "close");
    real_read = dlsym(RTLD_NEXT, "read");
    
    initialized = 1;

}


// Override open function
/**
 * Opens a file with the given pathname and flags.
 *
 * @param pathname The path of the file to be opened.
 * @param flags The flags for opening the file.
 * @param ... Additional arguments for opening the file.
 * @return The file descriptor of the opened file.
 *
 * @throws None.
 */
int open(const char *pathname, int flags, ...){
    init();

    // default not migrate
    int should_migrate = 0;  //! check if need to migrate

    // Static: if the pathname have pm/ssd --> go to pm/ssd
    if(strstr(pathname, "/pmem/") != NULL)
    {
        pmem_metadata_t *entry = find_metadata(pathname);
        if(entry)
        {
            printf("Intercepted open for PMEM file: %s, returning custom fd: %d\n", pathname, entry->fd);
            return entry->fd;
        }
    }else {
        // if the file path don't have pm/ssd, find it's metadata first
        pmem_metadata_t *entry = find_metadata(pathname);
        if(entry)
        {
            printf("file %s found in PMEM metadata, using custom fd: %d\n", pathname, entry->fd);
            return entry->fd;
        }else{
            if(should_migrate)
            {
                entry = cache_file_content(pathname);
                if(entry)
                {
                    printf("Migrate file %s to PMEM, returning custom fd: %d\n", pathname, entry->fd);
                    return entry->fd;
                }
            }
            
        }
    } // end first if

    // if the file is not cached in PMEM, use the original open function
    int fd;
    if (flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode_t mode = va_arg(args, mode_t);
        fd = real_open(pathname, flags, mode);
        va_end(args);
    } else {
        fd = real_open(pathname, flags);
    }

    return fd;
}



ssize_t read(int fd, void *buf, size_t count){
    
}


ssize_t write(int fd, const void *buf, size_t count){
    init();

    // find the metadata of the fd
    pmem_metadata_t *entry = metadata_head;
    while(entry != NULL & entry->fd != fd)
    {
        entry = entry->next;
    }
}


int close(int fd){
    
}

