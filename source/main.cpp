#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <ctype.h>
#include <ctime>

#include "hash_table.h"
#include "read_to_buffer.h"

int main()
{
    HashTable *table = ctor_table();

    if (!table)
        assert(0 && "ALLOCATION MEMORY ERROR");

    if (load_book_to_hash(table, "Advanced_Harry.txt") != HASH_SUCCESS)
        assert(0 && "LOAD TABLE ERROR");

    int *result  =  search_words(table, "Advanced_Harry.txt");


    //printf ("here\n");
    //for (int i = 0; i < 8; i++)
    //    printf ("%d \n", result[i]);
    //printf ("here\n");

    if (dtor_table(table) != HASH_SUCCESS)

        assert(0 && "DTOR TABLE ERROR");
    free(result);
    return 0;
}
