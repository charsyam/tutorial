#include    <string.h>
#include    <stdio.h>
#include    "util.h"

void localIP(char **ip) {
    FILE *fp = popen("ifconfig", "r");
    if (fp) {
        char *e;
        size_t n;

        while ((getline(ip, &n, fp) > 0) && *ip) {
            if ((*ip = strstr(*ip, "inet addr:")) != NULL) {
                *ip += 10;
                if ((e = strchr(*ip, ' ')) != NULL) {
                    *e=0;
                }

                break;
            }
        }

        pclose(fp);
    }
}
