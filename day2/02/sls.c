#include    <sys/types.h>
#include    <dirent.h>
#include    <stdio.h>
#include    <sys/stat.h>
#include    <unistd.h>

int main(int argc, char *argv[]) {
    DIR *dir = opendir(".");
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

    return 0;
}
