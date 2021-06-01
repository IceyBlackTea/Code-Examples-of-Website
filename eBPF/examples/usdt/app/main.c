/*
 * app/main.c
 * Example: USDT, get data from tracepoints
 * This is a example executable program,
 * which provides a usdt tracepoint.
 * You can use 'gcc -o print main.c'
 * to compiler this file,
 * or just use `make` in this directory.
 */

#include <stdio.h>
#include <string.h>
// use for macro definition, such as DTRACE_PROBE
#include <sys/sdt.h>

struct data_t {
    int argc;
    char argv[2][64];
};

void print_usage() {
    printf("This is a example executable program.\n");
    printf("It will print the first two arguments of the program.\n");
    printf("usage: ./print [argv_1] [argv_2]\n");
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        struct data_t data = { 0, { "", "" } };

        data.argc = argc;
        for (int i = 1; i < argc; i++)
            strcpy(data.argv[i - 1], argv[i]);

        /*
         * DTRACE_PROBE[x]: x means the number of arguments, the maximum is 12
         * such as:
         *     DTRACE_PROBE(provider, probe)
         *     DTRACE_PROBE[X](provider, probe, parm1, ..., parmx)
         */
        DTRACE_PROBE1("echo-usdt", probe-main, &data);

        printf("%s %s\n", argv[1], argv[2]);
    } else
        print_usage();

    return 0;
}