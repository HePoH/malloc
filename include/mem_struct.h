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

MEM_CHUNK_STRUCT *mem_head = NULL, *mem_tail = NULL;
int *hlb = NULL, *hub = NULL;

