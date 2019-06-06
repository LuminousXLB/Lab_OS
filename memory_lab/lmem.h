#ifndef __DYNAMICMEMORY_LMEM_H__
#define __DYNAMICMEMORY_LMEM_H__

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define NEXT_FIT

/* memory map item */
typedef struct map {
    unsigned long m_size;
    char* m_addr;
    struct map *next, *prior;
} map_st;

/* memory map manager */
typedef struct map_ctl {
    map_st* core_map;

#ifdef NEXT_FIT
    map_st* last;
#endif
} map_ctl_st;

/* allocate memory and initialize map_ctl */
map_ctl_st* lmem_init(unsigned long size);

/* free the memory and map_ctl */
void lmem_free(map_ctl_st* status);

/* allocate a block of memory with size `size` */
char* lmalloc(map_ctl_st* status, unsigned long size);

/* free a block memory with size `size` at addr */
bool lfree(map_ctl_st* status, unsigned long size, char* addr);

/* print error */
#define _error_(msg) fprintf(stdout, "\e[0;31m[%6d] %s: %s\n\e[0m", __LINE__, __FUNCTION__, msg)

#endif //__DYNAMICMEMORY_LMEM_H__
