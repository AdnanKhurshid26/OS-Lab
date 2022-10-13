#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_LEN 200

int main()
{
    int lc;
    printf("Enter number of listeners : ");
    scanf("%d", &lc);

    if (lc == 0)
    {
        printf("Listeners cannot be zero\n");
        exit(0);
    }

    // dynamic array of file descriptors
    int **fdarr = (int **)malloc(lc * sizeof(int *));
    for (int i = 0; i < lc; i++)
        fdarr[i] = (int *)malloc(2 * sizeof(int));

    for (int i = 0; i < lc; i++)
    {
        int res = pipe(fdarr[i]);
        if (res == -1)
        {
            printf("Pipe creation failed for listener %d", i + 1);
            return 1;
        }
    }

    // forking a listener creator process
    int listener_creator = fork();
    if (listener_creator == -1)
    {
        printf("Fork failed");
        exit(0);
    }

    if (listener_creator == 0)
    {
        for (int i = 0; i < lc; i++)
        {
            int listener = fork();
            if (listener < 0)
            {
                perror("A listener creation failed\n");
                exit(1);
            }
            int listener_id = i + 1;
            if (listener == 0)
            {
                char data[BUFFER_LEN];
                while (1)
                {
                    int res = read(fdarr[listener_id - 1][0], data, BUFFER_LEN);
                    if (res > 0)
                    {
                        if (strcmp(data, "EOF") == 0)
                        {
                            printf("\nListener %d finished\n", listener_id);
                            break;
                        }
                        printf("Listener %d: %s\n", listener_id, data);
                        sleep(0.5);
                    }
                }

                close(fdarr[listener_id - 1][0]);
            }
        }
    }
    else
    {
        // Close all read ends of pipes
        for (int i = 0; i < lc; i++)
        {
            close(fdarr[i][0]);
        }
        // Open file
        FILE *fp = fopen("weather.txt", "r");
        if (fp == NULL)
        {
            printf("File open failed");
            return 1;
        }
        // Read file line by line
        char buffer[BUFFER_LEN];
        while (fgets(buffer, BUFFER_LEN, fp) != NULL)
        {
            // Write to all pipes
            for (int i = 0; i < lc; i++)
            {
                write(fdarr[i][1], buffer, BUFFER_LEN);
            }
        }

        // Send something to all pipes to indicate end of file
        for (int i = 0; i < lc; i++)
        {
            write(fdarr[i][1], "EOF", BUFFER_LEN);
        }

        // Close all write ends of pipes
        for (int i = 0; i < lc; i++)
        {
            close(fdarr[i][1]);
        }

        // Wait for child process to finish
        wait(NULL);
    }

    return 0;
}