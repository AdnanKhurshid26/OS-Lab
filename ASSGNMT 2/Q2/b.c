#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>

#define ITERATIONS 10
sem_t *sem1;

void displayData(char *processName, int iteration)
{

    printf("Process  : %s  | PID : %d | Iteration : %d\n", processName, getpid(), iteration + 1);
    int x = rand() % 3;
    sleep(x);
}

int main()
{

    sem_unlink("sem1");
    sem1 = sem_open("sem1", O_CREAT, 0777, 0);

    int pidX = -1;
    int pidY = -1;

    pidX = fork();
    if (pidX == 0)
    {
        // Inside child X
        srand(time(0) + getpid());
        for (int i = 0; i < ITERATIONS; i++)
        {

            sem_wait(sem1);
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
                sem_post(sem1);
            }
        }
    }

    for (int i = 1; i <= 2; i++)
    {
        wait(NULL);
    }

    return 0;
}