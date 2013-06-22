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

char server_ip[32];

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

    const char *ipaddr = inet_ntop(AF_INET, (void *)&addr.sin_addr, server_ip, sizeof(server_ip));
    printf("get_broadcast: %s:%d\n", ipaddr, ntohs(addr.sin_port));

    return 0;
}


int process_tcp(char *ip) {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        return -1;
    }

    memset(&addr, 0, len);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TCPPORT);
    inet_aton(ip, &addr.sin_addr);

    if (connect(sfd, (struct sockaddr *)&addr, len) == -1) {
        close(sfd);
        return -1;
    }

    char *cmd = "GET /etc/hosts\r\n";
    send(sfd, cmd, strlen(cmd), 0);

    char buffer[4096];
    int numbytes;

    while(1) {
        numbytes = recv(sfd, buffer, 4096, 0);
        if (numbytes == 0)
            break;

        buffer[numbytes] = 0;
        printf("%s", buffer);        
    }

    close(sfd);
    return 0;
}

int main(int argc, char *argv[])
{
    int sufd;
    struct sockaddr_in uaddr;
    int ret;
    fd_set readfds;

    memset(&uaddr, 0, sizeof(uaddr));

    uaddr.sin_family = AF_INET;
    uaddr.sin_port = htons(UDPPORT); 
    uaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    sufd = createUdpServerSocket(&uaddr);

    if (sufd == -1) {
        fprintf(stderr, "ERROR: socket create error\n");
        exit(1);
    }

    char *ip = NULL;
    localIP(&ip);
    printf("ip: %s\n", ip);

    struct timeval tm;

    broadcast(UDPPORT);
    while(1) {
        FD_ZERO(&readfds);
        FD_SET(sufd, &readfds);

        tm.tv_sec = 5;
        tm.tv_usec = 0;
        ret = select(sufd+1, &readfds, NULL, NULL, &tm);
        if (ret == -1) {
            fprintf(stderr, "ERROR: socket select error\n");
            break;
        }
        
        if (ret != 0) {
            if (FD_ISSET(sufd, &readfds)) {
                process_udp(sufd);
            }
        } else {
            if (server_ip[0] == 0) {
                broadcast(UDPPORT);
            } else {
                process_tcp(server_ip); 
            }
        }
    } 

    close(sufd);

    return 0;
}
