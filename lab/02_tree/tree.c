#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>

int info(const char* path, const struct stat* sb, int tflag, struct FTW* ftwbuf)
{
    if (tflag == FTW_F) {
        printf("%10ldB\t%s\n", sb->st_size, path);
    }

    return 0;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s <directory>\n", argv[0]);
        return 1;
    }

    if (nftw(argv[1], info, 20, 0) == -1) {
        return 1;
    }

    return 0;
}
