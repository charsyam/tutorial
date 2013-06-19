#include    <stdio.h>
#include    <string.h>

int main(int argc, char *argv[]) {
    FILE *fp;
    fp = fopen(argv[1], "a");
    if (fp == NULL) {
        fprintf(stderr, "File Open Error: %s\n", argv[1]);
        return 1;
    }

    int i;
    char *line = "This is highio line\n";
    int linesize = strlen(line);
    for (i = 0; i < 10; i++) {
        fwrite(line, linesize, 1, fp);
    }

    fclose(fp);

    return 0;
}
