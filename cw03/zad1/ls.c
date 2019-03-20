#define _XOPEN_SOURCE 500
#include "error.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static void recurse()
{
    DIR* dir = opendir(".");
    if (dir == NULL) {
        perr("unable to open directory");
    }

    struct dirent* file;
    while ((file = readdir(dir)) != NULL) {
        struct stat sb;

        if (lstat(file->d_name, &sb) < 0) {
            perr("unable to lstat file %s", file->d_name);
        }

        if (S_ISDIR(sb.st_mode)) {
            if (strcmp(file->d_name, ".") == 0
                || strcmp(file->d_name, "..") == 0) {
                continue;
            }

            if (chdir(file->d_name) != 0) {
                perr("unable to change path to %s", file->d_name);
            }

            recurse();

            pid_t child_pid = fork();

            if (child_pid == -1) {
                perr("unable to fork");
            } else if (child_pid > 0) {
                int status;
                waitpid(child_pid, &status, 0);
            } else {
                char cwd[4096];
                if (getcwd(cwd, 4096) == NULL) {
                    perr("unable to get current working directory");
                }

                printf("\nPID: %d\n%s\n", getpid(), cwd);
                execlp("ls", "ls", "-l", NULL);
                _exit(EXIT_FAILURE);
            }

            if (chdir("..") != 0) {
                perr("unable to change path to %s/../", file->d_name);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        err("usage: %s [path]", argv[0]);
    }

    if (chdir(argv[1]) != 0) {
        perr("unable to change path to %s", argv[1]);
    }

    recurse();

    return 0;
}
