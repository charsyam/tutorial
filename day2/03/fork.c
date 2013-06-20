#include    <stdio.h>
#include    <unistd.h>

int main(int argc, char *argv[]) {
    if (fork() == 0) {
        printf("child, %d, parent: %d\n", getpid(), getppid());
    } else {
        printf("parent, %d\n", getpid());
        sleep(10);
    }
    return 0;
}

