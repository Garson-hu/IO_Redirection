#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>

// Link with -lmemkind
// See https://pmem.io/memkind/manpages/memkind.3/
#include <memkind.h>

// #define REPORTP(P) printf("%s = %p\n", #P, (P))

void *malloc(size_t size)
{
  printf("OK, malloc'ing %zd :)\n", size);

  if (size == 0) {
    printf("... shortcut, though\n");
    return NULL;
  }

  printf("Calling  memkind_malloc with %zd\n ...", size);
  void *ret = memkind_malloc(MEMKIND_DEFAULT, size);
  printf("memkind_malloc %p = %zd\n", ret, size);
  return ret;
}

void free(void *ptr)
{
  // printf("TODO: destroy %p\n", ptr);
}

void *realloc(void *ptr, size_t size)
{
  printf("realloc %p -> %zd\n", ptr, size);
  void *ret = memkind_realloc(MEMKIND_DEFAULT, ptr, size);
  printf("%p memkind_realloc (now %p) = %zd\n", ptr, ret, size);
  return ret;
}

// int main()
// {
//   FILE *fp = fopen("README.md", "r");
//   REPORTP(fp);
// }