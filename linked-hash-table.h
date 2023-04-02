#ifndef LHT_HEADER

/* just a decently large prime number */
#define INIT_HASH 65537

typedef struct lht_node {
    const char* key;
    void* value;
    struct lht_node* next;
} lht_node_t;

int lht_insert_new_element(void** table, const char* key, void* value);
void** linked_hash_table_init();
void* get_element(void** self, const char* key);

#endif /* !LHT_HEADER */
