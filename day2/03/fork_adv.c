#include    <stdio.h>
#include    <unistd.h>
#include    <sys/types.h>
#include    <dirent.h>
#include    <sys/stat.h>


void child_main(char *folder) {
    DIR *dir = opendir(folder);
    struct dirent *dirInfo = NULL;

    while((dirInfo = readdir(dir)) != NULL) {
        printf("(%d)%s",dirInfo->d_type, dirInfo->d_name);
        if ((dirInfo->d_type & DT_REG) == DT_REG) {
            struct stat buf;
            stat(dirInfo->d_name, &buf);
            printf(" size(%d)", buf.st_size);
        }
        printf("\n");
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    while(1) {
        if (fork() == 0) {
            printf("child, %d, parent: %d\n", getpid(), getppid());
            child_main(argv[1]);
        } else {
            printf("parent, %d\n", getpid());
        }
        sleep(10);
    }
    return 0;
}

