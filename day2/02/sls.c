#include    <sys/types.h>
#include    <dirent.h>
#include    <stdio.h>
#include    <sys/stat.h>
#include    <unistd.h>

int main(int argc, char *argv[]) {
    DIR *dir = opendir(argv[1]);
    struct dirent *dirInfo = NULL;

    FILE *fp = fopen("result.xml","w");
    if (fp == NULL) {
        fprintf(fp, "File open Error: %s\n", "result.xml");
        return 1;
    }

    fprintf(fp, "<files>\n");
    while((dirInfo = readdir(dir)) != NULL) {
        fprintf(fp, "\t<file>\n");
        fprintf(fp, "\t\t<type>%d</type>\n", dirInfo->d_type);
        fprintf(fp, "\t\t<name>%s</name>\n", dirInfo->d_name);

        if ((dirInfo->d_type & DT_REG) == DT_REG) {
            struct stat buf;
            stat(dirInfo->d_name, &buf);
            fprintf(fp, "\t\t<size>%d</size>\n", buf.st_size);
        }
        fprintf(fp, "\t</file>\n");
    }
    fprintf(fp, "</files>\n");
    
    closedir(dir); 

    return 0;
}
