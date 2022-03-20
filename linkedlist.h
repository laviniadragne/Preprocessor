#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENOMEM -12
#define SUCCESS 0

typedef struct Node {
	char *key;
	char *value;
	struct Node *next;
} Node;

typedef struct LinkedList {
	struct Node *head;
	struct Node *tail;
} LinkedList;

void init_list(LinkedList *list);

int init_node(Node *new_node, char *key, char *value);

int add_node(LinkedList *list, char *key, char *value);

void remove_node(LinkedList *list, char *key);

void free_node(Node *node);

void free_list(LinkedList *list);
