//
// Created by lx on 5/18/19.
//

#ifndef DYNAMICMEMORY_LMEM_H
#define DYNAMICMEMORY_LMEM_H

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define NEXT_FIT


#define _error_(msg) fprintf(stdout, "\e[0;31m[%6d] %s: %s\n\e[0m", __LINE__, __FUNCTION__, msg)


typedef struct map {
    unsigned long m_size;
    char *m_addr;
    struct map *next, *prior;
} map_st;


typedef struct map_ctl {
    map_st *core_map;

#ifdef NEXT_FIT
    map_st *last;
#endif
} map_ctl_st;


map_ctl_st *lmem_init(unsigned long size);

void lmem_free(map_ctl_st *status);

char *lmalloc(map_ctl_st *status, unsigned long size);

bool lfree(map_ctl_st *status, unsigned long size, char *addr);


#endif //DYNAMICMEMORY_LMEM_H
