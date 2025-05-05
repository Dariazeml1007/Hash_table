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
#ifdef USE_CALLGRIND
#include <valgrind/callgrind.h>
#endif

#include "read_to_buffer.h"
#include "hash_table.h"

#define MAX_RESULTS 100000

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


    char *buffer  = (char*) malloc ((size + 1) *sizeof(char));
    if (!buffer)
    {
        printf ("Allocation error");
        return NULL;
    }

    size_t amount_read = fread(buffer, 1, size, pfile);
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


int load_book_to_hash(HashTable *table, const char *filename)
{
    assert(table);
    assert(filename);


    char *buffer  = read_from_file(filename);


    char *current = buffer;
    int index = 0;

    while (*current)
    {

        alignas(32) char word[32]= {0};
        strncpy_avx2(word, current);
        //strncpy(word, current, 32);
        if (word[0] != '\0')
        {
            if (add_word(table, word) < HASH_SUCCESS)

                return HASH_LOAD_ERROR;

        }
        current += 32;

    }
    free(buffer);

    return HASH_SUCCESS;
}


int *search_words (HashTable *table, const char *file)
{
    assert(table);
    assert(file);

    char *buffer = read_from_file(file);

    char *current = buffer;
    int *result_massive = (int *)calloc(MAX_RESULTS, sizeof(int));
    int index = 0;

    while (*current)
    {
        alignas(32) char word[32]= {0};
        strncpy_avx2(word, current);

        if (word[0] != '\0')

            result_massive[index++] = search_word_table(table, word);

        current += 32;
    }


    free(buffer);

    if (!result_massive) {
        assert(0 && "process_words_from_buffer failed");
        return NULL;
    }



    return result_massive;

}

