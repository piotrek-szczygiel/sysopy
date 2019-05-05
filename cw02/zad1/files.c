#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "time.h"

void err(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);
  exit(1);
}

void perr(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  fprintf(stderr, ": %s\n", strerror(errno));
  exit(1);
}

void generate(const char* filename,
              unsigned int elements,
              unsigned int block_size) {
  FILE* file = fopen(filename, "w");
  if (file == NULL) {
    perr("unable to open file %s", filename);
  }

  int random_fd = open("/dev/urandom", O_RDONLY);
  if (random_fd < 0) {
    perr("unable to open /dev/urandom");
  }

  char* random_block = malloc(block_size);
  for (unsigned int i = 0; i < elements; ++i) {
    if (read(random_fd, random_block, block_size) < 0) {
      perr("error while reading /dev/urandom");
    }
    if (fwrite(random_block, 1, block_size, file) < 0) {
      perr("error while writing to file %s", filename);
    }
  }
  free(random_block);
  close(random_fd);
  fclose(file);
}

void sort_sys(const char* filename,
              unsigned int elements,
              unsigned int block_size) {
  int fd = open(filename, O_RDWR);
  if (fd < 0) {
    perr("unable to open file %s", filename);
  }

  char* block1 = malloc(block_size);
  char* block2 = malloc(block_size);

  for (unsigned int i = 0; i < elements; ++i) {
    lseek(fd, i * block_size, SEEK_SET);

    if (read(fd, block1, block_size) < 0) {
      perr("error while reading from file %s", filename);
    }

    unsigned int min_element = i;
    unsigned char min_value = block1[0];

    for (unsigned int j = i + 1; j < elements; ++j) {
      if (read(fd, block2, block_size) < 0) {
        perr("error while reading from file %s", filename);
      }

      if ((unsigned char)block2[0] < min_value) {
        min_element = j;
        min_value = block2[0];
      }
    }

    if (min_element != i) {
      if (lseek(fd, min_element * block_size, SEEK_SET) < 0) {
        perr("unable to seek to %d in %s", min_element * block_size, filename);
      }

      if (read(fd, block2, block_size) < 0) {
        perr("error while reading from file %s", filename);
      }

      if (lseek(fd, i * block_size, SEEK_SET) < 0) {
        perr("unable to seek to %d in %s", i * block_size, filename);
      }

      if (write(fd, block2, block_size) < 0) {
        perr("error while writing to file %s", filename);
      }

      if (lseek(fd, min_element * block_size, SEEK_SET) < 0) {
        perr("unable to seek to %d in %s", min_element * block_size, filename);
      }

      if (write(fd, block1, block_size) < 0) {
        perr("error while writing to file %s", filename);
      }
    }
  }

  free(block1);
  free(block2);
  close(fd);
}

void sort_lib(const char* filename,
              unsigned int elements,
              unsigned int block_size) {
  FILE* f = fopen(filename, "r+");
  if (f == NULL) {
    perr("unable to open file %s", filename);
  }

  char* block1 = malloc(block_size);
  char* block2 = malloc(block_size);

  for (unsigned int i = 0; i < elements; ++i) {
    if (fseek(f, i * block_size, SEEK_SET) < 0) {
      perr("unable to seek to %d in %s: %s", i * block_size, filename);
    }

    if (fread(block1, 1, block_size, f) != block_size) {
      perr("error while reading from file %s: %s", filename);
    }

    unsigned int min_element = i;
    unsigned char min_value = block1[0];

    for (unsigned int j = i + 1; j < elements; ++j) {
      if (fread(block2, 1, block_size, f) != block_size) {
        perr("error while reading from file %s", filename);
      }

      if ((unsigned char)block2[0] < min_value) {
        min_element = j;
        min_value = block2[0];
      }
    }

    if (min_element != i) {
      if (fseek(f, min_element * block_size, SEEK_SET) < 0) {
        perr("unable to seek to %d in %s", min_element * block_size, filename);
      }

      if (fread(block2, 1, block_size, f) != block_size) {
        perr("error while reading from file %s", filename);
      }

      if (fseek(f, i * block_size, SEEK_SET) < 0) {
        perr("unable to seek to %d in %s", i * block_size, filename);
      }

      if (fwrite(block2, 1, block_size, f) != block_size) {
        perr("error while writing to file %s", filename);
      }

      if (fseek(f, min_element * block_size, SEEK_SET) < 0) {
        perr("unable to seek to %d in %s", min_element * block_size, filename);
      }

      if (fwrite(block1, 1, block_size, f) != block_size) {
        perr("error while writing to file %s", filename);
      }
    }
  }

  free(block1);
  free(block2);
  fclose(f);
}

