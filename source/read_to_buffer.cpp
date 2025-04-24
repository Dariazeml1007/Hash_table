#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <ctype.h>

#include "read_to_buffer.h"
#include "hash_table.h"

#define MAX_RESULTS 1000

int get_size_of_file ( const char *name_of_file)
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

    size_t size = get_size_of_file(filename);


    char *buffer  = (char*) calloc (size + 1, sizeof(char));
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

int *process_words_from_buffer(char *buffer, HashTable *table, HashAction action)
{
    assert(table);
    assert(buffer);

    char *current = buffer;
    int *result_massive = (int *)calloc(MAX_RESULTS, sizeof(int));
    int index = 0;

    while (*current)
    {

        char word[32] = {0};
        strncpy(word, current, 32);


        if (word[0] != '\0')
        {
            if (action == HASH_ACTION_ADD)
            {
                if (add_word(table, word) < HASH_SUCCESS)
                {
                    free(result_massive);
                    return NULL;
                }
            }
            else
            {
                result_massive[index++] = search_word_table(table, word);
            }
        }
        current += 32;
    }

    if (action == HASH_ACTION_ADD)
    {
        free(result_massive);
        return NULL;
    }

    return result_massive;
}


int load_book_to_hash(HashTable *table, const char *filename)
{
    assert(table);
    assert(filename);


    char *buffer  = read_from_file(filename);

    process_words_from_buffer(buffer, table, HASH_ACTION_ADD);


    free(buffer);

    return HASH_SUCCESS;
}


int *search_words (HashTable *table, const char *file)
{
    assert(table);
    assert(file);

    char *buffer = read_from_file(file);

    int *result = process_words_from_buffer(buffer, table, HASH_ACTION_SEARCH);
    if (!result)
        assert(0 && "aaa");
    return result;

}

