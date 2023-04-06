#include "linked-hash-table.h"
#include <stddef.h>
#include <string.h>
#include <strings.h>
#include <sys/cdefs.h>
#include <sys/types.h>

/* TODO:
 * 1 - Delete functions (by key or all).
 */

lht_t* lht_init() {
    lht_t* new = (lht_t*)malloc(sizeof(lht_t));
    int i;
    if (!new) {
        fprintf(stderr, "couldn't get memory for the new hash table!\n");
        return NULL;
    }
    new->raw = malloc(sizeof(void*) * INIT_HASH);
    if (!new->raw) {
        fprintf(stderr, "couldn't get memory for the new hash table!\n");
        return NULL;
    }
    for (i = 0; i < INIT_HASH; i++) {
        new->raw[i] = NULL;
    }
    memset(new->raw, 0, sizeof(void*) * INIT_HASH);
    new->size = 0;
    new->capacity = INIT_HASH;
    new->first = NULL;
    new->last = NULL;
    return new;
}

void lht_destroy(lht_t* self) {
    if (!self)
        return;

    free(self->raw);
    free(self);
}

/*
 * hash function for a string (djb2)
 */
size_t calculate_hash2(const char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash % INIT_HASH;
}

size_t calculate_hash1(const char* str) {
        unsigned long hash = 0;
        int c;

        while ((c = *str++))
            hash = hash * 31 + c;

    return hash % INIT_HASH;
}

__always_inline size_t rehash(const char* str, const size_t prev, int round) {
    return (prev + round * calculate_hash2(str)) % INIT_HASH;
}

__always_inline ssize_t lht_get_index(lht_t* self, const char* key) {
    size_t init, i;
    int round = 1;
    init = i = calculate_hash1(key);
    while (self->raw[i]) {
        if (!strcmp(self->raw[i]->key, key))
            return i;
        i = rehash(key, init, round);
        round++;
    }

    return -1;
}

void* lht_get_element(lht_t* self, const char* key) {
    ssize_t i = lht_get_index(self, key);

    return (i >= 0) ? self->raw[i]->value : NULL;
}

/*
 * insert a new element into the linked hash table.
 * key must have the same lifetime as the value (e.g. a pointer to an attribute
 * of the value).
 */
int lht_insert_element(lht_t* self, const char* key, void* value) {
    size_t init, i;
    lht_node_t* new = malloc(sizeof(lht_node_t));
    int round = 1;
    init = i = (size_t)calculate_hash1(key);
    if (!new) {
        fprintf(stderr, "couldn't get memory for the new hash table node!\n");
        return -1;
    }
    while (self->raw[i]) {
        i = rehash(key, init, round);
        round++;
    }

    new->i = i;
    new->key = key;
    new->value = value;

    /* adding to the hash table */
    self->raw[i] = new;
    self->size++;

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

/*
 * removes the entry from the lht.
 * returns a pointer to the value.
 */
void* lht_leak_element(lht_t* self, const char* key) {
    ssize_t i = lht_get_index(self, key);
    void* value;
    if (i < 0)
        return NULL;

    value = self->raw[i]->value;
    /* if it is the last */
    if (self->raw[i]->next)
        self->raw[i]->next->prev = self->raw[i]->prev;
    else
        self->last = self->raw[i]->prev;

    /* if it is the first */
    if (self->raw[i]->prev)
        self->raw[i]->prev->next = self->raw[i]->next;
    else
        self->first = self->raw[i]->next;

    free(self->raw[i]);
    self->raw[i] = NULL;

    if (--self->size == 0) {
        self->first = NULL;
        self->last = NULL;
    }

    return value;
}

size_t lht_get_size(lht_t* self) { return self->size; }

/*
 * linked-hash-table iterator.
 * returns a pointer to the next element or NULL if it reached the end.
 * if used the BEGIN flag, it'll go to the beggining of the table.
 * if used the KEEP flag, it'll keep going where from it was.
 */
void* lht_iter(lht_t* self, iter_setting setting) {
    lht_node_t* next;
    if (setting == KEEP)
        next = self->lht_iterator_current->next;
    else
        next = self->first;
    self->lht_iterator_current = next;
    return (next) ? next->value : NULL;
}

void* lht_pop_element(lht_t* self) {
    lht_node_t* tmp;
    void* corn; /* cus popcorn lol */

    if (!self->last)
        return NULL;

    self->raw[self->last->i] = NULL;
    corn = self->last->value;
    tmp = self->last->prev;
    free(self->last);
    self->last = tmp;

    self->size--;
    if (!self->size) {
        self->first = NULL;
        self->last = NULL;
    }

    return corn;
}
