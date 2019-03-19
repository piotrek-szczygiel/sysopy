#define _XOPEN_SOURCE
#include "list.h"
#include "error.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

time_t parse_datetime(const char* datetime)
{
    struct tm tm_struct;
    memset(&tm_struct, 0, sizeof(struct tm));

    if (strptime(datetime, "%Y-%m-%d %H:%M:%S", &tm_struct) == NULL) {
        err("error while parsing date: %s", datetime);
    }

    time_t t = mktime(&tm_struct);
    if (t == -1) {
        err("error while converting date to time_t");
    }

    return t;
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        err("Usage: %s [path] [comparison sign] [datetime]", argv[0]);
    }

    char sign = (char)argv[2][0];
    if (strlen(argv[2]) != 1 || (sign != '=' && sign != '<' && sign != '>')) {
        err("Expected '>', '<', '=' instead of '%s'", argv[2]);
    }

    time_t time = parse_datetime(argv[3]);
    show_files(argv[1], time, sign);
    printf("\n");
    return 0;
}
