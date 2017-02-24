/* Implement your test code here. DO NOT remove function signatures below. You may add other functions as needed. */
#include "surfers.h" /* has dataT, NSURFERS */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

void surf(dataT *d) {
    /* ... */
    printf("s%d is surfing\n", d->id);
    d->state = SURFING;
    int r = rand() % 5;
    sleep(r);//WAIT FOR A RANDOM TIME BETWEEN 1 AND 5
     }

void leave(dataT *d) {
    /* ... */
    printf("s%d leaves\n", d->id);
    d->state = LEAVE;}

void getReady(dataT *d) {
    /* ... */
    printf("s%d arrives\n", d->id);
    d->state = READY;
}

void monitor(void * x) 
{
	sleep(6);	
	sem_post(&dusk);//GIVE THE SIGNAL FOR DUSK AFTER A PARTICULAR TIME
    //THIS WORKS AS A TIME OUT SIGNAL SO THAT A SURFER WHO IS THE LAST SURFER DOESN'T HAVE TO INFINETLY WAIT FOR ANOTHER SURFER
	
}

