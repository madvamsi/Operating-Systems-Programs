#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
void printout(int dir)
{
    //printing directory or file
	if (dir)printf("Directory");
    else printf("File");
}
int main(int argc, char const *argv[])
{
	struct stat myStat;
	int dir = 0;
    char mod_time[20];
    struct group *grp_owner;
    struct passwd *owner;

	if (argc != 2)//checking if arguments were provided
	{
		printf("Enter a File/Folder name! \nFormat: %s<space>filename(or)foldername \nSample: %s newfolder\n",argv[0],argv[0] );
	}
	else if (stat(argv[1], &myStat) < 0) //calling stat system call
    {
	   printf(" Error! Stat Function\n");
	   return 1;
    }
    else{
    if (S_ISDIR(myStat.st_mode))//CHECKING IF GIVEN INPUT IS A DIRECTORY OR A FILE
    {
    	dir = 1;
    }
    printout(dir);
    printf(" Name             : %s\n",argv[1]);//name
    printout(dir);
    printf(" Size             : %lld bytes\n",myStat.st_size );//size
    printf("Number of Blocks Allocated : %lld\n",myStat.st_blocks );//blocks
    printf("Number of References/Links : %hu\n",myStat.st_nlink );//links
    printout(dir);
    printf(" Permissions      : ");
    printf("(%04o/",myStat.st_mode & 0777);//octal
    if (S_ISDIR(myStat.st_mode))printf("d");else printf("-");
    if (myStat.st_mode & S_IRUSR)printf("r");else printf("-");
    if (myStat.st_mode & S_IWUSR)printf("w");else printf("-");
    if (myStat.st_mode & S_IXUSR)printf("x");else printf("-");
    if (myStat.st_mode & S_IRGRP)printf("r");else printf("-");
    if (myStat.st_mode & S_IWGRP)printf("w");else printf("-");
    if (myStat.st_mode & S_IXGRP)printf("x");else printf("-");
    if (myStat.st_mode & S_IROTH)printf("r");else printf("-");
    if (myStat.st_mode & S_IWOTH)printf("w");else printf("-");
    if (myStat.st_mode & S_IXOTH)printf("x)");else printf("-)");
    
    printf("\nInode                      : %llu\n",myStat.st_ino );//inodes

    owner = getpwuid(myStat.st_uid);
    printf("Owner Name                 : %s\n",owner->pw_name);//username
    grp_owner = getgrgid(myStat.st_gid);
    printf("Group Owner Name           : %s\n",grp_owner->gr_name);//groupusername
    printf("Last Modified Time         : ");
    strftime(mod_time, 20, "%b %d %H:%M  ", localtime(&(myStat.st_mtime)));//modified 
    printf("%s\n",mod_time );
    printf("Last Status Change         : ");
    strftime(mod_time, 20, "%b %d %H:%M  ", localtime(&(myStat.st_ctime)));//change time 
    printf("%s\n",mod_time );
    printf("Last Access Time           : ");
    strftime(mod_time, 20, "%b %d %H:%M  ", localtime(&(myStat.st_atime)));//modified time 
    printf("%s\n",mod_time );
}
	return 0;
}
