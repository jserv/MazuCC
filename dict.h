#ifndef MAZUCC_DICT_H
#define MAZUCC_DICT_H

#include <stdlib.h>
#include <string.h>
#include "list.h"

typedef struct Dict {
    List *list;
    struct Dict *parent;
} Dict;

#define EMPTY_DICT ((Dict){&EMPTY_LIST, NULL})

typedef struct {
    char *key;
    void *val;
} DictEntry;

static inline void *make_dict(void *parent)
{
    Dict *r = malloc(sizeof(Dict));
    r->list = make_list();
    r->parent = parent;
    return r;
}

static inline void *dict_get(Dict *dict, char *key)
{
    for (; dict; dict = dict->parent) {
        for (Iter i = list_iter(dict->list); !iter_end(i);) {
            DictEntry *e = iter_next(&i);
            if (!strcmp(key, e->key))
                return e->val;
        }
    }
    return NULL;
}

static inline void dict_put(Dict *dict, char *key, void *val)
{
    DictEntry *e = malloc(sizeof(DictEntry));
    e->key = key;
    e->val = val;
    list_push(dict->list, e);
}

static inline List *dict_keys(Dict *dict)
{
    List *r = make_list();
    for (; dict; dict = dict->parent)
        for (Iter i = list_iter(dict->list); !iter_end(i);)
            list_push(r, ((DictEntry *) iter_next(&i))->key);
    return r;
}

static inline List *dict_values(Dict *dict)
{
    List *r = make_list();
    for (; dict; dict = dict->parent)
        for (Iter i = list_iter(dict->list); !iter_end(i);)
            list_push(r, ((DictEntry *) iter_next(&i))->val);
    return r;
}

static inline void *dict_parent(Dict *dict)
{
    void *r = dict->parent;
    list_free(dict->list);
    free(dict->list);
    free(dict);
    return r;
}

#endif /* MAZUCC_DICT_H */
