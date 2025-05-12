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
int add_word(HashTable* table, char* word)
{
    assert(table && word);
    static_assert(sizeof(HashEntry) == 44, "Invalid HashEntry size");

    // 1. Находим нужную корзину
    unsigned long index = hash_crc32_asm(word) % table->size;
    HashEntry* entry = table->buckets[index];

    // 2. Проверяем, есть ли слово уже в таблице
    while (entry != NULL) {
        if (strncmp(entry->word, word, 32) == 0)
        {
            // Слово найдено - увеличиваем счетчик
            entry->count++;
            return HASH_SUCCESS;
        }
        entry = entry->next;
    }

    // 3. Слово не найдено - создаем новую запись
    HashEntry* new_entry = (HashEntry*)calloc(1, sizeof(HashEntry));
    if (!new_entry)
    {
        return HASH_ALLOCATION_MEMORY_ERROR;
    }

    // Копируем слово (безопасно с нуль-терминатором)
    strncpy(new_entry->word, word, 31);
    new_entry->word[31] = '\0'; // Гарантируем завершение строки
    new_entry->count = 1;

    // Добавляем в начало цепочки
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

HashTable* ctor_table()
{

    HashTable* table = (HashTable*)aligned_alloc(64, sizeof(HashTable));
    if (!table) {
        printf("Memory allocation error for table\n");
        return NULL;
    }

    // 2. Выделяем выровненную память для массива buckets
    table->buckets = (HashEntry**)aligned_alloc(64, TABLE_SIZE * sizeof(HashEntry*));
    if (!table->buckets) {
        printf("Memory allocation error for buckets\n");
        free(table);
        return NULL;
    }

    // 3. Инициализация
    table->size = TABLE_SIZE;
    memset(table->buckets, 0,  TABLE_SIZE * sizeof(HashEntry*));

    return table;
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
//    // Проверка входных параметров
//    assert(table && word);
//
//    // Вычисление хеш-индекса
//    uint32_t index = hash_intrinsic(word) % table->size;
//    HashEntry *entry = table->buckets[index];
//
//    // Загрузка целевого слова в AVX регистр (1 раз перед циклом)
//    const __m256i target = _mm256_loadu_si256((const __m256i*)word);
//
//    while (entry)
//    {
//        uint32_t mask;  // Для хранения битовой маски сравнения
//        uint8_t found;  // Флаг совпадения (0/1)
//
//        /*
//         * Ассемблерный блок с ручной оптимизацией:
//         * 1. vmovdqu - загрузка текущего слова
//         * 2. vpcmpeqb - векторное сравнение
//         * 3. vpmovmskb - получение битовой маски
//         * 4. Сравнение маски с 0xFFFFFFFF через регистр ECX
//         * 5. sete - установка флага found
//         */
//        __asm__ __volatile__(
//            "vmovdqu ymm0, [%[current]]\n\t"  // 1. Загрузка 32 байт из памяти
//            "vpcmpeqb ymm0, ymm0, %[target]\n\t" // 2. Побайтовое сравнение
//            "vpmovmskb %k[mask], ymm0\n\t"    // 3. Конвертация в битовую маску
//            "xor %%ecx, %%ecx\n\t"            // 4.1 Обнуление ECX
//            "not %%ecx\n\t"                   // 4.2 ECX = 0xFFFFFFFF
//            "cmp %k[mask], %%ecx\n\t"         // 4.3 Сравнение маски
//            "sete %[found]\n\t"               // 5. Установка флага
//            : [mask] "=&r" (mask),
//              [found] "=r" (found)
//            : [current] "r" (entry->word),
//              [target] "x" (target)
//            : "ymm0", "ecx", "cc"  // Список разрушаемых регистров
//        );
//
//        if (found) {
//            return entry->count;
//        }
//
//        entry = entry->next;
//    }
//
//    return HASH_NOT_FOUND_WORD;
//}
////int search_word_table (HashTable *table, const char *word)
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
