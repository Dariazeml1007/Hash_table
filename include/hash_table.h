#ifndef HASH_TABLE
#define HASH_TABLE

typedef enum
{
    HASH_SUCCESS = 1,
    HASH_ALLOCATION_MEMORY_ERROR = -1,
    HASH_FILE_NOT_OPENED = -2,
    HASH_FILE_NOT_CLOSED = -3,
    HASH_STAT_ERROR = -4,
    HASH_LOAD_ERROR = -5,
    HASH_NO_SUCH_ITEM_ERROR = -6,
    HASH_READ_ERROR = -7,
    HASH_INSERT_ERROR = -8,
    HASH_SUCCESS_DEFINE_SIZE = -9,
    HASH_ADD_WORD_ERROR = -10,
    HASH_NOT_FOUND_WORD = 0

}Hash_table_errors;



typedef struct HashEntry
{
    char word[32];
    int count;
    struct HashEntry *next;
} HashEntry;

typedef struct
{
    int size;
    HashEntry **buckets;
} HashTable;


HashTable *ctor_table ();
int dtor_table(HashTable *table);
int add_word(HashTable *table, char *word);
int search_word_table (HashTable *table, const char *word);
void strncpy_avx2(char* dest, const char* src) ;
#endif
