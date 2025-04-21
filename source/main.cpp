#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <sys\stat.h>
#include <ctype.h>

#include "hash_table.h"
#include "read_to_buffer.h"

int main()
{
    HashTable *table = ctor_table();

    if (!table)
        assert(0 && "ALLOCATION MEMORY ERROR");

    if (load_book_to_hash(table, "Harry_Potter_1.txt") != HASH_SUCCESS)
        assert(0 && "LOAD TABLE ERROR");
    printf("%d", search_word_table(table, "ron"));

    if (dtor_table(table) != HASH_SUCCESS)
        assert(0 && "DTOR TABLE ERROR");

    return 0;
}
