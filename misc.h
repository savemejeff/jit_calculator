#ifndef MISC_H

#if defined(__GNUC__)
#    define PRINTF_LIKE(x, y) __attribute__((format (printf, x, y)))
#    define NORETURN __attribute__((noreturn))
#    define FALLTHROUGH __attribute__((fallthrough))
#else
#    define PRINTF_LIKE(x, y)
#    define NORETURN
#    define FALLTHROUGH
#endif

#ifndef ASSERT
#include <assert.h>
#define ASSERT assert
#endif

#ifndef DA_INIT_CAPACITY
#define DA_INIT_CAPACITY 32
#endif

#define da_reserve(da, at_least)                                               \
do {                                                                           \
    if ((at_least) > (da)->capacity) {                                         \
        if ((da)->capacity == 0) {                                             \
            (da)->capacity = DA_INIT_CAPACITY;                                 \
        }                                                                      \
        while ((at_least) > (da)->capacity) {                                  \
            (da)->capacity *= 2;                                               \
        }                                                                      \
    }                                                                          \
    (da)->items = realloc((da)->items, sizeof(*(da)->items) * (da)->capacity); \
    ASSERT((da)->items != NULL && "ran out of memory");                        \
} while(0)

#define da_append(da, new_item)                  \
    do {                                         \
        da_reserve((da), (da)->count + 1);       \
        (da)->items[(da)->count++] = (new_item); \
    } while (0)

// read a entire file into a c-string
char *read_entire_file(const char *filename);
// allocate memory with executable access, then copy SIZE bytes of PTR
void *make_executable(void *ptr, size_t size);

#endif // MISC_H
