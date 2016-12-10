#include <stddef.h>
#include <stdlib.h>
#include "../inc/ramfs.h"
#include "../inc/alloc.h"
#include "../inc/print.h"

int strcmp(const char *a,const char *b){
    if (! (*a | *b)) return 0;
    return (*a!=*b) ? *a-*b : strcmp(++a,++b);
}

int get_empty_file_slot() {
    int i = 0;
    for (;FILE_TABLE[i].state != DELETED; ++i){} //todo handle out of range
    return i;
}

void create(const char * pathname){
    struct fsnode * first_file_node = (struct fsnode *) mem_alloc(sizeof(struct fsnode));
    first_file_node->prev = NULL;
    first_file_node->next = NULL;
    int empty_file_slot_index = get_empty_file_slot();
    FILE_TABLE[empty_file_slot_index].state = CLOSED;
    FILE_TABLE[empty_file_slot_index].byte_size = BLOCK_SIZE;
    FILE_TABLE[empty_file_slot_index].pathname = pathname;
    FILE_TABLE[empty_file_slot_index].start = first_file_node;
}

int find_file(const char * pathname) {
    int i = 0;
    for (;strcmp(FILE_TABLE[i].pathname, pathname) != 0
          && FILE_TABLE[i].state == DELETED; ++i){}

    return i;
}

struct FILE * open(const char * pathname) {

    int i = find_file(pathname);

    //todo handle out of range

    FILE_TABLE[i].state = OPENED;

    return &FILE_TABLE[i];
}

void close(struct FILE * file) {
    file->state = CLOSED;
}


void throw_ex(const char * what) {
    printf("\n");
    printf(what);
    printf("\n");
    int a = 1;
    int b = 1 / (a - 1);
    (void *) b;
}

struct fsnode * find_node_by_position(struct FILE * file, size_t * pos) {
    struct fsnode * node = file->start;

    while (*pos - BLOCK_SIZE > 0 && node->next != NULL) {
        printf("cycle\n");
        node = node->next;
        *pos -= BLOCK_SIZE;
    }

    return node;
}

char readchar(struct FILE * file, size_t shift) {
    if (file->state != OPENED) throw_ex("trying to open closed file");
    if (file->byte_size <= shift) throw_ex("trying to read file out of range");

    //todo add current_reading to improve sequential reading


    struct fsnode * block_shift = find_node_by_position(file, &shift);
    
    return block_shift->data[shift];
}


void writechar(struct FILE * file, char value) {
    if (file->state != OPENED) throw_ex("trying to write to closed file");
    struct fsnode * block_shift = find_node_by_position(file, &file->byte_size);


    // тут надо будет увелличивать файл, искать конец, и тд
}