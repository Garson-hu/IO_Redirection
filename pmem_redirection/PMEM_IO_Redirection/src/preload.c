#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "metadata.h"

static int (*real_open)(const char *pathname, int flags, ...) = NULL;
static int (*real_close)(int fd) = NULL;
static ssize_t (*real_read) (int fd, void *buf, size_t count) = NULL;
static ssize_t (*real_write) (int fd, void *buf, size_t count) = NULL;

// This function is used to get all the original function of (r/o/w/c)
void init(){
    real_open = dlsym(RTLD_NEXT, "open");
    real_write = dlsym(RTLD_NEXT, "write");
    real_close = dlsym(RTLD_NEXT, "close");
    real_read = dlsym(RTLD_NEXT, "read");
}


// Override open function
int open(const char *pathname, int flags, ...){
    if(!real_open)
    {
        init();
    }    

    // Static: if the pathname have pm/ssd --> go to pm/ssd
    if(strstr)

}

