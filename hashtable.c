#include "hashtable.h"

unsigned int hash_function(void *a)
{
	/*
	 * Credits: http://www.cse.yorku.ca/~oz/hash.html
	 */
	unsigned char *puchar_a = (unsigned char *)a;
	unsigned long hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

	return hash;
}

/*
 * Functie apelata dupa alocarea unui hashtable pentru a-l initializa.
 * Trebuie alocate si initializate si listele inlantuite.
 */
int init_ht(HashTable *ht, int hmax)
{
	int i;

	// aloc un vector de liste
	ht->buckets = malloc(hmax * sizeof(LinkedList));
	if (ht->buckets == NULL)
		return -ENOMEM;

	for (i = 0; i < hmax; i++) {
		// o initializez fiecare lista din vector
		init_list(&ht->buckets[i]);
	}

	ht->hmax = hmax;
	return SUCCESS;
}

// Intoarce -ENOMEM sau SUCCESS
int put_value(HashTable *ht, char *key, char *value)
{
	int index, is_present, err;
	Node *curr;

	index = hash_function(key) % ht->hmax;
	is_present = has_key(ht, key);

	// daca nu l-am gasit trebuie adaugat
	if (is_present == 0) {
		// adaug nodul in lista buckets[index] la final
		err = add_node(&ht->buckets[index], key, value);
		if (err != SUCCESS)
			return err;

	} else {
		// actualizez value de la respectiva key
		curr = get_entry(ht, key);
		free(curr->value);
		curr->value = malloc((strlen(value) + 1) * sizeof(char));
		if (curr->value == NULL)
			return -ENOMEM;
		strcpy(curr->value, value);
	}

	return SUCCESS;
}

char *get_value(HashTable *ht, char *key)
{
	Node *curr;

	curr = get_entry(ht, key);
	if (curr != NULL)
		return curr->value;
	else
		return NULL;
}

Node *get_entry(HashTable *ht, char *key)
{
	int index;
	Node *curr;

	// calculez indexul listei pe baza key
	index = hash_function(key) % ht->hmax;
	// ma plasez cu curr la inceputul listei cu indexul specific
	curr = ht->buckets[index].head;

	// verific fiecare element din lista
	while (curr != NULL) {
		if (strcmp(curr->key, key) == 0)
			return curr;
		curr = curr->next;
	}

	return NULL;
}

// Functie care intoarce:
// 1, daca pentru cheia key a fost asociata anterior o valoare
// 0, altfel.
int has_key(HashTable *ht, char *key)
{
	int index;
	Node *curr;

	// calculez indexul listei pe baza key
	index = hash_function(key) % ht->hmax;
	// ma plasez cu curr la inceputul listei cu indexul specific
	curr = ht->buckets[index].head;

	// verific fiecare element din lista
	while (curr != NULL) {
		if (strcmp(curr->key, key) == 0)
			return 1;
		curr = curr->next;
	}

	return 0;
}

// Elimina din hashtable intrarea asociata cheii key
void remove_entry(HashTable *ht, char *key)
{
	int index;

	// calculez indexul pe baza key
	index = hash_function(key) % ht->hmax;
	remove_node(&ht->buckets[index], key);
}

// Elibereaza memoria asociata hashtable-ului
void free_ht(HashTable *ht)
{
	int i;

	// parcurg vectorul de buckets
	for (i = 0; i < ht->hmax; i++) {
		// eliberez fiecare lista, daca nu e goala
		free_list(&ht->buckets[i]);
	}

	// eliberez vectorul
	free(ht->buckets);
	// eliberez hashtable
	free(ht);
}
