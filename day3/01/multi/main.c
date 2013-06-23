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
    printf("get_multicast: %s:%d\n", ipaddr, ntohs(addr.sin_port));

    return 0;
}

int process_tcp(int tfd) {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int newfd = accept(tfd, (struct sockaddr *)&addr, &len);
    if (newfd == -1) {
        return -1;
    }

    char ip[32];
    const char *ipaddr = inet_ntop(AF_INET, (void *)&addr.sin_addr, ip, sizeof(ip));
    printf("client: %s:%d\n", ipaddr, ntohs(addr.sin_port));
    close(newfd);
    return 0;
}

void multicast() {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    memset(&addr, 0, sizeof(addr));

    int ufd = socket(AF_INET, SOCK_DGRAM, 0);
    if (ufd == -1) {
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(UDPPORT);
    inet_aton("226.0.0.1", &addr.sin_addr);

    int ttl=1000;
    setsockopt(ufd, IPPROTO_IP, IP_MULTICAST_TTL, (void *)&ttl, sizeof(ttl)); 
    char *msg = "notification";
    int numbytes = sendto(ufd, msg, strlen(msg), 0, (struct sockaddr *)&addr, len);
    if (numbytes == -1) {
        fprintf(stderr, "ERROR: multicast\n");
    }
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

    struct ip_mreq join_addr;
    memset(&join_addr, 0, sizeof(struct ip_mreq));
    join_addr.imr_multiaddr.s_addr=inet_addr("226.0.0.1");
    join_addr.imr_interface.s_addr=htonl(INADDR_ANY);

    setsockopt(sufd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&join_addr, sizeof(join_addr));

    if (stfd == -1 || sufd == -1) {
        fprintf(stderr, "ERROR: socket create error\n");
        exit(1);
    }

    char *ip = NULL;
    localIP(&ip);
    printf("ip: %s\n", ip);

    multicast();

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
