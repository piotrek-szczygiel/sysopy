#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "error.h"

static char abs_path[4096];

static struct tm timeinfo;

static time_t timestamp;
static char sign;

int skip = 1;

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

static int info(const char* path,
                const struct stat* sb,
                int tflag,
                struct FTW* ftwbuf) {
  // skip first item, which is a directory passed as an argument
  if (skip == 1) {
    skip = 0;
    return 0;
  }

  if (sign == '=' && sb->st_mtime != timestamp)
    return 0;
  else if (sign == '<' && sb->st_mtime >= timestamp)
    return 0;
  else if (sign == '>' && sb->st_mtime <= timestamp)
    return 0;

  char atime[32];
  char mtime[32];

  timeinfo = *localtime(&sb->st_atime);
  strftime(atime, 32, "%Y-%m-%d %H:%M:%S", &timeinfo);

  timeinfo = *localtime(&sb->st_mtime);
  strftime(mtime, 32, "%Y-%m-%d %H:%M:%S", &timeinfo);

  printf("%-9s  %9ldB  %19s  %19s  %s\n", filetype(sb), sb->st_size, atime,
         mtime, path);
  return 0;
}

void show_files(const char* path, time_t t, char s) {
  if (realpath(path, abs_path) == NULL) {
    perr("unable to get absolute path of %s", path);
  }

  timestamp = t;
  sign = s;

  printf("%-9s   %9s  %-19s  %-19s  %s\n", "type", "size", "access time",
         "modification time", "file path");

  if (nftw(abs_path, info, 1024, FTW_PHYS) == -1) {
    perr("nftw error");
  }
}
