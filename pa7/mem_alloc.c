/*
 * mem_alloc.c
 * A simple dynamic memory allocator using a free list
 * Author: Lawrence Kim - kimevm@bc.edu, Nicholas Hernandez - hernantx@bc.edu
 */

#include <sys/mman.h>
#include <stdio.h>

#include "mem_alloc.h"

Header *free_list = NULL;

int is_allocated(Header *h);
int is_free(Header *h);
size_t get_size(Header *h);
void set_allocated(Header *h);
void set_free(Header *h);
Header *get_header(void *mem);
int same_page(Header *h1, Header *h2);

static int mem_init(void);
static int mem_extend(Header *last);



static int mem_init(void) {
    void *page = mmap(NULL, PAGE_SIZE, PROT_REAED | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page == MAP_FAILED) {
        return FAILURE;
    }
    
    free_list = (Header *)page;
    free_list->size = PAGE_SIZE - sizeof(Header);
    free_list->next = NULL;
    free_list->prev = NULL;
    set_free(free_list);

    return SUCCESS;
}

static int mem_extend(Header * last) {
    void *page = mmap(NULL, PAGE_SIZE, PROT_REAED | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page == MAP_FAILED) {
        return FAILURE;
    }

    Header *h = (Header *)page;
    h->size = PAGE_SIZE - sizeof(Header);
    h->next = NULL;
    h->prev = last;
    set_free(h);

    last->next = h;
    return SUCCESS;
}

void * mem_alloc(size_t requested_size) {
    if (requested_size > PAGE_SIZE - sizeof(Header)) {
        return NULL;
    }

    size_t aligned = ((requested_size + WORD_SIZE - 1) / WORD_SIZE) * WORD_SIZE;

    if (free_list == NULL) {
        if (mem_init() == FAILURE) {
            return NULL;
        }
    }

    if (is_allocated(h) || get_size(h) < aligned) {
        if (mem_extend(h) == FAILURE) {
            return NULL;
        }
        h = h->next;
    }

    size_t total_size = get_size(h);
    if (total_size > aligned + sizeof(Header)) {
        Header * new_hdr = (Header *)((char *)h + sizeof(Header) + aligned);
        new_hdr->size = total_size - aligned - sizeof(Header);
        new_hdr->next = h->next;
        new_hdr->prev = h;
        if (h->next) {
            h->next->prev = new_hdr;
        }
        h->next = new_hdr;
        set_free(new_hdr);

        h->size = aligned;
    }

    set_allocated(h);
    return (void *)((char *)h + sizeof(Header));
}

void mem_free(void * ptr) {
    if (ptr == NULL) {
        return;
    }

    Header * h = get_header(ptr);
    set_free(h);

    Header * n = h->next;
    if (n && is_free(n) && same_page(h,n)) {
        h->size = get_size(h) + sizeof(Header) + get_size(n);
        h->next = n->next;
        if (n->next) {
            n->next->prev = h;
        }
    }

    Header * p = h->prev;
    if (p && is_free(p) && same_page(p, h)) {
        p->size = get_size(p) + sizeof(Header) + get_size(h);
        p->next = h->next;
        if(h->next) {
            h->next->prev = p;
        }
        h = p;
    }

    if (get_size(h) == PAGE_SIZE - sizeof(Header)) {
        Header * prev = h->prev;
        Header * next = h->next;

        if (prev) {
            prev->next = next;
        }
        if (next) {
            next->prev = prev;
        }
        if (h == free_list) {
            free_list = next;
        }

        munmap((void *)h, PAGE_SIZE);

        if (prev == NULL && next == NULL) {
            free_list = NULL;
        }
    }
}

/*Print functions need work(Delete this comment later)*/

void print_header(Header * h) {
}


void print_list(void) {
}


/* Helper Implementation (Delete this comment later)*/

int is_allocated(Header *h) {
    return (h->size & 1);
}

int is_free(Header *h) {
    return !is_allocated(h);
}

size_t get_size(Header *h) {
    return (h->size & ~(size_t)1);
}

void set_allocated(Header *h) {
    h->size |= 1;
}

void set_free(Header *h) {
    h->size &= ~(size_t)1;
}

Header *get_header(void *mem) {
    return (Header *)((char *)mem - sizeof(Header));
}

int same_page(Header *h1, Header *h2) {
    const size_t mask = ~(((size_t)1 << 12) - 1);
    return (((size_t)h1 & mask) == ((size_t)h2 & mask));
}
