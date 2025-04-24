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

#define WORD_SIZE 32

void process_file(const char* input_file, const char* output_file);

void process_file(const char* input_file, const char* output_file)
{
    FILE *out = fopen(output_file, "wb");
    if (!out)
    {
        assert(0 && "Output file hasn't opened");
        return;
    }

    char *buffer = read_from_file(input_file);
    if (!buffer)
    {
        fclose(out);
        assert(0 && "Failed to read input file");
        return;
    }

    char *current = buffer;
    char word_buffer[WORD_SIZE];

    while (*current)
    {

        while (*current && (ispunct(*current) || isspace(*current))) {
            current++;
        }

        if (!*current) break;


        char *word_start = current;

        while (*current && !ispunct(*current) && !isspace(*current)) {
            *current = (char)tolower(*current);
            current++;
        }

        if (current > word_start)

        {

            char temp = *current;
            *current = '\0';

            memset(word_buffer, 0, WORD_SIZE);
            strncpy(word_buffer, word_start, WORD_SIZE - 1);


            fwrite(word_buffer, 1, WORD_SIZE, out);

            *current = temp;
        }
    }

    free(buffer);
    fclose(out);
}


int main()
{

    process_file("../word.txt", "../Advanced_word.txt");
    return 0;
}
