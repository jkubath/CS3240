
/// Taken from Posix Tutorial -- https://computing.llnl.gov/tutorials/pthreads/
// Example demonstrates Mutex control over Pies. Now add a Pie Producer
// And then a conditional ?
// R. Trenary, 1/26/12
//
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 5
int Done [NUM_THREADS];     // A flag for each thread. AND them
			// and one unfinished means we still have work

pthread_mutex_t MyMutex= PTHREAD_MUTEX_INITIALIZER; // A mutex and a cond (itional)
pthread_cond_t PiesToEat= PTHREAD_COND_INITIALIZER;  // Initialized

int PieceOfPies; // Important shared variable

//==================================
//    BAKER THREAD
//==================================
void *PieBaking(void *threadid)  // THE BAKER
{int i, AllDone;
int rc ; // Return Code
while (1)
{ AllDone=1;
	for (i=0;i<NUM_THREADS;i++) AllDone= ( Done[i] && AllDone);
	if  (AllDone) {printf("All Eaters are done \n"); break;}

	do {rc = pthread_mutex_trylock(&MyMutex); // does this lock if successful ?
	      } while (rc !=0);
	if (PieceOfPies == 0) {PieceOfPies +=2;
			printf("Baked a couple of of pies\n");
		}
			// make a couple of pies if needed


	pthread_cond_broadcast(&PiesToEat);
  pthread_mutex_unlock(&MyMutex);
}
	printf("Done Baking\n");
 	pthread_exit(NULL);
}
// ==================================
//   EATER THREAD
// ==================================
void *PieEating(void *threadid)
{
   long tid;
   int rc = -1; // Initialize to not acquired
   tid =  * ((long *) threadid);
   int PiecesEaten = 0;	 // Control Pieces Eaten by Thread
	Done[tid] = 0; // Not Done
     while (PiecesEaten < 2)
     {
	while ( rc !=0 )
	{   rc = pthread_mutex_trylock(&MyMutex);  // will return non zero if not acquired
	     usleep(100); // sleep and try again
	}
	printf(" Thread %lu acquired lock \n", tid);
	// pthread_mutex_lock(&MyMutex);
	    if (PieceOfPies == 0) (pthread_cond_wait(&PiesToEat, &MyMutex));
  	    printf("Eating pie for Dennis.  It's me, thread #%ld!\n", tid);
		PieceOfPies--; // critical section
	 pthread_mutex_unlock(&MyMutex);
	// and sleep some of it off
	usleep(100);
	rc = -1;
	printf("%d available piece of pies \n",PieceOfPies);
	PiecesEaten++; // Ate a piece of pie
       }
	Done[tid] = 1; // All done
	printf("Thread %lu is done and said so \n",tid);
         pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
   pthread_t threads[NUM_THREADS];
   pthread_t PieBaker;
   int rc;
   long t;
    long *Tptr[NUM_THREADS];
	PieceOfPies = 0;
   for(t=0; t<NUM_THREADS; t++)
    { Tptr[t] = (long *) malloc(sizeof(long));
     (*Tptr[t]) = (long) t;
      printf("In main: creating thread %lu\n", t);
      rc = pthread_create(&threads[t], NULL, PieEating, (void *)Tptr[t]);
      if (rc){
         printf("ERROR; return code from pthread_create() for eaters is %d\n", rc);
         exit(-1); }
     }
    	rc = pthread_create(&PieBaker, NULL, PieBaking, (void *)t);
       	if (rc){
         	printf("ERROR; return code from pthread_create() for baker is %d\n", rc);
        	 exit(-1);}

	printf("looks like pies are done cooking\n");
   		/* Last thing that main() should do */
	pthread_mutex_destroy(&MyMutex);
	pthread_cond_destroy(&PiesToEat);
  pthread_exit(NULL);
	printf("Done in main\n");
	exit(0);
}
