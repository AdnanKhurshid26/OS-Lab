#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
    int count;
    
    char msg[300];
    printf("Enter number of listeners : ");
    scanf("%d", &count);
    int listeners_left = count;
    int mainid = getpid();
    printf("Created Listener with process id %d\n", mainid);
    for(int i = 0; i < count; i++)
    {
        if(getpid()==mainid){
            int id = fork();
        }
    }

    while (true)
    {
        int fd = open("fifo", O_RDONLY);
        if (fd == -1)
        {
            printf("Error opening fifo\n");
            exit(1);
        }
        read(fd, msg, sizeof(msg));
        printf("Weather report : %s\n", msg);
        listeners_left--;
        if(listeners_left==0)
        {
            close(fd);
            listeners_left = count;
        }
        else{
            continue;
        }
    }
    



    return 0;
}