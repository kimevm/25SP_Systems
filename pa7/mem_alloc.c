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
