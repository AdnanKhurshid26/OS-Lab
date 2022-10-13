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

sem_t *sem;

int main()
{

    int *var = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);

    if (var == MAP_FAILED)
    {
        perror("In mmap()");
        exit(1);
    }

    int readers;
    sem_unlink("/sem");
    sem = sem_open("/sem", O_CREAT, 0777, 1);
    printf("Enter number of readers : ");
    scanf("%d", &readers);

    int wcount;

    printf("Enter how many times you want to write : ");
    scanf("%d", &wcount);

    // creating readers

    int reader_creator = fork();

    if (reader_creator == 0)
    {
        for (int i = 0; i < readers; i++)
        {
            int reader = fork();

            if (reader < 0)
            {
                perror("In fork()");
                exit(1);
            }

            int r = i + 1;

            if (reader == 0)
            {

                srand(time(0));
                int rcount = (rand() % (8 - 4 + 1)) + 4;

                while (rcount--)
                {
                    sem_wait(sem);
                    sem_post(sem);
                    printf("Reader %d read %d\n", r, *var);
                    int number = (rand() % (5 - 1 + 1)) + 1;
                    sleep(number);
                }

                return 0l;
            }
        }

        while(wait(NULL)>0);
        return 0;
    }

    while (wcount--)
    {
        sem_wait(sem);
        srand(0);
        int x = rand() % 100;
        *var += x;
        printf("Writer writing %d\n", *var);
        int number = (rand() % (5 - 2 + 1)) + 2;
        sleep(number);
        sem_post(sem);
        number = (rand() % (3 - 1 + 1)) + 2;
        sleep(number);
    }

    wait(NULL);

    return 0;
}
