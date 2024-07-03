#include <stdio.h>
#include <stdlib.h>
#include <libpmem.h>

#define PMEM_FILE_PATH "pmem-file"

#define ERROR(M) fprintf(stderr, "%s = %s\n", #M, (M))

int main() {
    int *pmem_ptr;
    int data[3] = {42, 1337, 777};

    size_t mapped_len;
    int is_pmem;

    pmem_ptr = (int *)pmem_map_file(PMEM_FILE_PATH, sizeof(int) * 3,
                                   PMEM_FILE_CREATE | PMEM_FILE_EXCL,
                                   0666, &mapped_len, &is_pmem);
    if (pmem_ptr == NULL) {
		ERROR(pmem_errormsg());
        exit(1);
    }

    // Store data in pmem_ptr
    for (int i = 0; i < 3; i++) {
        pmem_ptr[i] = data[i];
    }

    pmem_persist(pmem_ptr, sizeof(int) * 3);
    pmem_unmap(pmem_ptr, mapped_len);

    return 0;
}
