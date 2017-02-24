#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	int myFile,start,end,current,rd;
	char buffer[1];
	int num_lines=0;
	//CHECK IF INPUT IS PASSED
	if (argc != 3)
	{
		printf("Enter Number of lines and FilePath! \nFormat: %s -<number of lines> <FilePath/FileName> \nSample: %s -9 output.txt\n",argv[0],argv[0] );
	}
	//OPENING FILE
	if((myFile = open(argv[2],O_RDONLY)) < 0)
	{
		printf("Error! Unable to open File %s\n",argv[2]);
		return 1;
	}
	start = lseek(myFile,0,SEEK_SET);
	if (start!=0)
	{
		printf("ERROR! Cannot find Start of File\n");
		return(1);
	}
	end = lseek(myFile,0,SEEK_END);
	if (end < 0)
	{
		printf("ERROR! Cannot find End of File\n");
		return(1);		
	}
	while (num_lines < -atoi(argv[1]))
	{
		current = lseek(myFile,-2,SEEK_CUR);
		if (current < 0)
		{
		printf("ERROR! Cannot find Current position\n");
		return(1);		
		}
		rd = read(myFile,buffer,1);
		if(rd == -1)
		{
			printf("Error! Cannot read File");
			return 1;
		}
		if (buffer[0] == '\n')
		{
			num_lines++;
		}
		if (current == start)
		{
			//printf("Cannot Print %d lines! File has only %d lines\n",-atoi(argv[1]),num_lines+1);
			write(1,buffer,1);
			break;
		}
	}
	do
	{
		current = lseek(myFile,0,SEEK_CUR);
		if (current < 0)
		{
		printf("ERROR! Cannot find Current position\n");
		return(1);		
		}
		rd = read(myFile,buffer,1);
		if(rd == -1)
		{
			printf("Error! Cannot read File");
			return 1;
		}
		write(1,buffer,1);
	}while(current != end);
	return 0;
}