#ifndef METADATA_H
#define METADATA_H

#include <stddef.h>
#include <uthash.h>

#define MAXMIUN_FILE_PATH 256

// data structure of metadata 
typedef struct {
    unsigned long hash_key;             // The hash value of a file path (as the key)
    char filepath[MAXMIUN_FILE_PATH];   // the length of a file path
    void *pmem_addr;                    // the start address of this file in PMEM
    int access_count;                  // access time of the file
    int fd;                             // file descritpor 
    UT_hash_handle hh;                  // uthash handle
}pmem_metadata_t;

// function declarition
unsigned long generate_hash(const char *path);          // return the hash generated from the path
void read_file(const char *path);                       // read the content of the file
pmem_metadata_t *cache_file_content(const char *path);  // cache the content of a file
int allocate_unique_fd();                               // return a unique fd for each file


#endif // METADATA_H