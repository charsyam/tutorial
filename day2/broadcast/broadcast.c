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

    serverINETAddress.sin_family = AF_INET;      /* host byte order */
    serverINETAddress.sin_port = htons(UDPPORT);  /* short, network byte order */
    serverINETAddress.sin_addr.s_addr = INADDR_BROADCAST; /* BROADCAST */
    bzero(&(serverINETAddress.sin_zero),8);

    if ((serverFd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
    {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)); /* BROADCAST */

    if ((numbytes=sendto(serverFd, udp_buf, strlen(udp_buf),0, serverSockAddrPtr, serverLen)) == -1) 
    {
        perror("sendto");
        exit(1);
    }
    else
    {
        printf("UDP로 전송된 데이터[BYTE]:%s[%d]\n\r", udp_buf, numbytes);
        udp_buf[0]='\0';
    }

    close(serverFd);
}
