#ifndef LHT_HEADER
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/cdefs.h>

/* just a decently large prime number */
#define INIT_HASH 65537

typedef struct lht_node {
    const char* key;
    void* value;
    struct lht_node* next;
} lht_node_t;

typedef struct lht {
    lht_node_t** raw;
    size_t size;
    size_t capacity;
    lht_node_t* first;
    lht_node_t* last;
} lht_t;

lht_t* lht_init();
int lht_insert_new_element(lht_t* table, const char* key, void* value);
void* lht_get_element(lht_t* self, const char* key);

#endif /* !LHT_HEADER */
