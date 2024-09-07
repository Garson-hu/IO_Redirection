#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "metadata.h"


int main() {
    init_pmem_metadata();
    return 0;
}