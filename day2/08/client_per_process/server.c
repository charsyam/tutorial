/* socket header file */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> /* AF_INET socket */
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/stat.h>

#define UDPPORT     2001    /* UDP PORT number */

int process_request(int fd);

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
    while (1) {
        int len = sizeof(struct sockaddr_in);
        struct sockaddr_in in;
        int newfd = accept(serverFd, (struct sockaddr *)&in, &len);
        if (newfd == -1) {
            printf("%s\n", strerror(errno));
            continue;
        }
        if (fork() == 0) {
            close(serverFd);
            process_request(newfd);
            exit(0);
        } else {
            inet_ntop(AF_INET, &(in.sin_addr), s, sizeof(s));
            printf("accepted(%d): len: %d, addr: %s\n", newfd, len, s);
            close(newfd);
        }
    }

    close(serverFd);
    return 0;
}

int process_request(int fd) {
    char buffer[4096];
    char *ptr = buffer;
    int size = sizeof(buffer);
    int numbytes;
    int readed = 0;

    while(1) {
        numbytes = recv(fd, ptr, size, 0);
        if (numbytes == 0) {
            close(fd);
            return 0;
        }
        ptr[numbytes] = 0;
        readed += numbytes;

        if (ptr[numbytes-1] == '\n'){
            if (ptr[numbytes-2] == '\r')
                ptr[numbytes-2] = 0;
            else
                ptr[numbytes-1] = 0; 

            break;
        }

        if (readed > 1024) {
            char *resp = "500 Invalid Request\r\n\r\n";
            send(fd, resp, strlen(resp), 0);
            close(fd);
            return 0;
        }
    }

    if ((buffer[0] == 'g' || buffer[0] == 'G') &&
        (buffer[1] == 'e' || buffer[1] == 'E') &&
        (buffer[2] == 't' || buffer[2] == 'T')) {
    } else {
        char *resp = "500 Invalid Command\r\n\r\n";
        send(fd, resp, strlen(resp), 0);
        close(fd);
        return 0;
    }

    char *filename = &buffer[4];
    printf("filename: %s\n", filename);

    
    struct stat buf;
    if (stat(filename, &buf) == -1) {
        char *resp = "500 Not ExistFile\r\n\r\n";
        send(fd, resp, strlen(resp), 0);
        close(fd);
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
