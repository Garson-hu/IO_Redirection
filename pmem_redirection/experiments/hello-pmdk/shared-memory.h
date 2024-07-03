#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#define SHM_KEY 1234

typedef struct {
    int values[3];
} Data;

#endif /* SHARED_MEMORY_H */
