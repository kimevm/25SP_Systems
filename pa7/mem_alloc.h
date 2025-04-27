/*
 * Author: Lawrence Kim - kimevm@bc.edu, Nicholas Hernandez - hernantx@bc.edu
 */

#include <stddef.h>

#define FAILURE -1
#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H
#define PAGE_SIZE 4096
#define SUCCESS 0
#define WORD_SIZE 8

typedef struct Header {
    size_t        size;
    struct Header * next;
    struct Header * previous;
} Header;
extern Header * free_list;
void * mem_alloc(size_t requested_size);
void mem_free(void * ptr);
#endif