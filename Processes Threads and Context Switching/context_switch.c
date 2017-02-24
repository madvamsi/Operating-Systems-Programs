//MEHUL SURESH KUMAR
//U52982215
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sched.h>
#define BUFF 256
int main()
{
    //user input
    int n=0;
    float avg1=0,avg2=0;
    printf("Enter the number of iterations to run the test\n");
    scanf("%d",&n);
    struct timeval time1,time2,time3,time4;
    
    //code to make sure program runs on a single processor
    int set_aff;
    cpu_set_t cs;     
    struct timespec begin, end;
    pid_t pid = getpid();
    CPU_ZERO(&cs);
    CPU_SET(4,&cs);//we select processor 4
    set_aff = sched_setaffinity(pid, sizeof(cs),&cs);//setting affinity


    int p1fd[2],p2fd[2];//Pipe File Descriptors
    ssize_t numRead = -1;
    const char* message1 = "Process 1 says 'Hi!'\n";//messages to be passed
    const char* message2 = "Process 2 says 'Hello!'\n";
    const unsigned int mess_size1 = strlen(message1) + 1;
    const unsigned int mess_size2 = strlen(message2) + 1;
    char reply1[BUFF];//replies
    char reply2[BUFF];
    
//CREATING PIPES
    if(pipe(p1fd) == -1)
        {
            printf("Error creating pipe 1!\n");
            _exit(1);
        }
    if(pipe(p2fd) == -1)
        {
            printf("Error creating pipe 2!\n");
            _exit(1);
        }
    printf("Two pipes have been created\n");

//CREATING PROCESS 1

    pid_t pid1 = fork();

    if (pid1 == -1)//ERROR HANDLING
        {
            printf("Process 1 failed to be created!\n");
            _exit(1);
        }
    else if (pid1 == 0)//SUCCESSFULL CASE
        {
            //printf("Process 1 has been created! with PID : %ld \n", getpid());
            int i = 0;
            if (close(p2fd[1]) == -1)
                {
                    printf("Error while closing Pipe 2 - Write\n");
                    _exit(1);
                }//closing pipe 2 write
            if (close(p1fd[0]) == -1)
                {
                    printf("Error while closing Pipe 1 - Read\n");
                    _exit(1);
                }//closing pipe 1 read
            
                while(i++<n)
                {
                    //printf("Process 1 PID:%ld runs now \n",getpid());
                    if (write(p1fd[1], message1, mess_size1) != mess_size1)
                        {
                            printf("Error while writing to pipe 1.\n");
                            _exit(1);
                        }                   //writing to pipe1
                    //printf("Message Sent by Process 1\nProcess 1 waits for Process 2 to write\n");00
                    gettimeofday(&time1, NULL);//process 1 waits he0r0.  e
                    if (read(p2fd[0], reply2, mess_size2) == -1) 
                        {
                            printf("Error while reading from pipe 2.\n");
                            _exit(1);
                        }//reading from pipe2
                    gettimeofday(&time2, NULL);
                    printf("Process 2 ran for %ld ms\n", time2.tv_usec-time1.tv_usec);
                    avg1+=time2.tv_usec-time1.tv_usec;
                    printf("Message Received by Process 1: %s------------ONE CYCLE COMPLETE-----------\n", reply2); 
                }
            if (close(p1fd[1]) == -1)
                {
                    printf("Error while closing Pipe 1 - Write\n");
                    _exit(1);
                }
            if (close(p2fd[0]) == -1)
                {
                    printf("Error while closing Pipe 2 - Read\n");
                    _exit(1);
                }//closing pipe1 write
            printf("Average Context Switching time of Process 1 is :%.2f microseconds<----------\n",avg1/n);
            exit(0);
        }
    
    //CREATING PROCESS 2

    pid_t pid2 = fork();

    if (pid2 == -1)
        {
            printf("Process 2 failed to be created!\n");
                _exit(1);
        }
    else if (pid2 == 0)
        {
            //printf("Process 2 has been created! with PID : %ld \n", getpid());
            int i = 0;
            if (close(p1fd[1]) == -1)
                {
                    printf("Error while closing Pipe 1 - Write\n");
                    _exit(1);
                }//closing pipe1 write
            if (close(p2fd[0]) == -1)
                {
                    printf("Error while closing Pipe 2 - Read\n");
                    _exit(1);
                }//closing pipe2 read
            while(i++<n)
                {
                    //printf("Process 2 waits for Process 1 to write\n");

                    gettimeofday(&time3, NULL);
                    if (read(p1fd[0], reply1, mess_size1) == -1)
                        {
                            printf("Error while reading from pipe 1.\n");
                            _exit(1);
                        }//reading from pipe 1
                    gettimeofday(&time4, NULL);
                    printf("Process 1 ran for %ld ms\n", time4.tv_usec-time3.tv_usec);
                    avg2+=time4.tv_usec-time3.tv_usec;
                    //printf("Process 2 PID:%ld runs now \n",getpid());
                    printf("Message Received by Process 2: %s", reply1);
                    if (write(p2fd[1], message2, mess_size2) != mess_size2)
                        {
                            printf("Error while writing to pipe 2.\n");
                            _exit(1);
                        }//writing to pipe2
                    //printf("Message Sent by Process 2\n");
                }
            if (close(p1fd[0]) == -1)
                {
                    printf("Error while closing Pipe 1 - Read\n");
                    _exit(1);
                }//closing pipe1 read
            if (close(p2fd[1]) == -1)
                {
                    printf("Error while closing Pipe 2 - Write\n");
                    _exit(1);
                }//closing pipe2 write
            printf("Average Context Switching time of Process 2 is :%.2f microseconds<----------\n",avg2/n);
            exit(0);
        }

//CLOSING ALL PIPES BY PARENT

    if (close(p1fd[0]) == -1)
            {
                printf("Error while closing Pipe 1 - Read\n");
                _exit(1);
            }
    if (close(p1fd[1]) == -1)
            {
                printf("Error while closing Pipe 1 - Write\n");
                _exit(1);
            }
    if (close(p2fd[0]) == -1)
            {
                printf("Error while closing Pipe 2 - Read\n");
                _exit(1);
            }
    if (close(p2fd[1]) == -1)
            {
                printf("Error while closing Pipe 2 - Write\n");
                _exit(1);
            };

  //MAKING PARENT WAIT FOR ALL CHILD PROCESSES TO FINISH
    int i=0;
    for (i = 2; i > 0; i--)
        {
            wait(NULL);
            printf("FINISHED PROCESS %d\n",i);
        }
    return 0;
}
