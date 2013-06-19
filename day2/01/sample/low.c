#include    <stdio.h>
#include    <sys/types.h>
#include    <sys/stat.h>
#include    <fcntl.h>


int main(int argc, char *argv[]) {
    int fd;
    fd = open(argv[1], O_RDWR | O_CREAT);
    if (fd == -1) {
        fprintf(stderr, "File Open Error: %s\n", argv[1]);
        return 1;
    }

    close(fd);

    return 0;
}
