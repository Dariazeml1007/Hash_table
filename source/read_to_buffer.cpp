#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>
#include <x86intrin.h>
#include <math.h> // для sqrt
#ifdef USE_CALLGRIND
#include <valgrind/callgrind.h>
#endif

#include "read_to_buffer.h"
#include "hash_table.h"

const int  max_results = 100000;
const int WORD_SIZE = 32;

double get_time();
char** create_word_pointers(char* buffer, size_t buffer_size, size_t* out_word_count);


long get_size_of_file ( const char *name_of_file)
{
    assert(name_of_file);
    int info_of_file = 0;
    struct stat buffer;
    if ((info_of_file = stat(name_of_file, &buffer)) != 0)
    {
        printf("stat failure error .%d\n", info_of_file);
        return HASH_STAT_ERROR;
    }

    return buffer.st_size;
}

char * read_from_file (const char *filename)
{
    assert(filename);

    FILE *pfile = fopen (filename, "rb");
    if (pfile == NULL)
    {
        printf("Not opened\n");
        return NULL;
    }

    long size = get_size_of_file(filename);


    char *buffer  = (char*) calloc ((size + 1) , sizeof(char));//malloc
    if (!buffer)
    {
        printf ("Allocation error");
        return NULL;
    }

    long amount_read = fread(buffer, 1, size, pfile);
    if (amount_read != size)
    {
        free(buffer);
        fclose(pfile);
        fprintf(stderr, "Error: Failed to read file\n");
        return NULL;
    }
    fclose(pfile);
    return buffer;

}

int load_book_to_hash(HashTable* table, const char* filename)
{
    assert(table && filename);
    assert((uintptr_t)table % 64 == 0 && "Table must be 64-byte aligned");

    char* buffer = read_from_file(filename);
    if (!buffer) return HASH_FILE_NOT_OPENED;

    char* current = buffer;
    while (*current)
    {
        // Создаем выровненное слово
        alignas(32) char word[32] = {0};
        memcpy(word, current, 32);

        if (word[0] != '\0') {
            if (add_word(table, word) < HASH_SUCCESS)
            {
                free(buffer);
                return HASH_LOAD_ERROR;
            }
        }
        current += 32;
    }
    free(buffer);
    return HASH_SUCCESS;
}


double get_time()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
    {
        perror("clock_gettime failed");
        return 0.0;
    }
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

int *search_words(HashTable *table, const char *file)
{
    assert(table);
    assert(file);

    char *buffer = read_from_file(file);
    if (!buffer)
    {
        fprintf(stderr, "Failed to read file\n");
        return NULL;
    }

    size_t buffer_size = get_size_of_file(file);
    size_t word_count = 0;
    char **word_list = create_word_pointers(buffer, buffer_size, &word_count);

    int *result_massive = (int*) calloc(max_results, sizeof(int));
    if (!result_massive)
    {
        perror("calloc failed");
        free(buffer);
        free(word_list);
        return NULL;
    }


    const int RUNS = 10;
    double durations[RUNS];

    for (int k = 0; k < RUNS; k++)
    {
        double start = get_time();

        for (int i = 0; i < 500; ++i)
        {
            for (size_t j = 0; j < word_count; ++j)
            {

                result_massive[j] = search_word_table(table, word_list[j]);

            }
        }

        double end = get_time();
        durations[k] = end - start;
        printf("Запуск %2d: %.6f сек\n", k + 1, durations[k]);
    }


    double sum = 0.0;
    for (int i = 0; i < RUNS; ++i)
    {
        sum += durations[i];
    }
    double mean = sum / RUNS;

    // Считаем стандартное отклонение
    double variance = 0.0;
    for (int i = 0; i < RUNS; ++i)
    {
        variance += (durations[i] - mean) * (durations[i] - mean);
    }
    variance /= RUNS;
    double stddev = sqrt(variance);

    // Вывод результата
    printf("\nСреднее время:     %.6f сек\n", mean);
    printf("Стандартное отклонение: %.6f сек\n", stddev);
    printf("Результат:           %.6f ± %.6f сек\n", mean, stddev);

    free(word_list);
    free(buffer);

    return result_massive;
}
char** create_word_pointers(char* buffer, size_t buffer_size, size_t* out_word_count)
{
    assert(buffer != NULL);
    assert(out_word_count != NULL);


    size_t word_count = buffer_size / WORD_SIZE;


    char** word_list = (char**)calloc(word_count , sizeof(char*));
    if (!word_list)
    {
        perror("Failed to allocate word list");
        return NULL;
    }

    for (size_t i = 0; i < word_count; ++i)
    {
        word_list[i] = buffer + i * WORD_SIZE;
    }

    *out_word_count = word_count;
    return word_list;
}
