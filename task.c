#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#define SIZE 100
#define MAX 100000

int even_start=0;//start*2 -first value of even array
int odd_start=0;//start*2+1 - first values of odd array
unsigned int glob[2*SIZE+1]={};
// glob[2*SIZE]=0 -not ready for output;=1 one part(even part)-ready for output;
  // =2-ready for output

pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_write,cond_calc_even,cond_calc_odd;

void* f_write(void* arg)
{
  FILE* file;
  file=fopen("result.txt","w");
  if(file!=NULL)
  {
    printf("work with file started..\n");
    for(int k=0;k<MAX;k++)
    {
      pthread_mutex_lock(&lock);
      while(glob[2*SIZE]!=2)       pthread_cond_wait(&cond_write,&lock);

      for(int i=0;i<SIZE;i++)
      {
          fprintf(file,"%d\t",glob[i]);

      }
      fprintf(file, "\n" );//to customize output
      for(int i=SIZE;i<2*SIZE;i++)
      {
          fprintf(file,"%d\t",glob[i]);

      }
      glob[2*SIZE]=0;
      fprintf(file, "\n" );
      printf("put in file 200 values\n");
      pthread_cond_signal(&cond_calc_even);
      pthread_mutex_unlock(&lock);
      }
      printf("-----------\n" );

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
  for(int k=0;k<MAX;k++)
  {
    pthread_mutex_lock(&lock);
    while(glob[2*SIZE]!=0)     pthread_cond_wait(&cond_calc_even,&lock);

    for(int i=0;i<SIZE;i++)
    {
      glob[i]=even_start*2;
      even_start++;

    }
    glob[2*SIZE]=1;
    printf("array full of even values\n");
    pthread_cond_signal(&cond_calc_odd);
    pthread_mutex_unlock(&lock);

  }
  pthread_exit(0);
}

void* calc_odd(void* arg)
{

  for(int k=0;k<MAX;k++)
  {
    pthread_mutex_lock(&lock);
    while(glob[2*SIZE]!=1)     pthread_cond_wait(&cond_calc_odd,&lock);

    for(int i=SIZE;i<2*SIZE;i++)
      {
        glob[i]=1+odd_start*2;
        odd_start++;
      }
      glob[2*SIZE]=2;
      printf("array full of odd values\n");
      pthread_cond_signal(&cond_write);
      pthread_mutex_unlock(&lock);

  }


  pthread_exit(0);
}
int main()
{

  pthread_t thread1,thread2,thread3;
  pthread_cond_init(&cond_write,NULL);
  pthread_cond_init(&cond_calc_even,NULL);
  pthread_cond_init(&cond_calc_odd,NULL);

  pthread_create(&thread1,NULL,calc_even,NULL);
  pthread_create(&thread3,NULL,calc_odd,NULL);
  pthread_create(&thread2,NULL,f_write,NULL);
  pthread_join(thread1,NULL);
  pthread_join(thread2,NULL);
  pthread_join(thread3,NULL);

  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&cond_write);
  pthread_cond_destroy(&cond_calc_even);
  pthread_cond_destroy(&cond_calc_odd);

  return 0;

}
