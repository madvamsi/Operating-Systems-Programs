//MEHUL SURESH KUMAR
//U52982215
#include <stdio.h>//for printf()
#include <unistd.h>
#include <sys/time.h>//for gettimeofday()
#include <stdlib.h>
int main(int argc, char const *argv[])
{
	struct timeval time1,time2,time3;
	int i=0,n;
	printf("Enter the number of iterations to run the test :");
	scanf("%d",&n);
	gettimeofday(&time1, NULL);//returns the time from a reference point we use this to find out the cost of creating new processes
	while(i++<n)
	{

		//gettimeofday(&time3, NULL);//second refence point for comparison or testing
		pid_t pid = fork();//CREATE A NEW CHILD PROCESS
		if (pid == 0)
		{
			/* Child Process */
			gettimeofday(&time2, NULL);//compare these values with reference time
			printf("Creating process %lu which completes after %ld microseconds\n"
			,getpid(),time2.tv_usec-time1.tv_usec);
			if(i==n)//final average time
			{
				printf("---------------------------------------------------\nAverage time to create a process is %ld microseconds\n---------------------------------------------------",
					(time2.tv_usec-time1.tv_usec)/n );
			}
			exit(0);
		}
		if (pid < 0)//error Handling Code
		{
			printf("Could not create a new process!\n");
			return 1;
		}
	}
	return 0;
}
