#include <stdio.h>
#include <stdlib.h>
#include <libpmem.h>
#include <libpmemobj.h>

#define DATA_SIZE 3

#define POOL_LAYOUT "pmem_store_layout"
#define POOL_SIZE (sizeof(int) * DATA_SIZE)

#define ERROR(M) fprintf(stderr, "Error at line %d: %s = %s\n", __LINE__, #M, (M))

int main() {
    PMEMobjpool *pmem_pool;
    PMEMoid pmem_oid;
    int *pmem_ptr;
    int data[DATA_SIZE] = {42, 1337, 777};

    if ((pmem_pool = pmemobj_create("pmem-pool-file", POOL_LAYOUT, POOL_SIZE + PMEMOBJ_MIN_POOL, 0666)) == NULL) {
        ERROR(pmemobj_errormsg());
        exit(1);
    }

    pmemobj_alloc(pmem_pool, &pmem_oid, POOL_SIZE, 1, NULL, NULL);
    pmem_ptr = pmemobj_direct(pmemobj_root(pmem_pool, sizeof(int) * DATA_SIZE));

    if (pmem_ptr == NULL) {
        ERROR(pmemobj_errormsg());
        pmemobj_close(pmem_pool);
        exit(1);
    }

    // Store data in pmem_ptr
    for (int i = 0; i < DATA_SIZE; i++) {
        pmem_ptr[i] = data[i];
    }

    pmemobj_persist(pmem_pool, pmem_ptr, POOL_SIZE);
    pmemobj_close(pmem_pool);

    return 0;
}
