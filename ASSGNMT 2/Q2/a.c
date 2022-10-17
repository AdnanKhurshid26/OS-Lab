#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define ITERATIONS 10

void displayData(char *processName, int iteration)
{

    printf("Process  : %s  | PID : %d | Iteration : %d\n", processName, getpid(), iteration + 1);
    int x = rand() % 5;
    sleep(x);
}

int main()
{

    int pidX = -1;
    int pidY = -1;

    pidX = fork();
    if (pidX == 0)
    {
        // Inside child X
        srand(time(0) + getpid());
        for (int i = 0; i < ITERATIONS; i++)
        {

            displayData("X", i);
        }
    }
    else
    {
        pidY = fork();
        if (pidY == 0)
        {
            // Inside Child Y
            srand(time(0) + getpid());
            for (int i = 0; i < ITERATIONS; i++)
            {
                displayData("Y", i);
            }
        }
    }

    for (int i = 1; i <= 2; i++)
    {
        wait(NULL);
    }

    return 0;
}