#ifndef HASH_TABLE
#define HASH_TABLE
#include <cstdint>
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


#pragma pack(push, 1)
struct HashEntry {
    char word[32];     // 32 байта
    int count;         // 4 байта
    HashEntry* next;   // 8 байт (x64)
};                     // Итого: 44 байта (32+4+8)
#pragma pack(pop)

typedef struct {
    int size;          // 4 байта
    HashEntry** buckets; // 8 байт (x64)
} HashTable;

extern "C" {
    uint32_t hash_crc32_asm(const char* word);
    int search_word_table_asm(HashTable* table, const char* word);
}

// Прототипы функций управления таблицей
HashTable* ctor_table();
int dtor_table(HashTable* table);
int add_word(HashTable* table, char* word);

// Декларация для AVX2 версии
#ifdef __AVX2__
int search_word_table(HashTable* table, const char* word)
    __attribute__((target("avx2")));
#endif

#endif // HASH_TABLE_H
