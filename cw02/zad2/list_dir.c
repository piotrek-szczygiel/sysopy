#define _XOPEN_SOURCE 500
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "error.h"

static time_t timestamp;
static char sign;

static struct tm timeinfo;
static char cwd[4096];

static char* filetype(const struct stat* sb) {
  if (S_ISREG(sb->st_mode))
    return "file";
  if (S_ISDIR(sb->st_mode))
    return "dir";
  if (S_ISCHR(sb->st_mode))
    return "char dev";
  if (S_ISBLK(sb->st_mode))
    return "block dev";
  if (S_ISFIFO(sb->st_mode))
    return "fifo";
  if (S_ISLNK(sb->st_mode))
    return "slink";
  if (S_ISSOCK(sb->st_mode))
    return "sock";

  return "unknown file type";
}

static void recurse() {
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
      if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
        continue;
      }

      if (chdir(file->d_name) != 0) {
        perr("unable to change path to %s", file->d_name);
      }

      recurse();

      if (chdir("..") != 0) {
        perr("unable to change path to %s/../", file->d_name);
      }
    }

    if (sign == '=' && sb.st_mtime != timestamp)
      continue;
    else if (sign == '<' && sb.st_mtime >= timestamp)
      continue;
    else if (sign == '>' && sb.st_mtime <= timestamp)
      continue;

    if (getcwd(cwd, 4096) == NULL) {
      perr("unable to get current working directory");
    }

    char atime[32];
    char mtime[32];

    timeinfo = *localtime(&sb.st_atime);
    strftime(atime, 32, "%Y-%m-%d %H:%M:%S", &timeinfo);

    timeinfo = *localtime(&sb.st_mtime);
    strftime(mtime, 32, "%Y-%m-%d %H:%M:%S", &timeinfo);

    printf("%-9s  %9ldB  %19s  %19s  %s/%s\n", filetype(&sb), sb.st_size, atime,
           mtime, cwd, file->d_name);
  }
}

void show_files(const char* path, time_t t, char s) {
  if (chdir(path) != 0) {
    perr("unable to change path to %s", path);
  }

  timestamp = t;
  sign = s;

  printf("%-9s   %9s  %-19s  %-19s  %s\n", "type", "size", "access time",
         "modification time", "file path");
  recurse();
}