void copy_sys(const char* src,
              const char* dst,
              unsigned int elements,
              unsigned int block_size) {
  int src_fd = open(src, O_RDONLY);
  if (src_fd < 0) {
    perr("unable to open file %s", src);
  }

  int dst_fd = open(dst, O_WRONLY | O_CREAT, 0644);
  if (dst_fd < 0) {
    perr("unable to open file %s", dst);
  }

  char* block = malloc(block_size);
  for (unsigned int i = 0; i < elements; ++i) {
    if (read(src_fd, block, block_size) < 0) {
      perr("unable to read from file %s", src);
    }

    if (write(dst_fd, block, block_size) < 0) {
      perr("unable to write to file %s", dst);
    }
  }

  free(block);
}

void copy_lib(const char* src,
              const char* dst,
              unsigned int elements,
              unsigned int block_size) {
  FILE* src_f = fopen(src, "rb");
  if (src_f == NULL) {
    perr("unable to open file %s", src);
  }

  FILE* dst_f = fopen(dst, "wb");
  if (dst_f == NULL) {
    perr("unable to open file %s", dst);
  }

  char* block = malloc(block_size);
  for (unsigned int i = 0; i < elements; ++i) {
    if (fread(block, 1, block_size, src_f) != block_size) {
      perr("unable to read from file %s", src);
    }

    if (fwrite(block, 1, block_size, dst_f) != block_size) {
      perr("unable to write to file %s", dst);
    }
  }

  free(block);
  fclose(src_f);
  fclose(dst_f);
}

int main(int argc, char* argv[]) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "generate") == 0) {
      if (i + 3 >= argc) {
        err("not enough arguments for generate command");
      }

      generate(argv[i + 1], (unsigned int)atoi(argv[i + 2]),
               (unsigned int)atoi(argv[i + 3]));
      i += 3;
    } else if (strcmp(argv[i], "sort") == 0) {
      if (i + 4 >= argc) {
        err("not enough arguments for sort command");
      }

      if (strcmp(argv[i + 4], "sys") == 0) {
        sort_sys(argv[i + 1], (unsigned int)atoi(argv[i + 2]),
                 (unsigned int)atoi(argv[i + 3]));
      } else if (strcmp(argv[i + 4], "lib") == 0) {
        sort_lib(argv[i + 1], (unsigned int)atoi(argv[i + 2]),
                 (unsigned int)atoi(argv[i + 3]));
      } else {
        err("unknown sort argument: %s", argv[i + 4]);
      }

      i += 4;
    } else if (strcmp(argv[i], "copy") == 0) {
      if (i + 5 >= argc) {
        err("not enough arguments for copy command");
      }

      if (strcmp(argv[i + 5], "sys") == 0) {
        copy_sys(argv[i + 1], argv[i + 2], (unsigned int)atoi(argv[i + 3]),
                 (unsigned int)atoi(argv[i + 4]));
      } else if (strcmp(argv[i + 5], "lib") == 0) {
        copy_lib(argv[i + 1], argv[i + 2], (unsigned int)atoi(argv[i + 3]),
                 (unsigned int)atoi(argv[i + 4]));
      } else {
        err("unknown copy argument: %s", argv[i + 5]);
      }

      i += 5;
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
      err("unknown argument: %s", argv[i]);
    }
  }

  return 0;
}
