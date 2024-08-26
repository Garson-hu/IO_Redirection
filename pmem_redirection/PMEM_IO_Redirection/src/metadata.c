#include "metadata.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define PMEM_TOTAL_SIZE (16L * 1024L * 1024L * 1024L)   // Assume the size of PMEM is 16GB for now
#define PMEM_METADATA_SIZE (1024 * 1024 * 1024)         // 1GB for metadata
#define PMEM_PATH ""                                    //! NEED PATH FOR PMEM


static void *pmem_base = NULL;                          // BASE ADDRESS for PMEM
static pmem_metadata_t *metadata_head = NULL;           // pointer to the head of Metadata on PMEM
static int fd_counter = 1000;                           // Base file descriptor, start from 1000


// 1. Map the PMEM to the process
// 2. Get the base address of the PMEM
// 3. Initialize the metadata on PMEM
void init_pmem_metadata(){
    int fd = open(PMEM_PATH, O_RDWR | O_CREAT, 0666);                                           // open PMEM
    if(fd < 0)
    {
        printf("ERROR on open PMEM (init_pmem_metadata in metadata.c)");
        exit(1);
    }

    pmem_base = mmap(NULL, PMEM_TOTAL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);       // Allocate space for the metadata
    if(pmem_base == MAP_FAILED)
    {
        printf("ERROR on mmap PMEM (init_pmem_metadata in metadata.c)");
        exit(1);
    }
    close(fd);

    metadata_head = (pmem_metadata_t*)pmem_base;                                              // Use the first 1GB for metadata management
    if(metadata_head->hash_key == 0)
    {
        // initialize the data of metadata head
        memset(metadata_head, 0, sizeof(pmem_metadata_t));
    }
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

pmem_metadata_t *cache_file_content(const char *path){
    unsigned long file_hash_key = generate_hash(path);
    pmem_metadata_t *entry;

    HASH_FIND(hh, metadata_map, &file_hash_key, sizeof(unsigned long), entry);

    // if this file already in the hash table
    if(entry)
    {
        entry->access_count++;
        return entry;
    }

    // if file is not been cached, cache this file and return
    void *file_content = read_file(path);
    if(file_content)
    {
        entry = (pmem_metadata_t*)malloc(sizeof(pmem_metadata_t));
        entry->hash_key = file_hash_key;
        strcpy(entry->filepath, path);
        entry->pmem_addr = file_content;
        entry->size = sizeof(file_content);
        entry->access_count = 1;
        entry->fd = allocate_unique_fd();

        // add this new item to hash table
        HASH_ADD(hh, metadata_map, hash_key, sizeof(unsigned long), entry);
    }
    return entry;

}
