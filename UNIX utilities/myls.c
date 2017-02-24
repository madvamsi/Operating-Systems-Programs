#include <dirent.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

int main(int argc, char const *argv[])
{
  
  struct dirent **d;
  char const *dir;
  char myName[256],myName2[256];
  int flag = 0,sum = 0, n;

  if (argc == 3)
  {
    dir = argv[2];
  }
  else if (argc == 2 && (strncmp(argv[1],"-l",2))!=0)
  {
    dir = argv[1];
    flag = 1;
  }
  else
  {
    dir = ".";
    if (argc == 1)
    {
      flag = 1;
    }
    
  }
  if (flag == 1)
  {
    n = scandir(dir,&d,0, alphasort);
    if(n<0)
    { 
      printf("Error! in opening Directory");
      return 1;
    }
    else 
    {
      int i;
      for (i = 0; i < n; ++i)
      {
        if ((strncmp(d[i]->d_name,".",1))!=0)
          {
            printf("%-16s", d[i]->d_name);
            if (i==(n/2)+1)
            {
              printf("\n");
            }
            free(d[i]);
          }
      }
      free(d);
      printf("\n");
    }
  }

if (flag == 0)
{
  if ((strncmp(argv[1],"-l",2))==0)
  {
//FIND TOTAL SIZE OF DIRECTORY
    
            struct dirent **d2;
            int n;
             n = scandir(dir,&d2,0, alphasort);
            if(n<0)
            { 
              printf("Error! in opening Directory");
              return 1;
            }
            else
            {
             int i=0;
             for(i=0; i<n ;++i)
                {
                  if(d2[i] == NULL)
                  {
                    printf("Error! While reading Directory");
                    return 1;
                  }
                  struct stat myStat2;
                  strcpy(myName2,dir);
                  strcat(myName2,"/");
                  strcat(myName2,d2[i]->d_name);
                  if (stat(myName2, &myStat2) < 0) 
                  {
                   printf(" Error! Stat Function\n");
                   return 1;
                  }
                  if ((strncmp(d2[i]->d_name,".",1))!=0)
                  {
                    sum += myStat2.st_blocks;
                  }
                  free(d2[i]);
                  
                }
                 printf("total %d\n",sum/2);
              free(d2);
            }

    //PRINT DETAILS FOR EACH INDIVIDUAL FILE/FOLDER
    n = scandir(dir,&d,0, alphasort);
    if(n < 0)
    { 
      printf("Error! in opening Directory");
      return 1;
    }
    else
    {
      int i;
      for (i = 0; i < n; ++i)
        {
          struct stat myStat;
          struct group *grp_owner;
          struct passwd *owner;
          char mod_time[20];
          strcpy(myName,dir);
          strcat(myName,"/");
          strcat(myName,d[i]->d_name);
          if (stat(myName, &myStat) < 0) 
          {
           printf(" Error! Stat Function\n");
           return 1;
          }
          if ((strncmp(d[i]->d_name,".",1))!=0)
          {
            if (S_ISDIR(myStat.st_mode))printf("d");else printf("-");
            if (myStat.st_mode & S_IRUSR)printf("r");else printf("-");
            if (myStat.st_mode & S_IWUSR)printf("w");else printf("-");
            if (myStat.st_mode & S_IXUSR)printf("x");else printf("-");
            if (myStat.st_mode & S_IRGRP)printf("r");else printf("-");
            if (myStat.st_mode & S_IWGRP)printf("w");else printf("-");
            if (myStat.st_mode & S_IXGRP)printf("x");else printf("-");
            if (myStat.st_mode & S_IROTH)printf("r");else printf("-");
            if (myStat.st_mode & S_IWOTH)printf("w");else printf("-");
            if (myStat.st_mode & S_IXOTH)printf("x");else printf("-");
            printf("%3hu",myStat.st_nlink );
            owner = getpwuid(myStat.st_uid);
            printf("%10s",owner->pw_name);
            grp_owner = getgrgid(myStat.st_gid);
            printf("%10s",grp_owner->gr_name);
            printf("%10lld",myStat.st_size );
            strftime(mod_time, 20, "%b %d %H:%M  ", localtime(&(myStat.st_mtime))); 
            printf("%20s",mod_time );
            printf("%-16s\n", d[i]->d_name);
            free(d[i]);
          }

        }
        free(d);
       
      
    }
    
  }
}

  return 0;
}