#include <stdio.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include<fcntl.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>
#define BUFF_SIZE 25

void add(int* p,int val){
  *p = (*p)+val;
  return;
}

int generate_random_number(){
  srand(time(NULL)+getpid());
  return 1+rand()%79;
}

typedef struct{
  int pid;
  int value;
} entry;

void consumer(entry (*buffer)[BUFF_SIZE],int* consume,int* sum,sem_t* full,sem_t* empty,sem_t* mutex){
  sem_wait(full);
  sem_wait(mutex);

  entry item = (*buffer)[*consume];
  *consume = (*consume+1)%BUFF_SIZE;
  add(sum,item.value);

  sem_post(mutex);
  sem_post(empty);
  return;
}

void producer(entry (*buffer)[BUFF_SIZE],int* produce,sem_t* full,sem_t* empty,sem_t* mutex){
  sem_wait(empty);
  sem_wait(mutex);

  int num = generate_random_number();
  entry e;e.pid = getpid();e.value=num;
  (*buffer)[*produce] = e;
  *produce = (*produce+1)%BUFF_SIZE;

  sem_post(mutex);
  sem_post(full);
  return;
}

int main(){
  int shmid1 = shmget(12222,1024,0666|IPC_CREAT);
  int*  produce = (int *)shmat(shmid1,NULL,0);
  *produce = 0;

  int shmid2 = shmget(13333,1024,0666|IPC_CREAT);
  int* consume = (int *)shmat(shmid2,NULL,0);
  *consume = 0;

  int shmid3 = shmget(14444,1024,0666|IPC_CREAT);
  int *sum = (int *)shmat(shmid3,NULL,0);
  *sum=0;

  int shmid4 = shmget(15555,1024,0666|IPC_CREAT);
  entry (*buffer)[BUFF_SIZE] = shmat(shmid4,NULL,0);
  
  sem_t* empty =sem_open("/empty",O_CREAT|O_EXCL,0666,BUFF_SIZE);
  sem_t* full = sem_open("/full",O_CREAT|O_EXCL,0666,0);
  sem_t* mutex = sem_open("/mutex",O_CREAT|O_EXCL,0666,1);

  int num_producer;int num_consumer;
  printf("Enter number of producer\n");
  scanf("%d",&num_producer);
  printf("Enter number of consumer\n");
  scanf("%d",&num_consumer);

  int i;
  for(i=0;i<num_consumer;i++){
    int id=fork();
    if(id==0)
        break;
  }

  if(i<num_consumer)
    consumer(buffer,consume,sum,full,empty,mutex);
  else{
    i=0;
    for(i=0;i<num_producer;i++){
      int id = fork();
      if(id==0)
        break;
    }
    if(i<num_producer)
      producer(buffer,produce,full,empty,mutex);
    else{
      while(wait(NULL)>0);
      printf("Sum is %d\n",*sum);
    }
  }
  shmdt(produce);
  shmdt(consume);
  shmdt(buffer);
  shmdt(sum);
  shmctl(shmid1,IPC_RMID,NULL);
  shmctl(shmid2,IPC_RMID,NULL);
  shmctl(shmid3,IPC_RMID,NULL);
  shmctl(shmid4,IPC_RMID,NULL);

  sem_unlink("/empty");
  sem_unlink("/full");
  sem_unlink("/mutex");
  return 0;
}