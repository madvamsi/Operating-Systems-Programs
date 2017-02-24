//MEHUL SURESH KUMAR
//U52982215
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
//GLOBAL VARIABLES
float avg = 0;
int count=0,n=0;
//THREAD STARTS HERE AT ENTRY POINT
void *threadtimer (void *time1)
{
	count++;
	struct timeval time2;
	gettimeofday(&time2, NULL);//TIME OF THREAD CREATION
	struct timeval *time12 = (struct timeval *)time1;//TYPECASTING VOID TO TIME VALUE
	printf("Thread %d took %ld microseconds to be created\n\n"
			,count,time2.tv_usec-time12->tv_usec);
	avg += time2.tv_usec-time12->tv_usec;
	
	if(count == n)
	printf("Average time taken for thread creation is is %.2f microseconds\n",avg/n);//AVERAGE OF THREAD CREATION VALUES
	return NULL;
}

int main(int argc, char const *argv[])
{
	printf("Enter the number of iterations to run the test :");
	scanf("%d",&n);
	pthread_t thread1;//VARIABLE TO REFERENCE THREADS
	struct timeval time1;
	int i=0;
	while(i++<n)
	{ 
		gettimeofday(&time1, NULL);//REFERNCE TIME
		//CREATING THREAD
		if (pthread_create(&thread1,NULL,threadtimer,&time1))
	 		{
		 		printf("ERROR CREATING THREADS\n");
		 		return 1;
	 		} 
	 	//JOINING THREAD
	 	if (pthread_join(thread1,NULL))
	 		{
	 			printf("ERROR JOINING THREADS\n");
	 			return 2;
	 		}
	}
	return 0;
}