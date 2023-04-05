#include "linked-hash-table.h"

/* TODO:
 * 1 - Delete functions (by key or all).
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

    /* adding to the hash table */
    self->raw[index] = new;

    /* and linking */
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

/*
 * linked-hash-table iterator.
 * returns a pointer to the next element or NULL if it reached the end.
 * if used the BEGIN flag, it'll go to the beggining of the table.
 * if used the KEEP flag, it'll keep going where from it was.
 */
void* lht_iter(lht_t* table, iter_setting setting) {
    lht_node_t* next;
    if (setting == KEEP)
        next = table->lht_iterator_current->next;
    else
        next = table->first;
    table->lht_iterator_current = next;
    return (next) ? next->value : NULL;
}
