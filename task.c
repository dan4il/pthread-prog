#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#define SIZE 100
#define MAX 1000000

int start=0;//start*2 -first value of array
int glob[SIZE]={};
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_write,cond_calc;
void* f_write(void* arg)
{
  FILE* file;
  file=fopen("result.txt","w");
  if(file!=NULL)
  {
    printf("work with file started..\n");
    for(int k=0;k<MAX;k+=SIZE)
    {
      pthread_mutex_lock(&lock);
      for(int i=0;i<SIZE;i++)
      {
          fprintf(file,"%d\t",glob[i]);

      }
      fprintf(file, "\n" );
      printf("put in file 100 values\n");
      pthread_cond_signal(&cond_calc);
      pthread_cond_wait(&cond_write,&lock);
      pthread_mutex_unlock(&lock);
      }
      printf("---------\n" );

  }
  else
  {
    printf("error to open a file\n" );
  }
  fclose(file);
  printf("work with file finished..\n");
  pthread_exit(0);
}

void* calc_even(void* arg)
{
  for(int k=0;k<MAX;k+=SIZE)
  {
    pthread_mutex_lock(&lock);

    for(int i=0;i<SIZE;i++)
    {
      glob[i]=start*2;
      start++;

    }
    printf("array full of values\n");
    pthread_cond_wait(&cond_calc,&lock);
    pthread_cond_signal(&cond_write);
    pthread_mutex_unlock(&lock);

  }
  pthread_exit(0);
}

void* calc_odd(void* arg)
{
  for(int i=0;i<SIZE;i++)
    glob[i]=1+i*2;
  return NULL;
}
int main()
{

  pthread_t thread1,thread2;
  pthread_cond_init(&cond_write,NULL);
  pthread_cond_init(&cond_calc,NULL);

  pthread_create(&thread1,NULL,calc_even,NULL);
  pthread_create(&thread2,NULL,f_write,NULL);
  pthread_join(thread1,NULL);
  pthread_join(thread2,NULL);


  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&cond_write);
  pthread_cond_destroy(&cond_calc);
  return 0;

}
