#include <stdio.h>
#include <stdlib.h>
#include <libpmem.h>
#include <libpmemobj.h>

#define DATA_SIZE 3

#define POOL_LAYOUT "pmem_store_layout"
#define POOL_SIZE (sizeof(int) * DATA_SIZE)
#define PMEM_FILE_PATH "pmem-pool-file"

#define ERROR(M) fprintf(stderr, "Error at line %d: %s = %s\n", __LINE__, #M, (M))

int main() {
    PMEMobjpool *pmem_pool;
    int *pmem_ptr;

    if ((pmem_pool = pmemobj_open(PMEM_FILE_PATH, POOL_LAYOUT)) == NULL) {
        ERROR(pmemobj_errormsg());
        exit(1);
    }

    pmem_ptr = pmemobj_direct(pmemobj_root(pmem_pool, sizeof(int) * DATA_SIZE));

    // Print data from pmem_ptr
    for (int i = 0; i < DATA_SIZE; i++) {
        printf("Data[%d]: %d\n", i, pmem_ptr[i]);
    }

    pmemobj_close(pmem_pool);

    return 0;
}
