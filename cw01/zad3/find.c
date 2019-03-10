#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>

#ifdef DYNAMIC
#include "find_dyn.h"
#else
#include <find.h>
#endif

static clock_t st_time;
static clock_t en_time;
static struct tms st_cpu;
static struct tms en_cpu;

static char* log_filename = NULL;

void clock_log(FILE* f, char* name)
{
    int64_t clk_tck = sysconf(_SC_CLK_TCK);
    fprintf(f, "\n\n%s\n"
               "Real:   %5.2fs\n"
               "User:   %5.2fs  Children User: %5.2fs\n"
               "System: %5.2fs  Children User: %5.2fs\n",
        name,
        (double)(en_time - st_time) / clk_tck,
        (double)(en_cpu.tms_utime - st_cpu.tms_utime) / clk_tck,
        (double)(en_cpu.tms_cutime - st_cpu.tms_cutime) / clk_tck,
        (double)(en_cpu.tms_stime - st_cpu.tms_stime) / clk_tck,
        (double)(en_cpu.tms_cstime - st_cpu.tms_cstime) / clk_tck);
}

void clock_start()
{
    st_time = times(&st_cpu);
}

void clock_stop(char* name)
{
    en_time = times(&en_cpu);
    clock_log(stdout, name);

    if (log_filename != NULL) {
        FILE* file = fopen(log_filename, "a");
        clock_log(file, name);
        fclose(file);
    }
}

void set_log(char* filename)
{
    log_filename = filename;
}

int main(int argc, char* argv[])
{

#ifdef DYNAMIC
    fnd_dynamic_init();
#endif

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "table") == 0) {
            if (i + 1 >= argc)
                break;
            fnd_init((unsigned int)atoi(argv[i + 1]));
            i += 1;
        } else if (strcmp(argv[i], "find") == 0) {
            if (i + 3 >= argc)
                break;
            fnd_prepare(argv[i + 1], argv[i + 2], argv[i + 3]);
            fnd_search();
            i += 3;
        } else if (strcmp(argv[i], "delete") == 0) {
            if (i + 1 >= argc)
                break;
            fnd_del((unsigned int)atoi(argv[i + 1]));
            i += 1;
        } else if (strcmp(argv[i], "print") == 0) {
            if (i + 1 >= argc)
                break;
            printf("%s\n", fnd_get((unsigned int)atoi(argv[i + 1])));
            i += 1;
        } else if (strcmp(argv[i], "clock_start") == 0) {
            clock_start();
        } else if (strcmp(argv[i], "clock_stop") == 0) {
            if (i + 1 >= argc)
                break;
            clock_stop(argv[i + 1]);
            i += 1;
        } else if (strcmp(argv[i], "log") == 0) {
            if (i + 1 >= argc)
                break;
            set_log(argv[i + 1]);
            i += 1;
        } else {
            fprintf(stderr, "unknown argument: %s\n", argv[i]);
        }
    }

    fnd_free();

#ifdef DYNAMIC
    fnd_dynamic_free();
#endif

    return 0;
}
