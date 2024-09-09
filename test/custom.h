#include <unistd.h>

int arr[] = {'B'};

typedef int FILE;

FILE *fopen(const char *pathname, const char *mode) {
    return NULL;
}

int fscanf(FILE *stream, const char *format, char *x)
{
    *x = arr[0];
    return 0;
}

int printf(const char *format, char x)
{
    return 0;
}
