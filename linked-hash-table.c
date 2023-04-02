#include "linked-hash-table.h"
#include <strings.h>

/* TODO:
 * 1 - Create an iterator (strtok-like).
 *  1.1 - Change everything on the main to use it.
 * 2 - Delete functions (by key or all).
 */

lht_t* lht_init() {
    lht_t* new = (lht_t*)malloc(sizeof(lht_t));
    if (!new) {
        fprintf(stderr, "couldn't get memory for the new hash table!\n");
        return NULL;
    }
    new->raw = malloc(sizeof(void*) * INIT_HASH);
    if (!new->raw) {
        fprintf(stderr, "couldn't get memory for the new hash table!\n");
        return NULL;
    }
    memset(new->raw, 0, sizeof(void*) * INIT_HASH);
    new->size = 0;
    new->capacity = INIT_HASH;
    new->first = NULL;
    new->last = NULL;
    return new;
}

/*
 * hash function for a string (djb2)
 */
unsigned long calculate_hash(const char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash % INIT_HASH;
}

/*
 * insert a new element into the linked hash table.
 * key must have the same lifetime as the value (e.g. a pointer to an attribute
 * of the value).
 * WARNING: optimistic: assumes there will never be any collisions.
 */
int lht_insert_new_element(lht_t* self, const char* key, void* value) {
    unsigned long index = calculate_hash(key);
    lht_node_t* new = malloc(sizeof(lht_node_t));
    if (!new) {
        fprintf(stderr, "couldn't get memory for the new hash table node!\n");
        return -1;
    }
    if (self->raw[index]) {
        fprintf(stderr, "TODO not messing with collisions for now!\n");
        return -1;
    }

    new->key = key;
    new->value = value;

    self->raw[index] = new;

    if (!self->first) {
        self->first = self->last = new;
        new->prev = NULL;
    } else {
        self->last->next = new;
        new->prev = self->last;
        self->last = new;
    }
    new->next = NULL;

    return 0;
}

void* lht_get_element(lht_t* self, const char* key) {
    unsigned long index = calculate_hash(key);
    return (self->raw[index]) ? self->raw[index]->value : NULL;
}

lht_node_t* lht_iterator_current = NULL;

/*
 * linked-hash-table iterator. works similarly to strtok.
 * if given a table, it'll return, if it exists, the first element of the table.
 * if given a NULL, it'll give the next element of the previous iteration.
 * returns a pointer to the next element or NULL if it reached the end.
 */
void* lht_iter(const lht_t* table) {
    lht_node_t* next;
    if (!table)
        next = lht_iterator_current->next;
    else
        next = table->first;
    lht_iterator_current = next;
    return next->value;
}
