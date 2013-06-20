#include <sys/types.h> 
#include <sys/stat.h> 
#include <stdio.h> 
#include <fcntl.h> 
#include <stdio.h>

int main()
{
    pid_t   pid;

    if (( pid = fork()) < 0)
        exit(0);
    else if(pid != 0)
        exit(0);
    
    setsid();
    chdir("/");

    while(1)
    {
        sleep(1);
    }

    return 0;
}
