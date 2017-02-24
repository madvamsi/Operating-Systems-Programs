#include <dirent.h> 
#include <stdio.h> 
#include <string.h>
#include <sys/stat.h>

int draw_tree(char const *dir, int depth)
{

	char myName[256];
	DIR *myDirectory;
  	struct dirent *d;
  	myDirectory = opendir(dir);
  	if(myDirectory == NULL)
	{	
		printf("[error opening dir]");
		return 1;
	}
    if (myDirectory)
    {
      while ((d = readdir(myDirectory)) != NULL)
        {
	        if(d == NULL)
			{
				printf("[error reading dir]");
				return 1;
			}
          if ((strncmp(d->d_name,".",1))!=0)
          {
          	int i;
          	for (i = 0; i < depth; ++i)
          	{
          		if (i<depth-1) printf("│    ");
          		else if(i == 0) printf("├⎯⎯");
          		else printf("└─⎯⎯");
          	}
            printf("%s\n", d->d_name);
            struct stat myStat;
            strcpy(myName,dir);
			strcat(myName,"/");
			strcat(myName,d->d_name);
			if (stat(myName, &myStat) < 0) 
			    {
				   printf(" Error! Stat Function\n");
				   return 1;
			    }
			    if (S_ISDIR(myStat.st_mode))//CHECKING IF GIVEN INPUT IS A DIRECTORY OR A FILE
			    {
			    	draw_tree(myName,depth+1);
			    }
          }
          
        }
      closedir(myDirectory);
    }
    return 1;
}

int main(int argc, char const *argv[])
{
	char const *dir;
	if (argc == 2) dir = argv[1];
	else dir = ".";
	printf("%s\n",dir);
	if (draw_tree(dir,1));
	else printf("FAil\n");


	return 0;
}