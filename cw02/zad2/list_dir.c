#define _XOPEN_SOURCE 500
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "error.h"

static struct stat sb;
static time_t timestamp;
static char sign;

static void recurse()
{
    DIR* dir = opendir(".");
    if(dir == NULL) {
        perr("unable to open directory");
    }

    struct dirent* file;
    while((file = readdir(dir)) != NULL) {
        if(lstat(file->d_name, &sb) < 0) {
            perr("unable to lstat file %s", file->d_name);
        }

        if(S_ISDIR(sb.st_mode)) {
            if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
                continue;
            }

            if(chdir(file->d_name) != 0) {
                perr("unable to change path to %s", file->d_name);
            }

            recurse();

            if(chdir("..") != 0) {
                perr("unable to change path to %s/../", file->d_name);
            }
        }

        printf("%s\n", file->d_name);
    }
}


void show_files(const char *path, time_t t, char s)
{
    if(chdir(path) != 0) {
        perr("unable to change path to %s", path);
    }

    timestamp = t;
    sign = s;

    recurse();
}

