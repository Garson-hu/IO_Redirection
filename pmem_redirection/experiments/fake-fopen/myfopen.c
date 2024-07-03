#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>

// #define REPORTP(P) printf("%s = %p\n", #P, (P))

FILE *fopen(const char *path, const char *mode) {
  printf("Hello, %s :). So, you want to %s?\n", path, mode);
  FILE *(*original_fopen)(const char*, const char*);
  original_fopen = dlsym(RTLD_NEXT, "fopen");
  return (*original_fopen)(path, mode);
}

// int main()
// {
//   FILE *fp = fopen("README.md", "r");
//   REPORTP(fp);
// }