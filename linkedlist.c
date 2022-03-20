#include "linkedlist.h"

void init_list(LinkedList *list)
{
	list->head = NULL;
	list->tail = NULL;
}

// Initializeaza un nod cu key-ul si value-ul dat
// Il aloca si ii face next-ul sa pointeze la NULL
int init_node(Node *new_node, char *key, char *value)
{
	new_node->key = malloc((strlen(key) + 1) * sizeof(char));
	if (new_node->key == NULL)
		return -ENOMEM;

	strcpy(new_node->key, key);

	new_node->value = malloc((strlen(value) + 1) * sizeof(char));
	if (new_node->value == NULL) {
		free(new_node->key);
		return -ENOMEM;
	}
	strcpy(new_node->value, value);

	new_node->next = NULL;

	return SUCCESS;
}

// adauga la finalul liste
// intoarce 12 in caz de eroare si
// 0 in caz de succes
int add_node(LinkedList *list, char *key, char *value)
{
	int err;
	Node *new_node;

	new_node = (Node *)malloc(sizeof(Node));

	if (new_node == NULL)
		return -ENOMEM;

	err = init_node(new_node, key, value);
	if (err != SUCCESS) {
		free(new_node);
		return err;
	}

	// lista nu are niciun element
	if (list->head == NULL) {
		list->head = new_node;
		list->tail = new_node;
		return SUCCESS;
	}

	// adaug nodul la final, realizand legaturile
	list->tail->next = new_node;
	list->tail = new_node;

	return SUCCESS;
}

// sterge un nod cu continutul (key, value)
void remove_node(LinkedList *list, char *key)
{
	Node *prev, *curr;

	if (list == NULL)
		return;

	if (list->head == NULL)
		return;

	prev = list->head;
	curr = list->head->next;

	if (strcmp(list->head->key, key) == 0) {
		// am un singur element
		if (curr == NULL) {
			free_node(list->head);
			list->head = NULL;
			list->tail = NULL;
			return;
			// trebuia sa sterg head-ul
		} else {
			free_node(list->head);
			list->head = curr;
			return;
		}
	}

	while (curr != NULL) {
		if (strcmp(curr->key, key) == 0)
			break;
		prev = curr;
		curr = curr->next;
	}

	if (curr != NULL) {
		if (strcmp(curr->key, key) == 0) {
			// refac legatura
			prev->next = curr->next;

			// daca era ultimul nod din lista
			if (list->tail == curr)
				list->tail = prev;
			free_node(curr);
		}
	}
}

void free_node(Node *node)
{
	free(node->value);
	free(node->key);
	free(node);
}

void free_list(LinkedList *list)
{
	Node *curr, *prev;

	if (list == NULL)
		return;

	curr = list->head;
	while (curr != NULL) {
		prev = curr;
		curr = curr->next;
		free_node(prev);
	}
}
