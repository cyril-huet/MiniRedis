#ifndef STORE_H
#define STORE_H

#include <stddef.h>

/* One key and its value. */
struct store_item
{
    char *key;
    char *value;
    struct store_item *next;
};

/* The data kept by the server. */
struct store
{
    struct store_item *first;
};

void store_init(struct store *store);
void store_free(struct store *store);
int store_set(struct store *store, const char *key, const char *value);
const char *store_get(const struct store *store, const char *key);
int store_delete(struct store *store, const char *key);
int store_exists(const struct store *store, const char *key);
int store_increment(struct store *store, const char *key, char *result,
                    size_t result_size);

#endif /* ! STORE_H */
