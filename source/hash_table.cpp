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
#include <immintrin.h>
#include <cstdint>

#include "hash_table.h"
#include "read_to_buffer.h"

const int  TABLE_SIZE = 5147;

const int WORD_SIZE = 32;


uint32_t hash_intrinsic(const char* word) ;
int strcmp_avx2(const char *s1, const char *s2);

//unsigned long hash(const char *key);
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
__attribute__((noinline))
uint32_t hash_intrinsic(const char* word)
{
    uint64_t hash = 0;
    hash = _mm_crc32_u64(hash, *((const uint64_t*)(word)));     //process only 4 byte
    hash = _mm_crc32_u64(hash, *((const uint64_t*)(word + 8)));
    hash = _mm_crc32_u64(hash, *((const uint64_t*)(word + 16)));
    hash = _mm_crc32_u64(hash, *((const uint64_t*)(word + 24)));
    return (uint32_t)hash;
}
//__attribute__((noinline))
//unsigned long hash(const char *key)
//{
//    assert(key);
//    unsigned long hash = 5381;
//    for (size_t i = 0; key[i] != '\0'; i++)
//         hash = ((hash << 5) + hash) + (size_t) (key[i]); // hash * 33 + c
//    return hash;
//}
//
int add_word(HashTable *table, char *word)
{
    assert(table);
    assert(word);

    unsigned long index = hash_intrinsic(word) % table->size;
    HashEntry *entry = table->buckets[index];

    while (entry)
    {
        if (strcmp(entry->word , word) == 0) //strcmp_avx2
        {
            entry->count++;
            return HASH_SUCCESS;
        }
        entry = entry->next;
    }

    HashEntry *new_entry = (HashEntry*) calloc (1, sizeof(HashEntry));
    if (!new_entry)
        return HASH_ALLOCATION_MEMORY_ERROR;

    strncpy(new_entry->word, word, 32); //

    new_entry->word[sizeof(new_entry->word) - 1] = '\0';

    new_entry->word[32] = '\0';
    new_entry->count = 1;

    new_entry->next = table->buckets[index];
    table->buckets[index] = new_entry;

    return HASH_SUCCESS;
}


__attribute__((noinline))
int strcmp_avx2(const char *s1, const char *s2)
{
    assert(s1);
    assert(s2);

    const __m256i *ptr1 = (const __m256i*)s1;
    const __m256i *ptr2 = (const __m256i*)s2;

    __m256i vec1 = _mm256_load_si256(ptr1);
    __m256i vec2 = _mm256_load_si256(ptr2);

    uint32_t mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(vec1, vec2));
    return (mask == 0xFFFFFFFF) ? 0 : 1;
}
//__attribute__((noinline))
//void strncpy_avx2(char *dest, const char *src)
//{
//    const __m256i *src_vec = (const __m256i*)src;
//    __m256i data = _mm256_loadu_si256(src_vec);
//    _mm256_storeu_si256((__m256i*)dest, data);
//
//    dest[WORD_SIZE-1] = '\0';
//}

int dtor_table(HashTable *table)
{
    assert(table);

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


int search_word_table(HashTable *table, const char *word)
{
    assert(table);
    assert(word);

    unsigned long index = hash_intrinsic(word) % table->size;
    HashEntry *entry = table->buckets[index];

    // Подготавливаем искомое слово
    __m256i target = _mm256_loadu_si256((const __m256i *)word);

    while (entry)
    {
        // Загружаем слово и сравниваем
        __m256i current = _mm256_loadu_si256((const __m256i *)(entry->word));
        __m256i cmp = _mm256_cmpeq_epi8(current, target);
        int mask = _mm256_movemask_epi8(cmp);

        if (mask == 0xFFFFFFFF)
        {
            return entry->count;
        }

        entry = entry->next;
    }

    return HASH_NOT_FOUND_WORD;
}

//int search_word_table(HashTable *table, const char *word)
//{
//    assert(table);
//    assert(word);
//
//    unsigned long index = hash_intrinsic(word) % table->size;
//    HashEntry *entry = table->buckets[index];
//
//    __m256i target = _mm256_loadu_si256((__m256i*)word);
//    int is_match = 0;
//
//    while (entry && entry->next)
//    {
//        // Загружаем два слова
//        HashEntry *e1 = entry;
//        HashEntry *e2 = entry->next;
//
//        // Проверяем оба за один asm блок
//        __asm__ __volatile__(
//            "vmovdqu ymm0, %[w1] \n\t"
//            "vmovdqu ymm1, %[w2] \n\t"
//
//            "vpcmpeqb ymm0, ymm0, %[target] \n\t"
//            "vpcmpeqb ymm1, ymm1, %[target] \n\t"
//
//            "vpmovmskb eax, ymm0 \n\t"
//            "vpmovmskb ebx, ymm1 \n\t"
//
//            "cmp eax, -1 \n\t"
//            "je .Lmatch1 \n\t"
//            "cmp ebx, -1 \n\t"
//            "jne .Lno_match \n\t"
//
//        ".Lmatch1:"
//            "mov %[res], 1 \n\t"
//        ".Lno_match:"
//
//            : [res] "+r"(is_match)
//            : [w1] "m"(e1->word), [w2] "m"(e2->word), [target] "x"(target)
//            : "rax", "rbx", "ymm0", "ymm1", "memory", "cc"
//        );
//
//        if (is_match == 1) return e1->count;
//        if (is_match == 1) return e2->count; // оба не могут совпасть одновременно
//
//        entry = e2->next;
//    }
//
//    // Обычный поиск для остатка
//    while (entry) {
//        if (_mm256_movemask_epi8(_mm256_cmpeq_epi8(
//                _mm256_loadu_si256((__m256i*)entry->word),
//                target)) == 0xFFFFFFFF) {
//            return entry->count;
//        }
//        entry = entry->next;
//    }
//
//    return HASH_NOT_FOUND_WORD;
//}
//int search_word_table (HashTable *table, const char *word)
//{
//
//    assert(table);
//    assert(word);
//
//    unsigned long index = hash_intrinsic(word) % table->size;
//    HashEntry *entry = table->buckets[index];
//
//    while (entry)
//    {
//        if (strcmp_avx2(entry->word , word) == 0)//_avx2
//        {
//
//            return entry->count;
//        }
//
//        if (entry->next)
//            entry = entry->next;
//        else
//            break;
//    }
//
//    return HASH_NOT_FOUND_WORD;
//
//}
//
