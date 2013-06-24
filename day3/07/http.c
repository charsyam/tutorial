#include    <stdio.h>
#include    <string.h>

int parse_command(char *cmdline, char *cmd, char *uri, char *protocol) {
    char *token = NULL;
    char *next;

    token = strtok_r(cmdline, " ", &next);
    if (token == NULL)
        return 0;

    strcpy(cmd, token);

    token = strtok_r(NULL, " ", &next);
    if (token == NULL)
        return 0;

    strcpy(uri, token);

    token = strtok_r(NULL, " ", &next);
    if (token == NULL)
        return 0;

    strcpy(protocol, token);
    return 1;
}

int parse_options(char *line, char *name, char *value) {
    char *token = NULL;
    char *next;

    token = strtok_r(line, ": ", &next);
    if (token == NULL)
        return 0;

    strcpy(name, token);

    token = strtok_r(NULL, ": ", &next);
    if (token == NULL)
        return 0;

    strcpy(value, token);
    return 1;
}

int http_parser(char *header) {
    char *token = NULL;
    char *next;
    
    char cmd[1024];
    char uri[1024];
    char protocol[1024];

    char name[1024];
    char value[1024];

    token = strtok_r(header, "\r\n", &next);
    int line = 0;
    while(token) {
        printf("%s\n", token);
        if (line == 0) {
            parse_command(token, cmd, uri, protocol);
            printf("cmd: %s, uri: %s, protocol: %s\n", cmd, uri, protocol);
        } else {
            parse_options(token, name, value);
            printf("name: %s, value: %s\n", name, value);
        }

        token = strtok_r(NULL, "\r\n", &next);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    char header[] = "GET / HTTP/1.1\r\nHost: www.naver.com\r\nconnection: close\r\n\r\n";
    http_parser(header);
    return 0;
}
