#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>

#define ITERATIONS 10
sem_t *semx;
sem_t *semy;
sem_t *semz;

void displayData(char *processName, int iteration)
{

    printf("Process  : %s  | PID : %d | Iteration : %d\n", processName, getpid(), iteration + 1);
    int x = rand() % 5;
    sleep(x);
}

int main()
{
 
    sem_unlink("semx");
    semx = sem_open("semx", O_CREAT, 0777, 1);
    sem_unlink("semy");
    semy = sem_open("semy", O_CREAT, 0777, 0);
    sem_unlink("semz");
    semz = sem_open("semz", O_CREAT, 0777, 0);

    int pidX ;
    int pidY ;
    int pidZ ;

    pidX = fork();
    if (pidX == 0)
    {
        // Inside child X
        srand(time(0) + getpid());
        for (int i = 0; i < ITERATIONS; i++)
        {

            sem_wait(semx);
            displayData("X", i);
            sem_post(semy);
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
                sem_wait(semy);
                displayData("Y", i);
                sem_post(semz);
            }
        }
        else{
            pidZ = fork();
            if(pidZ == 0){
                srand(time(0) + getpid());
                for (int i = 0; i < ITERATIONS; i++)
                {
                    sem_wait(semz);
                    displayData("Z", i);
                    sem_post(semx);
                }
            }
        }
    }

    for (int i = 1; i <= 2; i++)
    {
        wait(NULL);
    }

    return 0;
}