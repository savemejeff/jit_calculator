#include "common.h"
#include "misc.h"
#include "scanner.h"
#include "compiler.h"

typedef int (*run)();

int main(int argc, const char **argv)
{
    ASSERT(argc == 2 && "no input file");
    char *source = read_entire_file(argv[1]);

    Code *code = compile(source);
    // for (size_t i = 0; i < code->count; i++) {
    //     printf("%02x\n", code->items[i]);
    // }
    run run = make_executable(code->items, code->count);
    printf("%d\n", run());

    free(source);
    return 0;
}