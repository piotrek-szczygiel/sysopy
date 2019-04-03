#define _XOPEN_SOURCE 500
#include "error.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        err("usage: %s fifo count");
    }

    char* fifo_name = argv[1];

    char* end;
    long count = strtol(argv[2], &end, 10);
    if (end == argv[2] || errno != 0) {
        perr("%s is not a valid number", argv[2]);
    }

    pid_t pid = getpid();
    printf("%d\n", pid);

    struct stat sb;
    if (stat(fifo_name, &sb) == -1) {
        perr("unable to stat %s", fifo_name);
    }

    if (!S_ISFIFO(sb.st_mode)) {
        err("%s is not a fifo", fifo_name);
    }

    srand(time(NULL));
    FILE* fifo = fopen(fifo_name, "w");

    char date[512];
    for (int i = 0; i < count; ++i) {
        fprintf(fifo, "%d ", pid);

        FILE* fdate = popen("date", "r");
        if (fdate == NULL) {
            perr("unable to popen date");
        }

        if (fgets(date, sizeof(date), fdate) == NULL) {
            perr("unable to read date");
        }

        pclose(fdate);
        fprintf(fifo, "%s", date);
        sleep(2 + (rand() % 4));
    }

    fclose(fifo);
    return 0;
}
