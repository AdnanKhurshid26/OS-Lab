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

    char msg[300];

    while (true)
    {

        printf("Enter weather report : ");
        fgets(msg, 200, stdin);
        msg[strcspn(msg, "\n")] = 0;
        fflush(stdin);
        int fd = open("fifo", O_WRONLY);
        if (fd == -1)
        {
            printf("Error opening fifo\n");
            exit(1);
        }

        write(fd, msg, sizeof(msg));
        printf("Weather report broadcast for all listeneres \n");
        close(fd);
    }

    return 0;
}