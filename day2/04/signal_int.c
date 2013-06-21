#include    <stdio.h>
#include    <signal.h>

int g_continue = 1;

void sig_int_handler(int sig) {
    printf("signal\n");
    g_continue = 0;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sig_int_handler);

    while(g_continue) {
        printf("test\n");
        sleep(1);
    }

    printf("quit\n");
    return 0;
}
