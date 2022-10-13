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

// semaphore
sem_t *buffmutex;

// buffer size of 25
#define BUFFER_LEN 25

int total_consumed = 0;

// defining structure for buffer
typedef struct
{
    int front, rear;
    int items[BUFFER_LEN]; // shared circular queue
    int TOTAL;
    int produce_complete, consume_complete;
    int total_consumed;
} BUFFER;

// initialize the buffer
void init_buffer(BUFFER *buff)
{
    buff->front = 0;
    buff->rear = 0;
    buff->TOTAL = 0;
    buff->consume_complete = 0;
    buff->produce_complete = 0;

    buff->total_consumed = 0;
    for (int i = 0; i < BUFFER_LEN; i++)
    {
        buff->items[i] = 0;
    }
}

int main()
{
    int p, c;
    printf("Enter number of producers : ");
    scanf("%d", &p);
    printf("Enter number of consumers : ");
    scanf("%d", &c);
    int pitem, citem;
    pitem = (BUFFER_LEN / p);
    citem = (BUFFER_LEN / c);
    if (p == 0 && c == 0)
    {
        printf("Producers or Consumers cannot be zero\n");
        exit(0);
    }

    sem_unlink("/mutex");

    sem_t *buffmutex = sem_open("/mutex", O_CREAT, 0777, 1);
    sem_t *consumer_empty = sem_open("/consumer_empty", O_CREAT, 0777, -23);

    if (buffmutex == SEM_FAILED)
    {
        printf("Semaphore creation failed\n");
        exit(0);
    }

    // requesting the semaphore not to be held when completely unreferenced
    sem_unlink("/mutex");

    // allocating shared memory
    BUFFER *buff = (BUFFER *)mmap(NULL, sizeof(BUFFER), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    if (buff == MAP_FAILED)
    {
        perror("In mmap()");
        exit(1);
    }
    fprintf(stderr, "Shared memory segment allocated correctly (%d bytes) at %p.\n", (int)sizeof(BUFFER), buff);

    // initializing buffer
    /* initialize the buffer */
    init_buffer(buff);

    // forking a new producer creator process
    int prod_creator = fork();

    if (prod_creator < 0)
    {
        printf("Prodcuer Creator fork failed\n");
        exit(0);
    }

    if (prod_creator == 0)
    {
        for (int i = 0; i < p; i++)
        {
            int producer = fork();
            if (producer < 0)
            {
                perror("A producer creation failed\n");
                exit(1);
            }
            int prod_no = i + 1;
            if (producer == 0)
            {
                int item_produced = 0;
                srand(time(0) + getpid());
                // loop until all item produced
                while (item_produced < pitem)
                {

                    sem_wait(buffmutex);
                    // produce item
                    int produce = random() % 80 + 1;
                    buff->items[buff->front] = produce;
                    printf("Producer %d produced %d at %d\n", prod_no, produce, buff->front);
                    buff->front = ((buff->front + 1) % BUFFER_LEN);
                    sem_post(buffmutex);
                    item_produced++;
                }
                exit(0);
            }
        }
        wait(NULL);
        buff->produce_complete = 1;
        exit(0);
    }

    // forking a new consumer creator process

    int cons_creator = fork();

    if (cons_creator < 0)
    {
        printf("Consumer Creator fork failed\n");
        exit(0);
    }
    if (cons_creator == 0)
    {
        for (int i = 0; i < c; i++)
        {
            int consumer = fork();
            if (consumer < 0)
            {
                perror("A consumer creation failed\n");
                exit(1);
            }
            int cons_no = i + 1;
            if (consumer == 0)
            {
                srand(time(0) + getpid());
                int item_consumed = 0;
                // loop until all item consumed
                while (item_consumed < citem)
                {

                    // consume item
                    sem_wait(buffmutex);
                    if (buff->front == buff->rear)
                    {
                        sem_post(buffmutex);
                        continue;
                    }
                    int consume = buff->items[buff->rear];
                    printf("Consumer %d consumed %d at %d\n", cons_no, consume, buff->rear);
                    buff->rear = (buff->rear + 1) % BUFFER_LEN;
                    buff->TOTAL += consume;
                    buff->total_consumed++;
                    // release the semaphore
                    sem_post(buffmutex);
                    item_consumed++;
                }
                if (buff->total_consumed == citem * c)
                {
                    while (buff->total_consumed < BUFFER_LEN)
                    {
                        sem_wait(buffmutex);
                        if (buff->front == buff->rear)
                        {
                            sem_post(buffmutex);
                            continue;
                        }
                        int consume = buff->items[buff->rear];
                        printf("Consumer %d consumed %d at %d\n", cons_no, consume, buff->rear);
                        buff->rear = (buff->rear + 1) % BUFFER_LEN;
                        buff->TOTAL += consume;
                        buff->total_consumed++;
                        sem_post(buffmutex);
                    }
                }
                exit(0);
            }
        }
        wait(NULL);
        buff->consume_complete = 1;
        exit(0);
    }

    wait(NULL);

    while (buff->produce_complete == 0 || buff->consume_complete == 0)
    {
        sleep(1);
    }

    /* freeing the reference to the semaphore */
    sem_close(buffmutex);
    printf("TOTAL -> %d\n", buff->TOTAL);

    /* release the shared memory space */
    munmap(buff, sizeof(BUFFER));

    return 0;
}