#include "lmem.h"

/******* DECLARATION : Memory Management Utility Function *********************/

/* do the changes on blocks after a series of checks */
void _lfree_commit(map_ctl_st* status, map_st* prior, map_st* next, char* addr, unsigned long size);

/******* DECLARATION : Linked List Manipulating Function **********************/

/* generate a new block after head */
map_st* generate_after(map_st* head);

/* remove the block head points to */
void remove_at(map_st* head);

/******* IMPLEMENTATION : lmem_init *******************************************/
map_ctl_st* lmem_init(unsigned long size)
{
    map_st* core_map = (map_st*)malloc(sizeof(map_st));
    core_map->m_size = size;
    core_map->m_addr = malloc(size);
    core_map->prior = NULL;

    map_st* head = (map_st*)malloc(sizeof(map_st));
    head->m_size = size;
    head->m_addr = core_map->m_addr;
    head->prior = core_map;
    head->next = NULL;

    core_map->next = head;

    map_ctl_st* ptr = (map_ctl_st*)malloc(sizeof(map_ctl_st));

    ptr->core_map = core_map;
#ifdef NEXT_FIT
    ptr->last = head;
#endif

    return ptr;
}

/******* IMPLEMENTATION : lmem_free *******************************************/
void lmem_free(map_ctl_st* status)
{
    map_st* head = status->core_map;
    free(head->m_addr);

    while (head->next) {
        head = head->next;
        free(head->prior);
    }

    free(head);
    free(status);
}

/******* IMPLEMENTATION : lmalloc *********************************************/
char* lmalloc(map_ctl_st* status, unsigned long size)
{
    map_st* core_map = status->core_map;

#ifdef NEXT_FIT
    /* start from the block where placement happened just now */
    map_st* last = status->last;
    map_st* head = (last == core_map) ? NULL : last;
#else
    /* start from the first block */
    map_st* head = core_map->next;
#endif

    /* walk until `head == NULL` which means alloc failed
     *         or `head->m_size < size` which means search succeeded
     */
    while (head && head->m_size < size) {
        head = head->next;
    }

    if (!head) {
        /* traverse failed */
#ifdef NEXT_FIT
        /* return to the beginning ... */
        head = core_map->next;
        /* and continue walking */
        while (head != last && head->m_size < size) {
            head = head->next;
        }
    }

    if (head == last && head->m_size < size) {
#endif
        _error_("allocate failed");
        return NULL;
    }

    /* FOR DEBUG: ensure that we have found a block big enough */
    assert(head->m_size >= size);

    /* allocate right here */
    char* alloc = head->m_addr;

    if (head->m_size == size) {
        /* fit exactly, remove the block */

#ifdef NEXT_FIT
        /* update last */
        status->last = head->next;
#endif
        remove_at(head);

    } else {
        /* a little bigger, shrink the block */

#ifdef NEXT_FIT
        /* update last */
        status->last = head;
#endif
        head->m_addr += size;
        head->m_size -= size;
    }

    return alloc;
}

/******* IMPLEMENTATION : lfree ***********************************************/
bool lfree(map_ctl_st* status, unsigned long size, char* addr)
{
    /* get status parameters */
    map_st* core_map = status->core_map;
    const unsigned long _size = core_map->m_size;
    const char* _addr = core_map->m_addr;

    /* handlers */
    map_st *prior = NULL, *next = NULL;

    /* point to the first block */
    next = core_map->next;
    if (!next) {
        /* there's no vacant block */

        if (addr < _addr || addr + size >= _addr + _size) {
            _error_("this block is beyond the whole memory");
            return false;
        }

        _lfree_commit(status, core_map, NULL, addr, size);
        return true;
    }

    if (addr < next->m_addr) {
        /* free before the first */

        if (addr < _addr) {
            _error_("this block is beyond the whole memory");
            return false;
        }

        if (addr + size > next->m_addr) {
            _error_("this block of memory overlays to the next");
            return false;
        }

        _lfree_commit(status, core_map, next, addr, size);
        return true;
    }

    /* walk until `next` touch the end
     *         or `prior->m_addr` < `addr` <= `next->m_addr` */
    while (next && addr > next->m_addr) {
        prior = next;
        next = prior->next;
    }

    if (!next) {
        /* free after the last */

        if (addr + size > _addr + _size) {
            _error_("this block is beyond the whole memory");
            return false;
        }

        if (addr < prior->m_addr + prior->m_size) {
            _error_("this block of memory overlays to the prior");
            return false;
        }

        _lfree_commit(status, prior, NULL, addr, size);
        return true;
    }

    /* commonly free in the middle */

    if (addr == next->m_addr) {
        _error_("this block of memory hasn't been allocated");
        return false;
    }

    if (addr < prior->m_addr + prior->m_size) {
        _error_("this block of memory overlays to the prior");
        return false;
    }

    if (addr + size > next->m_addr) {
        _error_("this block of memory overlays to the next");
        return false;
    }

    _lfree_commit(status, prior, next, addr, size);

    return true;
}

/******* IMPLEMENTATION : Memory Management Utility Function ******************/

void _lfree_commit(map_ctl_st* status, map_st* prior, map_st* next, char* addr, unsigned long size)
{

    if (prior != status->core_map && addr == prior->m_addr + prior->m_size) {
        /* the 2nd criteria means the block to be freed is attached to the prior
         * the 1st criteria ensures that the prior isn't core_map base
         */

        /* expand the prior */
        prior->m_size += size;

        if (next && (addr + size) == next->m_addr) {
            /* the 2nd criteria means the block to be freed is attached to both of the ends
             * the 1st criteria ensures that the next isn't NULL
             */

            /* concatenate prior and next, expand prior and remove the next*/
            prior->m_size += next->m_size;
#ifdef NEXT_FIT
            if (status->last == next) {
                /* if `next` is exactly where `last` is, update last */
                status->last = next->next;
            }
#endif
            remove_at(next);
        }
    } else if (next && (addr + size) == next->m_addr) {
        /* the 2nd criteria means the block to be freed is attached to the next
         * the 1st criteria ensures that the next isn't NULL
         */

        /* expand the next */
        next->m_addr = addr;
        next->m_size += size;
    } else {
        /* generate a new block */
        map_st* new = generate_after(prior);
        new->m_addr = addr;
        new->m_size = size;
    }
}

/******* IMPLEMENTATION : Linked List Manipulating Function *******************/

map_st* generate_after(map_st* head)
{
    map_st* node = malloc(sizeof(map_st));

    node->next = head->next;
    if (head->next) {
        head->next->prior = node;
    }
    head->next = node;
    node->prior = head;

    return node;
}

void remove_at(map_st* head)
{
    head->prior->next = head->next;

    if (head->next) {
        head->next->prior = head->prior;
    }

    free(head);
}
