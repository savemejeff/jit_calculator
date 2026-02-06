#include "common.h"
#include "misc.h"
#include "scanner.h"
#include "compiler.h"

typedef double (*run)();

void run_file(const char *filename)
{
    char *source = read_entire_file(filename);
    Code *code = compile(source);
    run run = make_executable(code->items, code->count);
    double result = run();
    printf("%lf\n", result);
    free(source);
}

#define BUF_SIZE 1024
void run_repl()
{
    char buf[BUF_SIZE];
    while (1) {
        if (fgets(buf, BUF_SIZE, stdin) == NULL) break;
        Code *code = compile(buf);
        run run = make_executable(code->items, code->count);
        double result = run();
        printf("%lf\n", result);
    }
}

void help() {
}

int main(int argc, const char **argv)
{
    if (argc == 1) {
        run_repl();
    } else if (argc == 2) {
        run_file(argv[1]);
    } else {
        help();
        return 1;
    }
    return 0;
}
