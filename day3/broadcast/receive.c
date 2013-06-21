/* socket header file */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> /* AF_INET socket */
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define UDPPORT     2001    /* UDP PORT number */

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

    if ((serverFd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
    {
        perror("socket");
        exit(1);
    }

    bind(serverFd, serverSockAddrPtr, serverLen);

    char buffer[4096];
    while (1) {
        numbytes = recvfrom(serverFd, buffer, sizeof(buffer), 0,
                            serverSockAddrPtr, &serverLen);
        buffer[numbytes] = 0;
        printf("UDP로 전송된 데이터[BYTE]:%s[%d]\n\r", buffer, numbytes);
    }

    close(serverFd);
    return 0;
}
