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

char *create_sem_name(int i)
{
	char *buffer;
	buffer = (char *)malloc(sizeof(char) * 12);
	sprintf(buffer, "/self%d", i);
	return buffer;
}

int N;

enum State
{
	THINKING,
	HUNGRY,
	EATING
};

enum State *state;

sem_t *mutex;

sem_t **self;

void eat(int i)
{
	// eating
	printf("Philosopher %d has taken chopstick %d and %d\n", i + 1, (i + N - 1) % N + 1, i + 1);
	printf("Philosopher %d is Eating\n", i + 1);
	sleep(3);
	printf("Philosopher %d has finished eating\n", i + 1);
	printf("Philosopher %d has put down chopstick %d and %d\n", i + 1, (i + N - 1) % N + 1, i + 1);
}

void test(int i)
{
	if ((state[(i + N - 1) % N] != EATING) && (state[i] == HUNGRY) && (state[(i + 1) % N] != EATING))
	{
		state[i] = EATING;
		sem_post(self[i]);
	}
}

void pickup(int i)
{
	state[i] = HUNGRY;
	printf("Philosopher %d is Hungry\n", i + 1);
	test(i);
	if (state[i] != EATING)
	{
		sem_wait(self[i]);
	}
}

void putdown(int i)
{
	state[i] = THINKING;
	test((i + N - 1) % N);
	test((i + 1) % N);
}

int main()
{

	printf("Enter number of philosophers : ");
	scanf("%d", &N);

	state = (enum State *)mmap(NULL, sizeof(enum State) * N, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);

	for (int i = 0; i < N; i++)
	{
		state[i] = THINKING;
		printf("Philosopher %d is thinking\n", i + 1);
	}

	mutex = sem_open("/mutex", O_CREAT, 0777, 1);

	self = (sem_t **)mmap(NULL, sizeof(sem_t *) * N, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);

	for (int i = 0; i < N; i++)
	{
		char *name = create_sem_name(i + 1);
		sem_unlink(name);
		self[i] = sem_open(name, O_CREAT, 0777, 0);
	}

	int philosopher_creator = fork();

	if (philosopher_creator == 0)
	{
		for (int i = 0; i < N; i++)
		{
			int philosopher = fork();
			if (philosopher == 0)
			{
				while (1)
				{
					srand(time(0));
					int t = rand() % 5 + 1;
					sleep(t);
					pickup(i);
					eat(i);
					putdown(i);
				}
				return 0;
			}
		}
		while (wait(NULL) > 0)
			;
		return 0;
	}

	wait(NULL);
	return 0;
}