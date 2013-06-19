#include    <stdio.h>

int main(int argc, char *argv[]) {
    FILE *fp;
    fp = fopen(argv[1], "a");
    if (fp == NULL) {
        fprintf(stderr, "File Open Error: %s\n", argv[1]);
        return 1;
    }

    fclose(fp);

    return 0;
}
