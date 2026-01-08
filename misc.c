#include "common.h"
#include "misc.h"

#include <sys/mman.h>

char *read_entire_file(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    ASSERT(file != NULL && "failed to open file");

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    char *buffer = malloc(size + 1);
    size_t read_count = fread(buffer, sizeof(char), size, file);
    ASSERT(read_count == size && "failed to read file");
    buffer[size] = '\0';

    fclose(file);
    return buffer;
}

void *make_executable(void *ptr, size_t size)
{
    void *m = mmap(NULL, size, PROT_EXEC | PROT_WRITE | PROT_READ,
                   MAP_PRIVATE | MAP_ANON, -1, 0);
    ASSERT(m != NULL && "mmap failed");
    memcpy(m, ptr, size);
    return m;
}