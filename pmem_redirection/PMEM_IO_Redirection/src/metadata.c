#include "metadata.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uthash.h" // uthash head file

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

// Read the file content from the path
void read_file(const char* path){
    FILE *file = fopen(path, "rd");
    if(!file)
    {
        printf("Cannot open file (function: read_file)")
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    void *pmem_addr = malloc(size);
    fread(pmem_addr, 1, size, file);
    fclose(file);
    return pmem_addr;
}

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
