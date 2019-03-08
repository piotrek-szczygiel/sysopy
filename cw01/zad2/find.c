#include <find.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char* argv[]) {
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "table") == 0) {
            if(i + 1 >= argc) break;
            fnd_init((size_t) atoi(argv[i + 1]));
            i += 1;
        } else if(strcmp(argv[i], "search") == 0) {
            if(i + 3 >= argc) break;
            fnd_prepare(argv[i + 1], argv[i + 2], argv[i + 3]);
            fnd_search();
            i += 3;
        } else if(strcmp(argv[i], "delete") == 0) {
            if(i + 1 >= argc) break;
            fnd_del((size_t) atoi(argv[i + 1]));
            i += 1;
        } else if(strcmp(argv[i], "print") == 0) {
            if(i + 1 >= argc) break;
            printf("%s\n", fnd_get((size_t) atoi(argv[i + 1])));
            i += 1;
        } else {
            fprintf(stderr, "unknown argument: %s\n", argv[i]);
        }
    }

    fnd_free();
    return 0;
}
