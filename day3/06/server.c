#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/stat.h>

#include "util.h"

#define UDPPORT     2001    
#define TCPPORT     2002

int createTcpServerSocket(struct sockaddr_in *addr) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        return -1;
    }

    if (bind(sock, (struct sockaddr *)addr, sizeof(struct sockaddr_in)) == -1) {
        return -1;
    } 

    if (listen(sock, 100) == -1)
        return -1;

    return sock;
}

int createUdpServerSocket(struct sockaddr_in *addr) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        return -1;
    }

    if (bind(sock, (struct sockaddr *)addr, sizeof(struct sockaddr_in)) == -1) {
        return -1;
    } 

    return sock;
}

int process_udp(int ufd) {
    char buffer[1024];
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    int numbytes = recvfrom(ufd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &len);
    buffer[numbytes] = 0;

    char ip[32];
    const char *ipaddr = inet_ntop(AF_INET, (void *)&addr.sin_addr, ip, sizeof(ip));
    printf("get_broadcast: %s:%d\n", ipaddr, ntohs(addr.sin_port));

    addr.sin_port = htons(UDPPORT);
    sendto(ufd, buffer, numbytes, 0, (struct sockaddr *)&addr, len);
    return 0;
}

int process_child(int client_fd) {
    char buffer[4096];
    char *ptr = buffer;
    int size = sizeof(buffer);
    int numbytes;
    int readed = 0;

    while(1) {
        numbytes = recv(client_fd, ptr, size, 0);
        if (numbytes == 0) {
            close(client_fd);
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
            char *resp = "HTTP/1.0 400 Invalid Request\r\n\r\n";
            send(client_fd, resp, strlen(resp), 0);
            close(client_fd);
            return 0;
        }
    }

        if ((buffer[0] == 'g' || buffer[0] == 'G') &&
        (buffer[1] == 'e' || buffer[1] == 'E') &&
        (buffer[2] == 't' || buffer[2] == 'T')) {
    } else {
        char *resp = "HTTP/1.0 501 Invalid Command\r\n\r\n";
        send(client_fd, resp, strlen(resp), 0);
        close(client_fd);
        return 0;
    }

    char *filename = "test.xml";
    printf("filename: %s\n", filename);

    struct stat buf;
    if (stat(filename, &buf) == -1) {
        char *resp = "HTTP/1.0 404 Not ExistFile\r\n\r\n";
        send(client_fd, resp, strlen(resp), 0);
        close(client_fd);
        return 0;
    }

    char *resp = "HTTP/1.0 200 OK\r\n";
    send(client_fd, resp, strlen(resp), 0);

    char sizebuffer[1024];
    snprintf(sizebuffer, sizeof(sizebuffer), "Content-Length: %d\r\n\r\n",
            (int)buf.st_size);
    send(client_fd, sizebuffer, strlen(sizebuffer), 0);

    FILE *fp = fopen(filename, "r");
    while(1) {
        int readed = fread(sizebuffer, 1, 1024, fp);
        if (readed == 0)
            break;

        send(client_fd, sizebuffer, readed, 0);
    }

    fclose(fp);
    close(client_fd);
    return 1;
}

int process_tcp(int tfd) {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int client_fd = accept(tfd, (struct sockaddr *)&addr, &len);
    if (client_fd == -1) {
        fprintf(stderr, "ERROR: accept error\n");
        return -1;
    }

    char ip[32];
    const char *ipaddr = inet_ntop(AF_INET, (void *)&addr.sin_addr, ip, sizeof(ip));
    printf("client: %s:%d\n", ipaddr, ntohs(addr.sin_port));

    if (fork() == 0) {
        close(tfd);
        process_child(client_fd);
        exit(0);
    } else {
        close(client_fd);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int stfd, sufd;
    struct sockaddr_in taddr, uaddr;
    int ret;
    fd_set readfds;

    memset(&taddr, 0, sizeof(taddr));
    memset(&uaddr, 0, sizeof(uaddr));

    taddr.sin_family = AF_INET;
    taddr.sin_port = htons(TCPPORT); 
    taddr.sin_addr.s_addr = htonl(INADDR_ANY);

    uaddr.sin_family = AF_INET;
    uaddr.sin_port = htons(UDPPORT); 
    uaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    stfd = createTcpServerSocket(&taddr);
    sufd = createUdpServerSocket(&uaddr);

    if (stfd == -1 || sufd == -1) {
        fprintf(stderr, "ERROR: socket create error\n");
        exit(1);
    }

    char *ip = NULL;
    localIP(&ip);
    printf("ip: %s\n", ip);

    struct timeval tm;

    while(1) {
        FD_ZERO(&readfds);
        FD_SET(stfd, &readfds);
        FD_SET(sufd, &readfds);

        tm.tv_sec = 1;
        tm.tv_usec = 0;
        ret = select(sufd+1, &readfds, NULL, NULL, &tm);
        if (ret == -1) {
            fprintf(stderr, "ERROR: socket select error\n");
            break;
        }
        
        if (ret != 0) {
            if (FD_ISSET(stfd, &readfds)) {
                process_tcp(stfd);
            }
            if (FD_ISSET(sufd, &readfds)) {
                process_udp(sufd);
            }
        } else {
            printf("Timeout\n");
        }
    } 

    close(stfd);
    close(sufd);

    return 0;
}
