#ifndef FILES_TIME_H
#define FILES_TIME_H

#include <stddef.h>
#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>

static clock_t st_time;
static clock_t en_time;
static struct tms st_cpu;
static struct tms en_cpu;

static char* log_filename = NULL;

void set_log(char* filename)
{
    log_filename = filename;
}

void clock_log(FILE* f, char* name)
{
    unsigned int clk_tck = sysconf(_SC_CLK_TCK);
    fprintf(f, "\n\n%s\n  user:   %5.2f\n  system: %5.2f\n", name,
        (double)(en_cpu.tms_utime - st_cpu.tms_utime) / clk_tck,
        (double)(en_cpu.tms_stime - st_cpu.tms_stime) / clk_tck);
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

#endif
