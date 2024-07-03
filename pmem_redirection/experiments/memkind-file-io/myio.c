#include <stdio.h>
#include <stdbool.h>
#include <string.h> // strdup
#include <stdlib.h> // exit
#include <errno.h>
#include <memkind.h>

#define malloc(S) malloc_wrapper((S))
#define realloc(P, S) realloc_wrapper((P), (S))

void *malloc_wrapper(size_t size)
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

void *realloc_wrapper(void *ptr, size_t size)
{
  printf("realloc %p -> %zd\n", ptr, size);
  void *ret = memkind_realloc(MEMKIND_DEFAULT, ptr, size);
  printf("%p memkind_realloc (now %p) = %zd\n", ptr, ret, size);
  return ret;
}


#define REPORTP(P) printf("%s = %p\n", #P, (P))

// For now, only one "file" at a time
typedef struct {
  char *path;

  // Typical buffer
  size_t size;
  size_t capacity;
  char *buf;

  size_t seek;
  bool isEof;
} MyFILE;

MyFILE *fp = NULL;

// Ignores "path" for now
static bool fileExists(const char *path)
{
  return fp != NULL;
}

// Assumes "src" is a proper string
static char *mystrdup(const char *src)
{
  char *dest = malloc(strlen(src) + 1);
  strcpy(dest, src);
  return dest;
}

static MyFILE *makeFp(const char *path)
{
  fp = malloc(sizeof(*fp));
  fp->path = mystrdup(path);
  fp->size = 0;
  fp->capacity = 5;
  fp->buf = malloc(fp->capacity);
  fp->seek = 0;
  fp->isEof = false;
  return fp;
}

// Supports "a" or "r" in that order
FILE *fopen(const char *path, const char *mode) {
  if (strstr(mode, "a")) {
    if (fileExists(path)) {
      fp->seek = fp->size;

      // TODO: violates strict aliasing
      return (FILE *)fp;
    }

    fp = makeFp(path);

    // TODO: violates strict aliasing
    return (FILE *)fp;
  }

  // Assumes "r"
  if (fp == NULL) {
    errno = ENOENT;
    return NULL;
  }

  fp->seek = 0;
  return NULL;
}

// Appends only
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  if (fp == NULL) {
    fprintf(stderr, "fwrite: open fp first");
    exit(1);
  }

  size_t total = size * nmemb;
  const char *src = ptr;
  for (int i = 0; i < total; i++) {
    char ch = src[i];

    if (fp->capacity == fp->size) {
      fp->capacity *= 2;
      char *newBuf = realloc(fp->buf, fp->capacity);
      if (newBuf == NULL) {
        perror("newBuf");
        exit(1);
      }
      fp->buf = newBuf;
    }

    fp->buf[fp->size++] = ch;
  }

  return nmemb;
}

// Assumes size = 1 for now
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  char *dest = ptr;
  size_t idx = 0;

  while (idx < nmemb && fp->seek < fp->size) {
    dest[idx++] = fp->buf[fp->seek++];
  }

  // fp->isEof = idx < nmemb;
  if (idx < nmemb)
    fp->isEof = true;

  return idx;
}

// TODO: Not implemented
int fclose(FILE *stream)
{
  if (stream) {
    printf("Closing %s ", fp->path);

    // free(fp->buf);
    // putchar('.');

    // free(fp->path);
    // putchar('.');

    // free(fp);
    // putchar('.');

    // fp = NULL;
    // putchar('.');

    puts("Done");
  }

  return 0;
}

int main()
{
  FILE *fp = fopen("hi", "r");
  REPORTP(fp);
  if (!fp) {
    perror("hi");
  } else {
    fclose(fp);
  }

  fp = fopen("hi", "a");
  REPORTP(fp);

  char *world = "Hello, World!\n";
  fwrite(world, 1, strlen(world), fp);

  fclose(fp);
  
  fp = fopen("hi", "r");
  char chptr[1];
  while (fread(chptr, 1, 1, fp))
    putchar(*chptr);

  fclose(fp);
}
