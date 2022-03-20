#include "linkedlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUCKETS 10

typedef struct HashTable {
	LinkedList *buckets; /* Array de liste simplu-inlantuite. */
	int hmax;	     /* Nr. de bucket-uri. */
} HashTable;

unsigned int hash_function_string(void *a);

int init_ht(HashTable *ht, int hmax);

int put_value(HashTable *ht, char *key, char *value);

Node *get_entry(HashTable *ht, char *key);

char *get_value(HashTable *ht, char *key);

int has_key(HashTable *ht, char *key);

void remove_entry(HashTable *ht, char *key);

void free_ht(HashTable *ht);
