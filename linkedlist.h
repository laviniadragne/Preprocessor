#ifndef __LINKEDLIST_H
#define __LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>    
// #include <unistd.h>     
#include <string.h>    

#define ENOMEM 12
#define SUCCESS 0

// typedef struct Pair {
//     char* key;      // first
//     char* value;    // second
// } Pair;

typedef struct Node {
    char* key;      // first
    char* value;    // second
    struct Node *next;
} Node;

typedef struct LinkedList {
    struct Node *head;
    struct Node *tail;
} LinkedList;

// int create_list(LinkedList* l);

void init_list(LinkedList *list);

int init_node(Node* new_node, char* key, char* value);

int add_node(LinkedList *list, char* key, char* value);

void remove_node(LinkedList *list, char* key);

void free_node(Node* node);

void free_list(LinkedList *list);

void print(LinkedList *list);

#endif /* __LINKEDLIST_H__ */