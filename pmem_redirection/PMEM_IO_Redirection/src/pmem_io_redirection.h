// pmem_io_redirection.h
#ifndef PMEM_IO_REDIRECTION_H
#define PMEM_IO_REDIRECTION_H

#include <stddef.h>

/* 
    Those definitions are used for metadata management in pmem.
    Everytime when we want to read/write something, we need to check metadata region first.
    Para:
    1. META_OFFSET_C35: Assumed metadata storage start address on node C35, this is also the start addr of devdax on C35
    2. META_OFFSET_C36: Assumed metadata storage start address on node C36, same above
    3. META_SIZE: Assumed metadata storage length, default: 2MB
    4. DATA_OFFSET_C35: Start address for store data on node C35
    5. DATA_OFFSET_C36: Start address for store data on node C36
*/
#define META_OFFSET_C35 0xae40200000
#define META_OFFSET_C36 0
#define META_SIZE 2097152
#define DATA_OFFSET_C35 (META_OFFSET_C35 + META_SIZE)
#define DATA_OFFSET_C36 (META_OFFSET_C36 + META_SIZE)

// External declarations
extern char *base_addr_devdax;
extern int pmem_fd;

#endif // PMEM_IO_REDIRECTION_H