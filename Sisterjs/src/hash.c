#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#define TABLE_SIZE 100

unsigned int hash(const char *key) {
    unsigned long int hash_value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    for (; i < key_len; i++) {
        hash_value = hash_value * 37 + key[i];
    }
    return hash_value % TABLE_SIZE;
}

KeyValue *create_pair(const char *key, const char *value) {
    KeyValue *pair = malloc(sizeof(KeyValue));
    pair->key = strdup(key);
    pair->value = strdup(value); 
    pair->next = NULL;
    return pair;
}

void insert(HashTable *hashtable, const char *key, const char *value) {
    unsigned int index = hash(key);
    KeyValue *new_pair = create_pair(key, value);

    if (hashtable->table[index] == NULL) {
        hashtable->table[index] = new_pair;
    } else {
        KeyValue *current = hashtable->table[index];
        while (current->next != NULL && strcmp(current->key, key) != 0) {
            current = current->next;
        }
        if (strcmp(current->key, key) == 0) {
            free(current->value); 
            current->value = strdup(value); 
            free(new_pair->key);
            free(new_pair->value);
            free(new_pair);
        } else {
            current->next = new_pair; 
        }
    }
}

char *get(HashTable *hashtable, const char *key) {
    unsigned int index = hash(key);
    KeyValue *pair = hashtable->table[index];

    while (pair != NULL && strcmp(pair->key, key) != 0) {
        pair = pair->next;
    }

    if (pair == NULL) {
        return NULL; 
    } else {
        return pair->value; 
    }
}

void free_table(HashTable *hashtable) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        KeyValue *pair = hashtable->table[i];
        while (pair != NULL) {
            KeyValue *temp = pair;
            pair = pair->next;
            free(temp->key);
            free(temp->value);
            free(temp);
        }
    }
}

void merge_tables(HashTable *table1, HashTable *table2) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        KeyValue *pair = table2->table[i];
        while (pair != NULL) {
            insert(table1, pair->key, pair->value);
            pair = pair->next; 
        }
    }
}

// driver test
// int main() {
//     HashTable *hashtable = malloc(sizeof(HashTable));
//     memset(hashtable->table, 0, sizeof(KeyValue *) * TABLE_SIZE);

//     insert(hashtable, "name", "Alice");
//     insert(hashtable, "email", "alice@example.com");

//     printf("Name: %s\n", get(hashtable, "name"));
//     printf("Email: %s\n", get(hashtable, "email"));

//     free_table(hashtable);
//     free(hashtable);

//     return 0;
// }
