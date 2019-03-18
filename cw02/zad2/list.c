#define _XOPEN_SOURCE
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void show_files(const char* path, time_t time, char sign);


void err(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);
    exit(1);
}

void perr(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    fprintf(stderr, ": %s\n", strerror(errno));
    exit(1);
}

time_t parse_datetime(const char *datetime)
{
    struct tm tm_struct;

    if(strptime(datetime, "%Y-%m-%d %H:%M", &tm_struct) == NULL) {
        err("error while parsing date: %s", datetime);
    }

    time_t t = mktime(&tm_struct);
    if(t == -1) {
        err("error while converting date to time_t");
    }

    return t;
}

int main(int argc, char* argv[])
{
    if(argc != 4) {
        err("Usage: %s [path] [comparison sign] [datetime]", argv[0]);
    }

    char sign = argv[2][0];
    if(strlen(argv[2]) != 1 || sign != '=' || sign != '<' || sign != '>') {
        err("Expected '>', '<', '=' instead of '%s'", argv[2]);
    }


    time_t time = parse_datetime(argv[3]);

    return 0;
}
