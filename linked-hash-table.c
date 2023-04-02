#include "linked-hash-table.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/cdefs.h>

/* TODO:
 * 1 - Missing getters.
 *  1.1 - Change everything on the main to use them.
 * 2 - Create an iterator (strtok-like).
 * 3 - Delete functions (by key or all).
 */

void** linked_hash_table_init() {
    void** new = malloc(sizeof(void*) * INIT_HASH);
    memset(new, 0, sizeof(void*) * INIT_HASH);
    if (!new) {
        fprintf(stderr, "couldn't get memory for the new hash table!\n");
        return NULL;
    }
    return new;
}

/*
 * hash function for a string (djb2)
 */
unsigned long calculate_hash(const char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % INIT_HASH;
}

/*
 * insert a new element into the linked hash table.
 * key must have the same lifetime as the value (e.g. a pointer to an attribute
 * of the value).
 * WARNING: optimistic: assumes there will never be any collisions.
 */
int lht_insert_new_element(void** self, const char* key, void* value) {
    unsigned long index = calculate_hash(key);
    lht_node_t* new = malloc(sizeof(lht_node_t));
    if (!new) {
        fprintf(stderr, "couldn't get memory for the new hash table node!\n");
        return -1;
    }

    if (self[index]) {
        fprintf(stderr, "TODO not messing with collisions for now!\n");
        return -1;
    }

    new->key = key;
    new->value = value;
    self[index] = new;
    return 0;
}

void* get_element(void** self, const char* key) {
    unsigned long index = calculate_hash(key);
    return (self[index]) ? ((lht_node_t*)self[index])->value : NULL;
}
