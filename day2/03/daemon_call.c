#include <sys/types.h> 
#include <sys/stat.h> 
#include <stdio.h> 
#include <fcntl.h> 
#include <stdio.h>

int main()
{
    daemon(0,0);
    while(1)
    {
        sleep(1);
    }

    return 0;
}
