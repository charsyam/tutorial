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
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "util.h"

#define UDPPORT     2001    
#define TCPPORT     2002

char server_ip[32];

struct file {
    char name[128];
    int size;
};

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

int process_udp(int ufd, char *localip) {
    char buffer[1024];
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    int numbytes = recvfrom(ufd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &len);
    buffer[numbytes] = 0;

    char ip[32];
    const char *ipaddr = inet_ntop(AF_INET, (void *)&addr.sin_addr, ip, sizeof(ip));
    if (!strcmp(ip, localip)) 
	    return 0;

    strcpy(server_ip, ip);
    printf("get_broadcast: %s:%d\n", ipaddr, ntohs(addr.sin_port));

    return 0;
}

void parseFile(xmlNode * cur_node, struct file *f) {
    xmlNode *child;
    for (child = cur_node; child; child = child->next) {
        if (child->type == XML_ELEMENT_NODE && !strcmp(child->name, "name")) {
            xmlNode *node;
            for (node = child->children; node; node = node->next) {
                if (node->type == XML_TEXT_NODE) {
                    strcpy(f->name, node->content);
                }
            }
        }
        if (child->type == XML_ELEMENT_NODE && !strcmp(child->name, "size")) {
            xmlNode *node;
            for (node = child->children; node; node = node->next) {
                if (node->type == XML_TEXT_NODE) {
                    f->size = atoi(node->content);
                }
            }
        }
    }
}


void print_element_names(xmlNode * node)
{
    xmlNode *cur_node = NULL;

    for (cur_node = node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!strcmp(cur_node->name, "file")) {
                if (cur_node->children) {
                    struct file f;
                    parseFile(cur_node->children, &f);
                    printf("name: %s, size: %d\n", f.name, f.size);
                }
            }
        }
    }
}

void parseXmlReal(char *filename) {
    xmlDocPtr doc; /* the resulting document tree */

    doc = xmlReadFile(filename, NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse %s\n", filename);
    return;
    }

    xmlNode *root_element = NULL;
    root_element = xmlDocGetRootElement(doc);
    print_element_names(root_element->children);

    xmlFreeDoc(doc);
}

int parseXml(char *filename) {
    LIBXML_TEST_VERSION
    parseXmlReal(filename);
    xmlCleanupParser();
    xmlMemoryDump();
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

    char *filename="1";
    FILE *fp = fopen(filename, "w");
    int find_header = 0;

    while(1) {
        numbytes = recv(sfd, buffer, 4096, 0);
        if (numbytes == 0)
            break;

        if (find_header == 0) {
            char *ptr = strstr(buffer, "\r\n\r\n");
            ptr += 4;
            int pos = ptr - buffer; 
            fwrite(ptr, 1, numbytes - pos, fp);
        } else {
            fwrite(buffer, 1, numbytes, fp);
        }
    }

    fclose(fp);
    parseXml(filename);

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
                process_udp(sufd, ip);
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
