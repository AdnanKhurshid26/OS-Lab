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

int main()
{

    int *var = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    int *writing = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);

    if (var == MAP_FAILED)
    {
        perror("In mmap()");
        exit(1);
    }

    int readers;
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
                    while (*writing)
                        ;
                    printf("Reader %d read %d\n", r, *var);
                    int number = (rand() % (12 - 2 + 1)) + 1;
                    sleep(number);
                }

                return 0l;
            }
        }

        while (wait(NULL) > 0)
            ;
        return 0;
    }

    while (wcount--)
    {
        (*writing) = 1;
        srand(0);
        int x = rand() % 100;
        *var += x;
        printf("Writer writing %d\n", *var);
        sleep(2);
        (*writing) = 0;
        int number = (rand() % (8 - 1 + 1)) + 2;
        sleep(number);
    }

    wait(NULL);
    
    return 0;
}
