#ifndef HASH_READ
#define HASH_READ

#include "hash_table.h"
typedef enum
{
    HASH_ACTION_ADD,
    HASH_ACTION_SEARCH
} HashAction;


int load_book_to_hash(HashTable *table, const char *filename);
long get_size_of_file (const char *name_of_file);
char * read_from_file (const char *filename);
int *search_words (HashTable *table, const char *file);
#endif
