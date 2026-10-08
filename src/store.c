#include "store.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *copy_text(const char *text)
{
    size_t length = strlen(text);
    char *copy = malloc(length + 1);

    if (copy == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < length; i++)
    {
        copy[i] = text[i];
    }
    copy[length] = '\0';
    return copy;
}

static struct store_item *find_item(const struct store *store, const char *key)
{
    struct store_item *item = store->first;

    while (item != NULL)
    {
        if (strcmp(item->key, key) == 0)
        {
            return item;
        }
        item = item->next;
    }
    return NULL;
}

void store_init(struct store *store) { store->first = NULL; }

void store_free(struct store *store)
{
    struct store_item *item = store->first;

    while (item != NULL)
    {
        struct store_item *next = item->next;
        free(item->key);
        free(item->value);
        free(item);
        item = next;
    }
    store->first = NULL;
}

int store_set(struct store *store, const char *key, const char *value)
{
    struct store_item *item = find_item(store, key);
    char *new_value = copy_text(value);

    if (new_value == NULL)
    {
        return -1;
    }

    if (item != NULL)
    {
        free(item->value);
        item->value = new_value;
        return 0;
    }

    item = malloc(sizeof(struct store_item));
    if (item == NULL)
    {
        free(new_value);
        return -1;
    }

    item->key = copy_text(key);
    if (item->key == NULL)
    {
        free(new_value);
        free(item);
        return -1;
    }

    item->value = new_value;
    item->next = store->first;
    store->first = item;
    return 0;
}

const char *store_get(const struct store *store, const char *key)
{
    struct store_item *item = find_item(store, key);

    if (item == NULL)
    {
        return NULL;
    }
    return item->value;
}

int store_delete(struct store *store, const char *key)
{
    struct store_item *previous = NULL;
    struct store_item *item = store->first;

    while (item != NULL)
    {
        if (strcmp(item->key, key) == 0)
        {
            if (previous == NULL)
            {
                store->first = item->next;
            }
            else
            {
                previous->next = item->next;
            }
            free(item->key);
            free(item->value);
            free(item);
            return 1;
        }
        previous = item;
        item = item->next;
    }
    return 0;
}

int store_exists(const struct store *store, const char *key)
{
    if (find_item(store, key) == NULL)
    {
        return 0;
    }
    return 1;
}

int store_increment(struct store *store, const char *key, char *result,
                    size_t result_size)
{
    const char *old_value = store_get(store, key);
    char *end = NULL;
    long number = 0;

    if (old_value != NULL)
    {
        number = strtol(old_value, &end, 10);
        if (*old_value == '\0' || *end != '\0' || number == LONG_MAX)
        {
            return -1;
        }
    }

    number++;
    if (snprintf(result, result_size, "%ld", number) < 0 ||
        store_set(store, key, result) < 0)
    {
        return -1;
    }
    return 0;
}
