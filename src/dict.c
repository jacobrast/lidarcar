#include <stdio.h>
#include <string.h>

#include "chtbl.h"
#include "hashpjw.h"

typedef struct {
	void *key;
	void *value;
} entry;

int dict_cmp(const void *key1, const void *key2)
{
	return strcmp(key1, ((entry *)key2)->key);
}

int dict_hash(const void *key)
{
	return hashpjw(((entry *)key)->key);
}

int dict_init(CHTbl *table, int size)
{
	return chtbl_init(table, size, dict_hash, dict_cmp, NULL);
}

int dict_insert(CHTbl *table, entry *item)
{
	return chtbl_insert(table, item);
}

int dict_lookup(CHTbl *table, void *key, entry **item)
{
	int rc;
	entry lookup, *lookup_p;

	lookup_p = &lookup;
	lookup.key = key;

	rc = chtbl_lookup(table, &lookup_p);
	if (rc == 0){
		*item = lookup_p;
	}

	return rc;
}

//TODO: dict_set
//TODO: dict_remove
//TODO: dict_destroy
