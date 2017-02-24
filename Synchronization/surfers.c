/* DO NOT remove includes */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "surfers.h"

  sem_t mutex;
  sem_t OkToSurf;
  sem_t OkToLeave;
  static volatile int num_surfing=0,ReadyToSurf=0,ReadyToLeave=0,count=0;
  //pthread_t t2[NSURFERS];
/* ... */


/* Declare synchronization variables */
/* ... */

/* Add code to surfer's thread. Surfer MUST call getReady, surf, and leave (in that order) */
void surfer(void *dptr) 
{
  count++;//COUNT INCREMENTS HERE AND IS RESET WHEN NOBODY IS CURRENTLY IN THE WATER
  if (count>2)
  {//CODE TO MAKE SURE FIRST TWO SURFERS GO TOGETHER IN THE BEGINNING
    usleep(50);//SLEEP ALL THE OTHER THREADS FOR A SHORT TIME SO THAT THE FIRST TWO THREADS HAVE PRIORITY 
  }

  dataT *d = (dataT *)dptr;
  getReady(d);
  sem_wait(&mutex);//GET THE LOCK
  ReadyToSurf++;
  
  

  if (num_surfing == 0 && ReadyToSurf == 1)//HE IS THE FIRST PERSON
  {
      //printf("%d NEEDS ONE MORE PERSON TO SURF\n", d->id);
      sem_post(&mutex);
      sem_wait(&OkToSurf);
      ReadyToSurf-=1;
      num_surfing+=1;
  }


  else if (num_surfing == 0 && ReadyToSurf == 2)//HE IS THE SECOND PERSON
  {
      //printf("%d SURF SEASON BEGINS-----------------------\n", d->id);
      sem_post(&OkToSurf);
      ReadyToSurf-=1;
      num_surfing+=1;
      sem_post(&mutex);
  }
  else//HE IS THE THIRD OR LATER SURFER
  {
    ReadyToSurf--;
    num_surfing++;
    if (num_surfing == 2 && ReadyToLeave == 1)//CASE IN WHICH YOU ARE THE THIRD SURFER AND A PREVIOUS SURFER IS WAITING FOR THE OTHER TO LEAVE WITH HIM
    {
      //printf("S%d CAN LEAVE IF YOU WANT TO NOW\n", d->id);
      sem_post(&OkToLeave);
      ReadyToLeave--;
    }
    sem_post(&mutex);
  }
  
 
  surf(d);//LETS SURF

  sem_wait(&mutex);
  num_surfing--;
  ReadyToLeave++;
  
  

  if (num_surfing == 1 && ReadyToLeave == 1)//CASE WHEN THERE ARE ONLY TWO PEOPLE IN THE WATER AND YOU ARE WAITING FOR THE OTHER GUY TO LEAVE WITH YOU
  {
    //printf("%d NEEDS TO WAIT FOR LAST GUY TO LEAVE\n", d->id);
    printf("s%d is ready to leave\n", d->id);
    sem_post(&mutex);
    sem_wait(&OkToLeave);
    ReadyToLeave -= 1;
  }
  else if (num_surfing == 0 && ReadyToLeave == 2)//CASE WHEN YOU ARE THE LAST TWO IN THE WATER SO YOU LEAVE TOGETHER
  {
    //printf("%d READY TO LEAVE-------------------------\n", d->id);
    printf("s%d is ready to leave\n", d->id);
    sem_post(&OkToLeave);
    ReadyToLeave -= 1;
    sem_post(&mutex);
    count=0;

  }
  else if (num_surfing == 0 && ReadyToSurf == 1 && ReadyToLeave == 1)//HANDLING RARE CASE WHEN YOU MAY WAIT FOR A GUY WHO HAS JUST ARRRIVED SO THAT HE DOESNT STARVE
  {
    sem_wait(&OkToLeave);
  }
  else//YOU ARE FREE TO LEAVE AS TTHERE ARE MORE THAN TWO PEOPLE IN THE WATER
  {
    printf("s%d is ready to leave\n", d->id);
    ReadyToLeave--;
    sem_post(&mutex);
  }
  //printf("No of people in the water %d Ready to surf %d Ready to leave %d\n",num_surfing,ReadyToSurf,ReadyToLeave );
  leave(d);

}

/* Add code to main (DO NOT remove initialization code) */
int main() {
  printf("The Sharks are in the water\n");
  int i=0,j=0;

  /* Initialize synchronization variables */
  if (sem_init(&dusk, 0, 0) == -1) { perror("sem_init"); } // THIS HAS CHANGED
  if (sem_init(&mutex, 0, 1) == -1) { perror("sem_init"); }
  if (sem_init(&OkToSurf, 0, 0) == -1) { perror("sem_init"); }
  if (sem_init(&OkToLeave, 0, 0) == -1) { perror("sem_init"); }

  /* Initialize thread data structures */
  pthread_t t[NSURFERS];
  dataT **ds = malloc(sizeof(dataT) * NSURFERS);
  for (j=0; j<NSURFERS; j++) { ds[j] = malloc(sizeof(struct data)); ds[j]->id = j; }


  /* s1 and s2 start surfing */ //THIS HAS CHANGED
  /*surf(ds[0]);  //THIS HAS CHANGED
    surf(ds[1]);*/ //THIS HAS CHANGED

  /* Create monitor */
  pthread_t mon;
  pthread_create(&mon, NULL, (void *)&monitor, (void *)ds);

  /* Create surfers */
  for (i = 0; i < NSURFERS; ++i)
  {
    if (pthread_create(&t[i],NULL,(void *)&surfer, (void *)ds[i]))
        {
          printf("ERROR CREATING THREADS\n");
          return 1;
        } 
  }
  sem_wait(&dusk);//HANDLE CASE OF DUSK AND CASE WHEN THERE IS STARVATION
  sem_wait(&mutex);
  printf("DUSK\n");
  for (i = 0; i < NSURFERS; ++i)
  {
    int ret = 0;
    if( (ret = pthread_kill(t[i], 0)) == 0)
      {
          printf("Surfer No: %d is forced out at DUSK\n",ds[i]->id);
          pthread_cancel(t[i]);
      }
  }
  sem_post(&mutex);  

  /* Wait for surfers to finish */
  for (i = 0; i < NSURFERS; ++i)
  {
    if (pthread_join(t[i],NULL))
      {
        printf("ERROR JOINING THREADS\n");
        return 2;
      }
  }

  /* Wait for monitor to finish */
  pthread_join(mon, NULL);

  /* Clean up synchronization variables */
  sem_destroy(&mutex);
   sem_destroy(&OkToLeave);
    sem_destroy(&OkToLeave);
     sem_destroy(&dusk);
      sem_destroy(&monitor_mutex);



  return 0;
}

