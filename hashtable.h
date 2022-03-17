#ifndef __HASHTABLE_H
#define __HASHTABLE_H

#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
// #include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include "linkedlist.h"

#define ENOMEM 12
#define SUCCESS 0
#define MAX_BUCKETS 10

typedef struct HashTable {
    LinkedList *buckets; /* Array de liste simplu-inlantuite. */
    int hmax;            /* Nr. de bucket-uri. */
} HashTable;


unsigned int hash_function_string(void *a);

int init_ht(HashTable *ht, int hmax);

int put_value(HashTable *ht, char *key, char *value);

Node* get_entry(HashTable *ht, char *key);

char* get_value(HashTable *ht, char *key);

int has_key(HashTable *ht, char *key);

void remove_entry(HashTable *ht, char *key);

void free_ht(HashTable *ht);

void print_ht(HashTable *ht);

int get_ht_hmax(HashTable *ht);

#endif /* __HASHTABLE_H__ */