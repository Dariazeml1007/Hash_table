#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <sys\stat.h>
#include <ctype.h>

#include "hash_table.h"
#include "read_to_buffer.h"

#define TABLE_SIZE 10  // Размер хеш-таблицы

unsigned long hash(const char *key);

HashTable *ctor_table ()
{
    HashTable *table = (HashTable *)calloc(1, sizeof(HashTable));
    if (!table)
    {
        printf ("Memory allocation error");
        return NULL;
    }
    table->buckets = (HashEntry**) calloc(TABLE_SIZE, sizeof(HashEntry*));  // Инициализация нулями
    table->size = TABLE_SIZE;
    return table;
}


unsigned long hash(const char *key)
{
    assert(key);

    unsigned long hash = 5381;

    for (size_t i = 0; key[i] != '\0'; i++)

         hash = ((hash << 5) + hash) + (size_t) tolower(key[i]); // hash * 33 + c


    return hash;
}

int add_word(HashTable *table, const char *word)
{
    assert(table);
    assert(word);

    unsigned long index = hash(word) % table->size;
    HashEntry *entry = table->buckets[index];


    while (entry)
    {
        if (strcmp(entry->word , word) == 0)
        {
            entry->count++;
            return HASH_SUCCESS;
        }
        entry = entry->next;
    }

    HashEntry *new_entry = (HashEntry*) calloc (1, sizeof(HashEntry));
    if (!new_entry)
        return HASH_ALLOCATION_MEMORY_ERROR;

    new_entry->word = strdup(word);
    new_entry->count = 1;
    new_entry->next = table->buckets[index];
    table->buckets[index] = new_entry;

    return HASH_SUCCESS;
}

int dtor_table(HashTable *table)
{

    for (int i = 0; i < table->size; i++)
    {
        HashEntry *entry = table->buckets[i];
        while (entry)
        {
            HashEntry *next = entry->next; // Сохраняем указатель на следующий
            free(entry->word);             // Ключ (strdup)
            free(entry);                  // Сам элемент
            entry = next;
        }
    }

    free(table->buckets);
    free(table);

    return HASH_SUCCESS;
}


int search_word_table (HashTable *table, const char *word)
{
    assert(table);
    assert(word);

    unsigned long index = hash(word) % table->size;
    HashEntry *entry = table->buckets[index];

    while (entry)
    {
        if (strcmp(entry->word , word) == 0)
        {
            return entry->count;
        }
        entry = entry->next;
    }

    return HASH_NOT_FOUND_WORD;


}
