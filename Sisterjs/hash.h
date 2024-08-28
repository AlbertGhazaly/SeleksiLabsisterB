#ifndef HASH_H  
#define HASH_H 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

typedef struct KeyValue{
    char *key;
    char *value; 
    struct KeyValue *next;
} KeyValue;

typedef struct HashTable {
    KeyValue *table[TABLE_SIZE];
} HashTable;

unsigned int hash(const char *key);
KeyValue *create_pair(const char *key, const char *value);
void insert(HashTable *hashtable, const char *key, const char *value);
char *get(HashTable *hashtable, const char *key) ;
void free_table(HashTable *hashtable) ;
void merge_tables(HashTable *table1, HashTable *table2);
#endif