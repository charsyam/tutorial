#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include "util.h"

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

void broadcast(int port) {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    memset(&addr, 0, sizeof(addr));

    int ufd = socket(AF_INET, SOCK_DGRAM, 0);
    if (ufd == -1) {
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_aton("255.255.255.255", &addr.sin_addr);

    int opt = 1;
    setsockopt(ufd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)); 
    char *msg = "notification";
    int numbytes = sendto(ufd, msg, strlen(msg), 0, (struct sockaddr *)&addr, len);
    if (numbytes == -1) {
        fprintf(stderr, "ERROR: broadcast\n");
    }
}

