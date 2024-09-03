#ifndef METADATA_H
#define METADATA_H

#include <stddef.h>

#define MAXMIUN_FILE_PATH 256

// data structure of metadata 
typedef struct {
    unsigned long hash_key;             // The hash value of a file path (as the key)
    char filepath[MAXMIUN_FILE_PATH];   // the length of a file path
    void *pmem_addr;                    // the start address of this file in PMEM
    int access_count;                   // access time of the file
    size_t size;                        // size of the file
    int fd;                             // file descritpor 
    struct pmem_metadata_t *next;       // pointer to the next metadata block
}pmem_metadata_t;

// function declarition
unsigned long generate_hash(const char *path);                              // return the hash generated from the path
// void read_file(const char *path);                                        // read the content of the file
void init_pmem_metadata();                                                  // initialize the metadata of pm
pmem_metadata_t *find_metadata(const char *path);                           // find the metadata through the given path
pmem_metadata_t *cache_file_content(const char *path, int should_migrate);  // cache/migrate the content of a file
int allocate_unique_fd();                                                   // return a unique fd for each file

// global variable
static pmem_metadata_t *metadata_head = NULL;           // pointer to the head of Metadata on PMEM

#endif // METADATA_H