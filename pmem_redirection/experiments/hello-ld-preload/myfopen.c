#include <stdio.h>
#include <errno.h>

FILE *fopen(const char *path, const char *mode) {
  printf("Hello, World!\n");
  errno = ENOENT;
  return NULL;
}
