#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>

// #define REPORTP(P) printf("%s = %p\n", #P, (P))

void *malloc(size_t size)
{
  printf("OK, malloc'ing %zd :)\n", size);

  if (size == 0) {
    printf("... shortcut, though\n");
    return NULL;
  }

  static void *(*original_malloc)(size_t) = NULL;
  if (original_malloc == NULL) {
    printf("Calling dlsym\n");
    original_malloc = dlsym(RTLD_NEXT, "malloc");
  }
  return (*original_malloc)(size);
}

// int main()
// {
//   FILE *fp = fopen("README.md", "r");
//   REPORTP(fp);
// }