#ifndef LHT_HEADER
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* just a decently large prime number */
#define INIT_HASH 65537

typedef struct lht_node {
    const char* key;
    void* value;
    struct lht_node* next;
    struct lht_node* prev;
} lht_node_t;

typedef struct lht {
    lht_node_t** raw;
    size_t size;
    size_t capacity;
    lht_node_t* first;
    lht_node_t* last;
    lht_node_t* lht_iterator_current;
} lht_t;

typedef enum {
    BEGIN,
    KEEP
} iter_setting;

lht_t* lht_init();
int lht_insert_new_element(lht_t* table, const char* key, void* value);
void* lht_get_element(lht_t* self, const char* key);
void* lht_iter(lht_t* table, iter_setting setting);

#endif /* !LHT_HEADER */
