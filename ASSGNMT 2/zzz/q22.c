#include <stdio.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include<fcntl.h>
#include <stdlib.h> 
#include<pthread.h>

int main(){
  sem_t* sem1 = sem_open("/sem1",O_CREAT|O_EXCL,0666,1);
  sem_t* sem2 = sem_open("/sem2",O_CREAT|O_EXCL,0666,0);

  int id = fork();

  if(id==0){
    srand(time(0)+getpid());
    for(int i=0;i<10;i++){
      sem_wait(sem2);
      printf("Child in iteration %d\n",i);
      int t = rand()%10;
      sleep(t);
      sem_post(sem1);
    }
  }
  else{
    srand(time(0)+getpid());
    for(int i=0;i<10;i++){
      sem_wait(sem1);
      printf("Parent in iteration %d\n",i);
      int t = rand()%10;
      sleep(t);
      sem_post(sem2);
    }
  }
  sem_unlink("/sem1");
  sem_unlink("/sem2");
  wait(NULL);
  return 0;
}