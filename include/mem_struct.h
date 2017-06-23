#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <malloc.h>
#include <unistd.h>

#include <errno.h>

typedef struct mcs {
        size_t size;
	short int stat;

        struct mcs* mp_next;
        struct mcs* mp_prev;
} MEM_CHUNK_STRUCT;

/*typedef struct mcl {
        MEM_CHUNK_STRUCT chunk;

        struct fil *next;
        struct fil *prev;
} MEM_CHUNK_LIST;

        1. 0 - 10
        2. 11 - 100
        3. 101 - 1024
        4. 1025 - 10240
        5. > 10240

MEM_CHUNK_LIST mem_alloc_list[5];*/

MEM_CHUNK_STRUCT *mem_head = NULL, *mem_tail = NULL;
int *hlb = NULL, *hub = NULL;

