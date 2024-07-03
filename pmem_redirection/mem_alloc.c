#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memkind.h>
#include <time.h>
#include <assert.h>

/*
    Since I don't have libpmem at this moment
    So, what I can utilize is to use memkind
*/

// char *path = "/mnt/pmem";

#define ONE_GIB (1ULL << 30)  // 定义1 GiB


int main(){
    printf("all_1");
    struct timespec start_read, end_read, start_write, end_write;
    
    struct memkind *kind = NULL;
    int ret = memkind_create_pmem("/mnt/fsdax", 0, &kind);
    if (ret) {
        fprintf(stderr, "Failed to create pmem kind\n");
        return 1;
    }
    printf("all_2");
    // allocate 1 GiB data
    uint64_t *p = (uint64_t*)memkind_malloc(kind, ONE_GIB);
    if (p == NULL) {
        fprintf(stderr, "Failed to allocate pmem\n");
        return 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &start_write);  // The start time to write
    // random write
    for (uint64_t i = 0; i < ONE_GIB; i++) {
        p[i] = i % 256;  
    }
    clock_gettime(CLOCK_MONOTONIC, &end_write);  // The end time to write
    printf("Successfully written to pmem\n");

    //recore the time of random write of pmem
    double elapsed_write = (end_write.tv_sec - start_write.tv_sec) + 
                     (end_write.tv_nsec - start_write.tv_nsec) / 1e9;
    double speed_write = ONE_GIB / (1024 * 1024) / elapsed_write; 

    //random read
    srand(time(NULL));
    clock_gettime(CLOCK_MONOTONIC, &start_read);  // The start time to read 

    uint64_t sum = 0;
    for (uint64_t i = 0; i < ONE_GIB / sizeof(uint64_t); i++) {
        uint64_t index = rand() % (ONE_GIB / sizeof(uint64_t));  // random index
        sum += p[index];
    }
    clock_gettime(CLOCK_MONOTONIC, &end_read);  // The end time to read
    double elapsed_read = (end_read.tv_sec - start_read.tv_sec) + 
                     (end_read.tv_nsec - start_read.tv_nsec) / 1e9;

    double speed_read = ONE_GIB / (1024 * 1024) / elapsed_read; 
    printf("Read checksum: %llu\n", sum);

    printf("random write speed: %.2f MiB/s\n", speed_write);
    printf("random read speed: %.2f MiB/s\n", speed_read);

    // release pmem 
    memkind_free(kind, p);

    return 0;

}