#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h> // wsl
#include <ctype.h>
#include <nmmintrin.h>
#include <stdint.h>

#include "hash_table.h"
#include "read_to_buffer.h"

#define TABLE_SIZE  5147
uint32_t hash_intrinsic(const char* word) ;

HashTable *ctor_table ()
{
    HashTable *table = (HashTable *)calloc(1, sizeof(HashTable));
    if (!table)
    {
        printf ("Memory allocation error");
        return NULL;
    }
    table->buckets = (HashEntry**) calloc(TABLE_SIZE, sizeof(HashEntry*));
    table->size = TABLE_SIZE;
    return table;
}

// First optimization
uint32_t hash_intrinsic(const char* word)
{
    uint64_t hash = 0;
    hash = _mm_crc32_u64(hash, *((const uint64_t*)(word)));     //process only 4 byte
    hash = _mm_crc32_u64(hash, *((const uint64_t*)(word + 8)));
    hash = _mm_crc32_u64(hash, *((const uint64_t*)(word + 16)));
    hash = _mm_crc32_u64(hash, *((const uint64_t*)(word + 24)));
    return (uint32_t)hash;
}


int add_word(HashTable *table, char *word)
{
    assert(table);
    assert(word);

    unsigned long index = hash_intrinsic(word) % table->size;
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

    strncpy(new_entry->word, word, 32);
    new_entry->word[32] = '\0';
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
            HashEntry *next = entry->next;
            free(entry);
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

    unsigned long index = hash_intrinsic(word) % table->size;
    HashEntry *entry = table->buckets[index];

    while (entry)
    {
        if (strncmp(entry->word , word, 32) == 0)
        {

            return entry->count;
        }
        entry = entry->next;
    }

    return HASH_NOT_FOUND_WORD;

}
