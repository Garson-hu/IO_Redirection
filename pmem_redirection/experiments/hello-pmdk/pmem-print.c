#include <stdio.h>
#include <stdlib.h>
#include <libpmem.h>

#define PMEM_FILE_PATH "pmem-file"

#define ERROR(M) fprintf(stderr, "%s = %s\n", #M, (M))

int main() {
    int *pmem_ptr;

    size_t mapped_len;
    int is_pmem;

    pmem_ptr = (int *)pmem_map_file(PMEM_FILE_PATH, 0, 0, 0666, &mapped_len, &is_pmem);
    if (pmem_ptr == NULL) {
        ERROR(pmem_errormsg());
        exit(1);
    }

    // Print the stored data
    for (int i = 0; i < 3; i++) {
        printf("Data[%d]: %d\n", i, pmem_ptr[i]);
    }

    pmem_unmap(pmem_ptr, mapped_len);

    return 0;
}
