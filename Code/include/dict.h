#ifndef DICT_H
#define DICT_H

#include "chtbl.h"

typedef struct {
	void *key;
	void *value;
} entry;

int dict_cmp(const void *key1, const void *key2);
int dict_hash(const void *key);
int dict_init(CHTbl *table, int size);
int dict_insert(CHTbl *table, entry *item);
int dict_lookup(CHTbl *table, void *key, entry **item);


#endif
