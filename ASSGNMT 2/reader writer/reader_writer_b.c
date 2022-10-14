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

sem_t *wrtMutex, *waitMutex;

int main()
{

    int *var = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    int *w_wrtcount = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    int *w_waitcount = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);

    if (var == MAP_FAILED)
    {
        perror("In mmap()");
        exit(1);
    }

    int readers, writers;
    sem_unlink("/wrtMutex");
    wrtMutex = sem_open("/wrtMutex", O_CREAT, 0777, 1);
    sem_unlink("/waitMutex");
    waitMutex = sem_open("/waitMutex", O_CREAT, 0777, 1);
    printf("Enter number of readers : ");
    scanf("%d", &readers);
    printf("Enter number of writers : ");
    scanf("%d", &writers);

    int writer_creator = fork();

    if (writer_creator == 0)
    {
        for (int i = 0; i < writers; i++)
        {
            int writer = fork();

            int w = i + 1;

            if (writer == 0)
            {
                srand(time(0));
                int wcount = (rand() % (4 - 2 + 1)) + 2;
                while (wcount--)
                {
                    sem_wait(waitMutex);
                    (*w_waitcount)++;
                    sem_post(waitMutex);

                    sem_wait(wrtMutex);

                    (*w_wrtcount)++;

                    sem_wait(waitMutex);
                    (*w_waitcount)--;
                    sem_post(waitMutex);

                    srand(time(0));
                    int x = rand() % 100;
                    *var += x;
                    printf("Writer %d writing %d\n", w, *var);
                    sleep(2);
                    printf("Writer %d finished writing %d\n", w, *var);
                    (*w_wrtcount)--;
                    sem_post(wrtMutex);

                    if (wcount)
                    {
                        srand(time(0));
                        int number = (rand() % (12 - 6 + 1)) + 6;
                        sleep(number);
                        printf("Writer %d wants to write again\n", w);
                    }
                }
                return 0;
            }
        }
        while (wait(NULL) > 0)
            ;

        return 0;
    }

    int reader_creator = fork();

    if (reader_creator == 0)
    {
        for (int i = 0; i < readers; i++)
        {
            int reader = fork();

            int r = i + 1;

            if (reader == 0)
            {

                srand(time(0));
                int rcount = (rand() % (8 - 2 + 1)) + 2;

                while (rcount--)
                {
                    while (*w_waitcount > 0 || *w_wrtcount > 0)
                        ;
                    printf("Reader %d read %d\n", r, *var);
                    if (rcount)
                    {
                        srand(time(0));
                        int number = (rand() % (8 - 1 + 1)) + 1;
                        sleep(number);
                    }
                }

                return 0;
            }
        }
    

        while (wait(NULL) > 0)
            ;
        return 0;
    }

    while (wait(NULL) > 0)
        ;
    sem_unlink("/wrtMutex");
    sem_unlink("/waitMutex");
    sem_destroy(wrtMutex);
    sem_destroy(waitMutex);

    return 0;
}
