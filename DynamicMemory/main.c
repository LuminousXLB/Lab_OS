#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define CYCLE
#define VIRTUL_MEMORY_SIZE 0x1000

#define _error_(msg) fprintf(stdout, "\e[0;31m[%6d] %s: %s\n\e[0m", __LINE__, __FUNCTION__, msg)


typedef struct map {
    unsigned m_size;
    char *m_addr;
    struct map *next, *prior;
} map_st;

map_st *core_map;

void mem_init(unsigned size) {
    core_map = (map_st *) malloc(sizeof(map_st));

    map_st *head = (map_st *) malloc(sizeof(map_st));
    head->m_size = size;
    head->m_addr = NULL + 0x100;
    head->prior = core_map;
    head->next = NULL;

    core_map->m_size = size;
    core_map->m_addr = NULL + 0x100;
    core_map->prior = NULL;
    core_map->next = head;
}

void mem_free() {
    if (core_map->next) {
        map_st *head = core_map->next;

        while (head->next) {
            head = head->next;
            free(head->prior);
        }
        free(head);

        core_map->next = NULL;
    }
}

map_st *generate_after(map_st *head) {
    map_st *node = malloc(sizeof(map_st));

    node->next = head->next;
    if(head->next) {
        head->next->prior = node;
    }
    head->next = node;
    node->prior = head;

    return node;
}

void remove_at(map_st *head) {
    head->prior->next = head->next;
    if (head->next) {
        head->next->prior = head->prior;
    }
    free(head);
}

unsigned lmalloc(unsigned size) {
    map_st *head = core_map->next;

    while (head && head->m_size < size) {
        head = head->next;
    }

    if (!head) {
        _error_("allocate failed");
        return -1;
sssss    }

    assert(head->m_size >= size);

    char *alloc = head->m_addr;

    if (head->m_size == size) {
        remove_at(head);
    } else {
        head->m_addr += size;
        head->m_size -= size;
    }

    return alloc - core_map->m_addr;
}

void _lfree_merge(map_st *prior, map_st *next, char *addr, unsigned size) {
    char *block_end = addr + size;

    if (prior != core_map && addr == prior->m_addr + prior->m_size) {
        if (next && block_end == next->m_addr) {
            prior->m_size += size + next->m_size;
            remove_at(next);
        } else {
            prior->m_size += size;
        }
    } else if (next && block_end == next->m_addr) {
        next->m_addr = addr;
        next->m_size += size;
    } else {
        map_st *new = generate_after(prior);
        new->m_addr = addr;
        new->m_size = size;
    }
}

bool lfree(unsigned offset, unsigned size) {
    char *addr = core_map->m_addr + offset;

    map_st *prior = NULL, *next = NULL;

    next = core_map->next;
    if (!next) {
        if (offset + size < core_map->m_size) {
            map_st *new = generate_after(core_map);
            return true;
        } else {
            _error_("this block of memory is invalid");
            return false;
        }
    }

    if (addr < next->m_addr) {
        // free before the first
        if (addr < core_map->m_addr) {
            _error_("this block of memory is invalid");
            return false;
        }

        if (addr + size > next->m_addr) {
            _error_("this block of memory overlays to the next");
            return false;
        }

        _lfree_merge(core_map, next, addr, size);
        return true;
    }

    while (next && addr > next->m_addr) {
        prior = next;
        next = prior->next;
    }

//    if (last->m_addr < addr) {
//        next = last->next;
//        while (next->m_addr < addr) {
//            next = next->next;
//        }
//        prior = next->prior;
//    } else if (last->m_addr > addr) {
//        prior = last->prior;
//        while (prior->m_addr > addr) {
//            prior = prior->prior;
//        }
//        next = prior->next;
//    }

    if (!next) {
        // free after the last
        if (addr + size > core_map->m_addr + core_map->m_size) {
            _error_("this block of memory is beyond the end");
            return false;
        }

        if (addr < prior->m_addr + prior->m_size) {
            _error_("this block of memory overlays to the prior");
            return false;
        }

        _lfree_merge(prior, NULL, addr, size);
        return true;
    }

    if (addr == next->m_addr) {
        _error_("this block of memory hasn't been allocated");
        return false;
    }

//    if (!prior || !next || prior->m_addr == addr || next->m_addr == addr) {
//        _error_("this block of memory hasn't been allocated");
//        return false;
//    }

    if (addr < prior->m_addr + prior->m_size) {
        _error_("this block of memory overlays to the prior");
        return false;
    }

    if (addr + size > next->m_addr) {
        _error_("this block of memory overlays to the next");
        return false;
    }

    _lfree_merge(prior, next, addr, size);
    return true;
}

void printCoreDump() {
    printf("%s\n", "------------------------------");
    map_st *head = core_map->next;
    while (head) {
        printf("\t%6lu\t%6u\t%6lu\n", head->m_addr - core_map->m_addr, head->m_size,
               head->m_addr - core_map->m_addr + head->m_size);
        head = head->next;
    }
    printf("%s\n", "==============================");
}

void work() {
    char input_buf[128];
    char cmd_buf[16];
    unsigned size, offset;

    char *ptr = NULL;

    while (fgets(input_buf, 128, stdin)) {
//        printf("%d: %s", __LINE__, input_buf);
        ptr = input_buf;
        while (ptr && isspace(*ptr)) {
            ptr++;
        }
        switch (*ptr) {
            case 'm':
                sscanf(ptr, "%s %i", cmd_buf, &size);
                offset = lmalloc(size);
                if (offset < core_map->m_size) {
                    printf("m\t%6u\t%6u\n", offset, size);
                }
                printCoreDump();
                break;
            case 'f':
                sscanf(ptr, "%s %i %i", cmd_buf, &size, &offset);
                if (lfree(offset, size)) {
                    printf("f\t%6u\t%6u\n", offset, size);
                }
                printCoreDump();
                break;
            case 'e':
                return;
            default:
                continue;
        }
        fflush(stdout);
    }
}

int main() {
    mem_init(VIRTUL_MEMORY_SIZE);

    freopen("/home/lx/lab/mytest1.in", "r", stdin);
    work();

    mem_free();
}

