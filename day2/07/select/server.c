/* socket header file */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> /* AF_INET socket */
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define UDPPORT     2001    /* UDP PORT number */

int fds[1024];
char *fdptr[1024];
int fdsizes[1024];

int isCompletedCommand(char *ptr) {
    int ret = 0;
    int numbytes = strlen(ptr);
    if (ptr[numbytes-1] == '\n'){
        if (ptr[numbytes-2] == '\r')
            ptr[numbytes-2] = 0;
        else
            ptr[numbytes-1] = 0;

        ret = 1;
    }

    return ret;
}

int process(int fd, char *ptr) {
    if ((ptr[0] == 'g' || ptr[0] == 'G') &&
        (ptr[1] == 'e' || ptr[1] == 'E') &&
        (ptr[2] == 't' || ptr[2] == 'T')) {
    } else {
        char *resp = "500 Invalid Command\r\n\r\n";
        send(fd, resp, strlen(resp), 0);
        return 0;
    }

    char *filename = &ptr[4];
    printf("filename: %s\n", filename);

    struct stat buf;
    if (stat(filename, &buf) == -1) {
        char *resp = "500 Not ExistFile\r\n\r\n";
        send(fd, resp, strlen(resp), 0);
        return 0;
    }
 
    char *resp = "200 OK\r\n";
    send(fd, resp, strlen(resp), 0);

    char sizebuffer[1024];
    snprintf(sizebuffer, sizeof(sizebuffer), "Content-Length: %d\r\n\r\n",
            buf.st_size);
    send(fd, sizebuffer, strlen(sizebuffer), 0);

    FILE *fp = fopen(filename, "r");
    while(1) {
        int readed = fread(sizebuffer, 1, 1024, fp);
        if (readed == 0)
            break;

        send(fd, sizebuffer, readed, 0);
    }

    fclose(fp);
    close(fd);
    return 1;
}

int main(int argc, char *argv[])
{
    int serverLen;  /* address struct length */
    int serverFd;   /* server socket */
    struct sockaddr_in serverINETAddress;   /* server INET address */
    struct sockaddr* serverSockAddrPtr; /* server adress pointer */
    char udp_buf[] = "testing..";
    int numbytes;


    serverLen = sizeof(struct sockaddr); /* struct length */    
    serverSockAddrPtr = (struct sockaddr *)&serverINETAddress;

    bzero(&(serverINETAddress.sin_zero),8);
    serverINETAddress.sin_family = AF_INET;      /* host byte order */
    serverINETAddress.sin_port = htons(UDPPORT);  /* short, network byte order */
    serverINETAddress.sin_addr.s_addr = htonl(INADDR_ANY); /* BROADCAST */

    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        perror("socket");
        exit(1);
    }

    bind(serverFd, serverSockAddrPtr, serverLen);
    listen(serverFd,100);

    char buffer[4096];
    char s[1024];

    fd_set readfds;
    int ret;
    int i;
    fds[serverFd] = serverFd;

    while (1) {
        FD_ZERO(&readfds);
        for (i=0; i < 1024; i++) {
            if (fds[i] != 0) {
                FD_SET(fds[i], &readfds);
            }
        }

        ret = select(1024, &readfds, NULL, NULL, NULL);
        if (ret<0) {
            fprintf(stderr, "ERROR: select\n");
            break;
        }

        for (i=0; i<1024; i++) {
            if (fds[i] != 0 && FD_ISSET(fds[i], &readfds)) {
                if (fds[i] == serverFd) {
                    int len = sizeof(struct sockaddr_in);
                    struct sockaddr_in in;
                    int newfd = accept(serverFd, (struct sockaddr *)&in, &len);
                    inet_ntop(AF_INET, &(in.sin_addr), s, sizeof(s));
                    printf("len: %d, addr: %s\n", len, s);
                    fds[newfd] = newfd;
                    fdptr[newfd] = (char *)malloc(1024);
                    fdsizes[newfd] = 0;
                } else {
                    int size = fdsizes[i];
                    char *ptr = fdptr[i] + size;
                    numbytes = recv(fds[i], ptr, 1024-size,0);
                    ptr[numbytes] = 0;
                    fdsizes[i] += numbytes;
                    if (isCompletedCommand(fdptr[i]) == 1) {
                        process(fds[i], fdptr[i]);
                        close(fds[i]);
                        fdsizes[i] = 0;
                        fds[i] = 0;
                        free(fdptr[i]);
                        fdptr[i] = NULL;
                    }
                }
            }
        }
    }

    close(serverFd);
    return 0;
}

