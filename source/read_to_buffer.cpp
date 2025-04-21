#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <sys\stat.h>
#include <ctype.h>

#include "read_to_buffer.h"
#include "hash_table.h"

int get_size_of_file (const char *name_of_file);
int process_buffer_to_table(HashTable *table, char *buffer);

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

int load_book_to_hash(HashTable *table, const char *filename)
{
    assert(table);
    assert(filename);

    FILE *pfile = fopen (filename, "rb");
    if (pfile == NULL)
    {
        printf("Not opened\n");
        return HASH_FILE_NOT_OPENED;
    }

    size_t size = get_size_of_file(filename);

    char *buffer  = (char*) calloc (size + 1, sizeof(char));
    if (!buffer)
    {
        printf ("Allocation error");
        return HASH_ALLOCATION_MEMORY_ERROR;
    }

    size_t amount_read = fread(buffer, 1, size, pfile);
    if (amount_read != size)
    {
        free(buffer);
        fclose(pfile);
        fprintf(stderr, "Error: Failed to read file\n");
        return HASH_READ_ERROR;
    }

    if (process_buffer_to_table(table, buffer) != HASH_SUCCESS)
        return HASH_LOAD_ERROR;

    fclose(pfile);
    free(buffer);

    return HASH_SUCCESS;
}

int process_buffer_to_table(HashTable *table, char *buffer)
{
    assert(table);
    assert(buffer);

    char *current = buffer;
    while (*current)
    {

        while (*current && (ispunct(*current) || isspace(*current)))
        {
            current++;
        }

        if (!*current) break;


        char *word_start = current;


        while (*current && !ispunct(*current) && !isspace(*current))
        {
            *current = (char) tolower(*current);
            current++;
        }


        if (current > word_start)
        {

            char temp = *current;
            *current = '\0';

            if (add_word(table, word_start) != HASH_SUCCESS)
                return HASH_ADD_WORD_ERROR;

            *current = temp;
        }
    }
    return HASH_SUCCESS;
}
