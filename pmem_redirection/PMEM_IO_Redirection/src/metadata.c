#include "metadata.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
// #include <dlfcn.h>  // If I don't comment this head file, the pathname that I pass to open will be changed


#define PMEM_TOTAL_SIZE (16L * 1024L * 1024L * 1024L)   // Assume the size of PMEM is 16GB for now
#define PMEM_METADATA_SIZE (1024 * 1024 * 1024)         // 1GB for metadata
#define PMEM_PATH "/mnt/fsdax"                         //! NEED PATH FOR PMEM
// #define PMEM_PATH "/dev/dax1.0"                         //! NEED PATH FOR PMEM


static void *pmem_base = NULL;                          // BASE ADDRESS for PMEM
static int fd_counter = 1000;                           // Base file descriptor, start from 1000


// 1. Map the PMEM to the process
// 2. Get the base address of the PMEM
// 3. Initialize the metadata on PMEM
void init_pmem_metadata(){

    int fd = open(PMEM_PATH, O_RDWR | O_CREAT, 0666);                                         // open PMEM
    
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
}

// generate the hash for the input path
unsigned long generate_hash(const char* path){
    unsigned long hash = 6688;
    int c;
    while(c=*path++)
    {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

// // Read the file content from the path
// void read_file(const char* path){
//     FILE *file = fopen(path, "rd");
//     if(!file)
//     {
//         printf("Cannot open file (function: read_file)")
//         return NULL;
//     }
//     fseek(file, 0, SEEK_END);
//     size_t size = ftell(file);
//     fseek(file, 0, SEEK_SET);

//     void *pmem_addr = malloc(size);
//     fread(pmem_addr, 1, size, file);
//     fclose(file);
//     return pmem_addr;
// }


/**
 * Allocate a unique file descriptor for each file.
 * This function is used to simulate the unique file descriptor returned by the system call open()
 * @return a unique file descriptor
 */
int allocate_unique_fd() {
    return fd_counter++; 
}


/**
 * Find the metadata of a file from the hash table based on the given file path
 * @param path the path of the file
 * @return the metadata of the file if the file is found in the hash table, otherwise return NULL
 */
pmem_metadata_t *find_metadata(const char *path){
    unsigned long file_hash_key = generate_hash(path);
    pmem_metadata_t *entry = metadata_head;
    
    printf("Now in function find_metadata in metadata.c\n");

    while(entry != NULL && entry->hash_key != 0)
    {
        if(entry->hash_key == file_hash_key)
            return entry;
        entry = entry->next;
    }
    return NULL;
}

/**
 * Cache the content of a file in PMEM.
 * 
 * This function checks if the file is already cached in PMEM. If it is, the access count of the corresponding metadata is incremented and the metadata is returned.
 * If the file is not cached and migration is allowed, a new metadata is allocated on PMEM, the file is opened, its size is determined, memory is allocated for its content, and the content is read from disk to PMEM.
 * 
 * @param path the path of the file to be cached
 * @return the metadata of the cached file, or NULL if the file cannot be cached
 */
pmem_metadata_t *cache_file_content(const char *path, int should_migrate){
    unsigned long file_hash_key = generate_hash(path);
    pmem_metadata_t *entry = metadata_head;

    if(entry != NULL)
    {
        entry->access_count++;
        return entry;
    }

    // if the file is not cached and don't need migrate, return NULL
    if(!should_migrate)
    {
        printf("Don't migrate the file (function: cache_file_content)");
        return NULL;
    }

    // find the last metadata used in the hash table
    entry = metadata_head;
    while(entry != NULL & entry->hash_key != 0)
    {
        entry = entry->next;
    }

    // allocate a new metadata on PMEM
    pmem_metadata_t *new_entry = (pmem_metadata_t*)((char*)pmem_base + ((char*)entry - (char*)pmem_base) + sizeof(pmem_metadata_t));
    entry->next = new_entry;
    new_entry->hash_key = file_hash_key;
    strcpy(new_entry->filepath, path);
    new_entry->size = 0;
    new_entry->access_count = 1;
    new_entry->fd = allocate_unique_fd();
    new_entry->next = NULL;

    // open the file
    int disk_fd = open(path, O_RDONLY);
    if(disk_fd < 0)
    {
        printf("Cannot open file from disk (function: cache_file_content)");
        return NULL;
    }

    // get the size of the file
    off_t file_size = lseek(disk_fd, 0, SEEK_END);
    lseek(disk_fd, 0, SEEK_SET);

    // allocate memory for the file content
    new_entry->pmem_addr = malloc(file_size);
    if(new_entry->pmem_addr == NULL)
    {
        printf("Cannot allocate memory for file content (function: cache_file_content)");
        close(disk_fd);
        return NULL;
    }

    // read the file content from disk to PMEM
    ssize_t bytes_read = read(disk_fd, new_entry->pmem_addr, file_size);
    if(bytes_read != file_size)
    {
        printf("Cannot read file content from disk (function: cache_file_content)");
        close(disk_fd);
        return NULL;
    }

    new_entry->size = file_size;

    close(disk_fd);

    return new_entry;
}


/**
 * Print the metadata of a file.
 *
 * @param metadata the metadata of the file to be printed
 */
void print_pmem_metadata(const pmem_metadata_t *metadata) {
    if (metadata == NULL) {
        printf("No metadata to print.\n");
        return;
    }

    printf("Metadata Information:\n");
    printf("Hash Key        : %lu\n", metadata->hash_key);
    printf("File Path       : %s\n", metadata->filepath);
    printf("PMEM Address    : %p\n", metadata->pmem_addr);
    printf("Access Count    : %d\n", metadata->access_count);
    printf("File Size       : %zu bytes\n", metadata->size);
    printf("File Descriptor : %d\n", metadata->fd);

    // Check if the next metadata block is available
    if (metadata->next != NULL) {
        printf("Next Metadata Block Available.\n");
    } else {
        printf("No Next Metadata Block.\n");
    }
}