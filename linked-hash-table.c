#include "linked-hash-table.h"
#include <strings.h>

/* TODO:
 * 1 - Missing getters.
 *  1.1 - Change everything on the main to use them.
 * 2 - Create an iterator (strtok-like).
 * 3 - Delete functions (by key or all).
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

    if (!self->first) {
        self->first = new;
        self->last = new;
    }
    else {
        self->last->next = new;
        self->last = new;
    }

    new->key = key;
    new->value = value;
    self->raw[index] = new;
    new->next = NULL;
    return 0;
}

void* lht_get_element(lht_t* self, const char* key) {
    unsigned long index = calculate_hash(key);
    return (self->raw[index]) ? self->raw[index]->value : NULL;
}
