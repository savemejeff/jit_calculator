#ifndef COMPILER_H

#include "common.h"
#include "misc.h"

typedef struct {
    uint8_t *items;
    size_t count;
    size_t capacity;
} Code;

Code *compile(const char *source);

#endif // COMPILER_H