#include <stdio.h>
#include <libpmem.h>
#include <string.h>
#include <stdlib.h>

int main()
{
  const char *msg = pmem_errormsg();
  printf("Hello, pmem_errormsg = [%s] (%ld) at %p\n", msg, strlen(msg), msg);

  // Check version
  // See https://pmem.io/pmdk/manpages/linux/v1.13/libpmem/libpmem.7/#library-api-versioning-1
  const char *reason = pmem_check_version(PMEM_MAJOR_VERSION,
                            PMEM_MINOR_VERSION);
  
  fprintf(stderr, "reason = %s\n", reason);
  if (reason != NULL) {
    exit(1);
  }
}
