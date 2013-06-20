#include    <stdio.h>
#include    <fcntl.h>
#include    <pthread.h>

void *processfd(void *arg) {
    char buf[4096];
    int fd;
    ssize_t nbytes;

    fd = *((int *)(arg));
    for(;;) {
        if ((nbytes = read(fd, buf, 4096)) <= 0)
            break;

        buf[nbytes] = 0;
        printf("%s", buf);
    }
}

int main(int argc, char *argv[]) {
    pthread_t tid;
    int fd;
    fd = open("/etc/hosts", O_RDONLY);
    pthread_create(&tid, NULL, processfd, &fd); 
    pthread_join(tid, NULL);
    return 0;
}
