/// Taken from Posix Tutorial -- https://computing.llnl.gov/tutorials/pthreads/
// Example demonstrates race condition 
// R. Trenary, 1/26/12
//
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#define NUM_THREADS     5

void *GoodbyeDennis(void *threadid)
{long tid;
   //char  * File = malloc(strlen((char *) FileName)+1);
     //strncpy(File,(char *) FileName,strlen( (char *) FileName)+1);
   tid = (long)threadid;
   printf("Goodbye, Dennis ! It's me, thread #%ld!\n", tid);
  pthread_exit(NULL);
}
void *SomeThread(void *SomeArg)
{
char *SomeName;
SomeName=(char * )SomeArg;
printf("%s SomeName in thread\n",SomeName);
pthread_exit(NULL);

}

int main (int argc, char *argv[])
{
   pthread_t threads[NUM_THREADS];
   int rc;   long t;
   pthread_t *T; T=(pthread_t *) malloc(sizeof (pthread_t));
rc = pthread_create(T,NULL,  SomeThread, (void *) "string");

     //char  *  FakeFile  = "FileName"; For another day 
   for(t=0; t<NUM_THREADS; t++){
      printf("In main: creating thread %ld\n", t);
     rc = pthread_create(&threads[t], NULL, GoodbyeDennis, (void *)t);
      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   }
// testing create syntax


   /* Last thing that main() should do */
	pthread_exit((void *)42);
  return 42;
}



  



