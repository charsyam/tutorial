#include    <sys/types.h>
#include    <signal.h>

int main(int argc, char *argv[]) {
    kill(atoi(argv[1]), SIGKILL);
    return 0;
}
