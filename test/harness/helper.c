#include <helper.h>

#include <stdio.h>
#include <stdlib.h>

char* load_file(const char* filename, size_t* size)
{
    FILE* fin = NULL;
    // Open file
    if((fin = fopen(filename, "rb")) == NULL) {
        return NULL;
    }
    // Get file size
    if(fseek(fin, 0L, SEEK_END)) {
        return NULL;
    }
    *size = ftell(fin);
    if(*size == -1L) {
        return NULL;
    }
    if(fseek(fin, 0L, SEEK_SET)) {
        return NULL;
    }
    // Allocate data memory
    char* data = malloc(*size);
    if(data == NULL) {
        return NULL;
    }
    // Read file
    if(fread(data, sizeof(char), *size, fin) != *size) {
        return NULL;
    }
    fclose(fin);
    return data;
}
