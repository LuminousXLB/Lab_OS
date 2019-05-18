#define VIRTUL_MEMORY_SIZE 0x1000

#include <stdio.h>

#include "lmem.h"

/******* Wrapper for lmalloc and lfree ****************************************/

#if 0

char *lmalloc(map_ctl_st *status, unsigned long size);

bool lfree(map_ctl_st *status, unsigned long size, char *addr);

#endif

map_ctl_st *status;

unsigned long lmalloc_off(unsigned long size) {
    char *mem = lmalloc(status, size);
    return mem ? mem - status->core_map->m_addr : -1;
}

bool lfree_offset(unsigned long size, unsigned long offset) {
    return lfree(status, size, status->core_map->m_addr + offset);
}

/******************************************************************************/

/* print the details of vacant partition table */
void printCoreDump() {
    map_st *core_map = status->core_map;

    printf("%s\n", "------------------------------");
    map_st *head = core_map->next;
    while (head) {
        printf("\t%6lu\t%6lu\t%6lu\n", head->m_addr - core_map->m_addr, head->m_size,
               head->m_addr - core_map->m_addr + head->m_size);
        head = head->next;
    }

#ifdef NEXT_FIT
    map_st *last = status->last;
    printf("%s\n", "------------------------------");
    printf("last\t%6lu%6lu\n", last->m_addr - core_map->m_addr, last->m_size);
#endif

    printf("%s\n", "==============================");
}


int main() {
    status = lmem_init(VIRTUL_MEMORY_SIZE);
    map_st *core_map = status->core_map;

    char input_buf[128];
    char cmd_buf[16];
    unsigned long size, offset;

    char *ptr = NULL;

    while (fgets(input_buf, 128, stdin)) {
        // printf("%d: %s", __LINE__, input_buf);
        ptr = input_buf;
        while (ptr && isspace(*ptr)) {
            ptr++;
        }
        switch (*ptr) {
            case 'm':
                sscanf(ptr, "%s %li", cmd_buf, &size);
                offset = lmalloc_off(size);
                if (offset < core_map->m_size) {
                    printf("m\t%6lu\t%6lu\n", offset, size);
                }
                printCoreDump();
                break;
            case 'f':
                sscanf(ptr, "%s %li %li", cmd_buf, &size, &offset);
                if (lfree_offset(size, offset)) {
                    printf("f\t%6lu\t%6lu\n", offset, size);
                }
                printCoreDump();
                break;
            case 'e':
                exit(EXIT_SUCCESS);
            default:
                continue;
        }
        fflush(stdout);
    }
    lmem_free(status);
}
