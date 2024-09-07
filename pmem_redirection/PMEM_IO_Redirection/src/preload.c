#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include "metadata.h"

/**
 * The constructor attribute tells the linker to call this function before the
 * main() function is called. This is used to initialize the metadata of the
 * PMEM device before the program starts.
 *
 * The function simply calls init_pmem_metadata() to initialize the metadata.
 */
// __attribute__((constructor))
// void preload_init() {
//     // remove LD_PRELOAD
//     unsetenv("LD_PRELOAD");
    
//     // init PMEM metadata
//     init_pmem_metadata();
    
//     // recover LD_PRELOAD
//     setenv("LD_PRELOAD", "./lib/libpreload.so", 1);
// }



#define PMEM_TOTAL_SIZE (16L * 1024L * 1024L * 1024L)   // Assume the size of PMEM is 16GB for now
#define PMEM_METADATA_SIZE (1024 * 1024 * 1024)         // 1GB for metadata
#define PMEM_PATH "/mnt/fsdax/ghu4/test.txt"                         //! NEED PATH FOR PMEM

static void *pmem_base = NULL;                          // BASE ADDRESS for PMEM

static int (*real_open)(const char *pathname, int flags, ...) = NULL;
static int (*real_close)(int fd) = NULL;
static ssize_t (*real_read) (int fd, void *buf, size_t count) = NULL;
static ssize_t (*real_write) (int fd, void *buf, size_t count) = NULL;

static int initialized = 0;

// This function is used to get all the original function of (r/o/w/c)
void init(){

    if(initialized) return;
    // init_pmem_metadata();
    real_open = dlsym(RTLD_NEXT, "open");
    real_write = dlsym(RTLD_NEXT, "write");
    real_close = dlsym(RTLD_NEXT, "close");
    real_read = dlsym(RTLD_NEXT, "read");
    
    int fd = real_open(PMEM_PATH, O_RDWR | O_CREAT, 0666);                                         // open PMEM
    
    if(fd < 0)
    {
        printf("ERROR on open PMEM (init_pmem_metadata in metadata.c)");
        exit(1);
    }

    pmem_base = mmap(NULL, PMEM_TOTAL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);       // Allocate space for the metadata
    printf("pmem_base = %p in init_pmem_metadata() of metadata.c\n", pmem_base);

    if(pmem_base == MAP_FAILED)
    {
        printf("ERROR on mmap PMEM (init_pmem_metadata in metadata.c)");
        exit(1);
    }

    printf("pmem_base mapped, now initialize the metadata head (init_pmem_metadata in metadata.c)\n");

    close(fd);

    metadata_head = (pmem_metadata_t*)pmem_base;                                              // Use the first 1GB for metadata management
    if(metadata_head->hash_key == 0)
    {
        // initialize the data of metadata head
        memset(metadata_head, 0, sizeof(pmem_metadata_t));
    }

    printf("initialize done (init_pmem_metadata in metadata.c)\n");

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

    printf("I'm in open() of preload.c\n");

    int should_migrate = 1;  //! check if need to migrate
    printf("pathname:%s\n", pathname);
    // Static: if the pathname have pm/ssd --> go to pm/ssd
    if(strstr(pathname, "/pmem/") != NULL)
    {
        printf("I'm in first if of open() of preload.c\n");
        pmem_metadata_t *entry = find_metadata(pathname);
        if(entry)
        {
            printf("I'm in first ENTRY-1 of open() of preload.c\n");
            entry->access_count++;
            printf("Intercepted open for PMEM file: %s, returning custom fd: %d\n", pathname, entry->fd);
            return entry->fd;
        }
    }else {
        // if the file path don't have pm/ssd, find it's metadata first
        pmem_metadata_t *entry = find_metadata(pathname);
        if(entry)
        {   
            printf("I'm in second ENTRY-2 of open() of preload.c\n");
            printf("file %s found in PMEM metadata, using custom fd: %d\n", pathname, entry->fd);
            entry->access_count++;
            return entry->fd;
        }else{
            if(should_migrate)
            {
                printf("I'm in first should_migrate-1 of open() of preload.c\n");

                entry = cache_file_content(pathname, should_migrate);
                if(entry)
                {
                    printf("I'm in third ENTRY-3 of open() of preload.c\n");
                    entry->access_count++;
                    printf("Migrate file %s to PMEM, returning custom fd: %d\n", pathname, entry->fd);
                    return entry->fd;
                }
            }
            
        }
    } // end first if

    // if the file is not cached in PMEM, use the original open function
    int fd;
    // if (flags & O_CREAT) {
    //     va_list args;
    //     va_start(args, flags);
    //     mode_t mode = va_arg(args, mode_t);
    //     printf("original open\n");
    //     fd = real_open(pathname, flags, mode);
    //     va_end(args);
    // } else {
    //     printf("original open\n");
    //     fd = real_open(pathname, flags);
    // }

    printf("original open in open() at preload.c\n");
    fd = real_open(pathname, flags);

    return fd;
}


/**
 * Reads data from a file descriptor, intercepting reads to PMEM files.
 *
 * @param fd   The file descriptor to read from.
 * @param buf  The buffer to store the read data.
 * @param count The number of bytes to read.
 *
 * @return The number of bytes read, or the result of the underlying real_read function if the file is not a PMEM file.
 */
ssize_t read(int fd, void *buf, size_t count){
    init();

    pmem_metadata_t *entry = metadata_head;
    while(entry != NULL & entry->fd != fd)
    {
        entry = entry->next;
    }

    if (entry)
    {
        printf("Intercepted read for PMEM file: %s, reading %ld bytes from pmem\n", entry->filepath, count);
        memcpy(buf, entry->pmem_addr, count);
        entry->access_count++;
        return count;
    }

    printf("original read\n");
    return real_read(fd, buf, count);
    
}

/**
 * Writes data to a file descriptor, intercepting writes to PMEM files.
 *
 * @param fd   The file descriptor to write to.
 * @param buf  The buffer containing the data to write.
 * @param count The number of bytes to write.
 *
 * @return The number of bytes written, or the result of the underlying real_write function if the file is not a PMEM file.
 */
ssize_t write(int fd, const void *buf, size_t count){
    init();
    printf("I'm in write() of preload.c\n");
    // find the metadata of the fd

    pmem_metadata_t *entry = metadata_head;

    print_pmem_metadata(metadata_head);
    printf("fd = %d in write() of preload.c\n");
    while(entry != NULL & entry->fd != fd)
    {
        entry = entry->next;
    }
    
    if(entry)
    {
        printf("Intercepted write for PMEM file: %s, writing %ld bytes to pmem\n", entry->filepath, count);
        //TODO if the new size is smaller than the old size, delete the old content
        memcpy((char*)entry->pmem_addr, buf, count);
        entry->access_count++;
        entry->size = count;
        return count;
    }

    printf("original write\n");
    return real_write(fd, buf, count);
}


int close(int fd){
    init();

    pmem_metadata_t *entry = metadata_head;
    while(entry != NULL & entry->fd != fd)
    {
        entry = entry->next;
    }

    if(entry)
    {
        printf("Intercepted close for PMEM file: %s\n", entry->filepath);

        return 0;
    }

    return real_close(fd);
}

