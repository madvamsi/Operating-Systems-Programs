#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//GLOBAL VARIABLES
int i,gcount=0,reads=0,writes=0,num_frames=0,clean_count=0,dirty_count=0;
//STRUCT TO HOLD PAGE TABLES
typedef struct{
	unsigned addr;
	char rw;
	int dirty;	
	int ref;
	int instance;	
}page,main_page,dirty_page,clean_page;

//FUNCTION FOR UPDATING PAGE TABLE 

void update_table(int index,unsigned addr, char rw, page *page_array,int h)
	{
		if (page_array[index].dirty==1)
		{
			if (rw=='W' && h==1)//HERE WE INCREMENT THE NUMBER OF WRITES IF THE PREVIOUS... 
			{				//..ENTRY IN THE TABLE WHICH IS BEING EVICTED IS A WRITE AND...
						   //..THE CURRENT ENTRY IN THE TABLE IS NOT A HIT IN THE PAGE TABLE
			}
			else
			{
				writes=writes+1;
			}
			
		}
		page_array[index].addr = addr>>12;//SHIFT OPERATION
		page_array[index].rw = rw;
		page_array[index].ref = gcount;//COUNT TO CHECK WHICH IS THE RECENTLY USED INSTRUCTION
		if (rw=='W')
		{
			page_array[index].dirty = 1;//DIRTY BIT
		}
		else
		{
			page_array[index].dirty = 0;
		}
		gcount=gcount+1;//GLOBAL COUNTER
	}

void lru(FILE *filedata,int nf,int debug)
{
	int icount=0,hit=0,miss=0,victim=0;
	unsigned addr;
	char rw;
	page *page_array = (page*)malloc(sizeof(page)*nf);//CREATING A ARRAY OF STRUCT
	
	while(1)
	{
		int hitc = 0;
		fscanf(filedata,"%x %c",&addr,&rw);
		if ( feof(filedata) )//STOP WHEN THE END OF FILE IS REACHED
            break; 
		//printf("%x %c\n",addr,rw);
		if(icount<nf)//INITIALLY FILLING THE PAGE_TABLE 
		{

			for (i = 0; i < icount; ++i)
			{
				if(page_array[i].addr==addr>>12)
				{	
					hit=hit+1;
					hitc=1;
					update_table(i,addr,rw,page_array,1);
				}
			}
			if (hitc==0)
			{
				update_table(icount,addr,rw,page_array,0);
				miss=miss+1;	
				icount+=1;
			}
			
		}
		else//AFTER THE COLD START MISSES AND HITS ARE HANDLED HERE
		{
			for (i= 0; i < nf; ++i)//FIND IF IT IS A MATCH
			{
				if(page_array[i].addr==addr>>12)
				{
					hit=hit+1;
					hitc=1;
					update_table(i,addr,rw,page_array,1);
				}
			}
			//FIND VICTIM TO REPLACE
			if (hitc==0)
			{
				int min = 9999999;
				for (i= 0; i < nf; ++i)
				{
					if (page_array[i].ref<min)
					{
						min=page_array[i].ref;
						victim=i;
					}
				}
				update_table(victim,addr,rw,page_array,0);
				miss=miss+1;
			}
		}

	}
	fclose( filedata );//DONE WITH THE FILE
	for (i= 0; i < nf; ++i)
	{
		if (debug)
		{
			printf("%d %8x %c %d %d\n",i,page_array[i].addr,page_array[i].rw,page_array[i].ref,page_array[i].dirty );
		}
		
	}
	printf("Total memmory frames: %d\nEvents in trace: %d\nTotal disk reads: %d\nTotal disk writes: %d\n",num_frames,hit+miss,miss,writes);

}
void update_main_table(int index,unsigned addr, char rw, main_page *page_array,dirty_page *dirty_array,clean_page *clean_array,int h,int hs)//UPDATE PAGE TABLE FOR VMS 
	{
		int rep=0;

		if (page_array[index].rw=='R' && h!=1)//HERE WE CHECK IF THE PAGE BEING SWAPPED OUT IS A READ AND NOT A HIT 
		{									 //ALSO WE CHECK IF ITS ALREADY PRESENT IN THE CLEAN LIST IF ALL THE ABOVE 
			for (i= 0; i < num_frames; ++i) //CONDITIONS ARE SATISFIED THE PAGE IS ADDED TO THE CLEAN LIST
			{
				if(clean_array[i].addr==page_array[index].addr)
				{
					rep=1;
				}
			}	

				if (clean_count<num_frames && rep!=1)
				{
					clean_array[clean_count].addr=page_array[index].addr;
					clean_array[clean_count].rw=page_array[index].rw;
					clean_array[clean_count].dirty=page_array[index].dirty;
					clean_array[clean_count].ref=page_array[index].ref;
					clean_count++;
					if (clean_count==num_frames)
					{
						clean_count=0;//GO SYSTEMATICALLY FROM THE FRONT TO END OF LIST
					}
				}		

		}
		if (page_array[index].rw=='W' && h!=1)
		{
			for (i= 0; i < num_frames; ++i)
			{
				if(dirty_array[i].addr==page_array[index].addr)
				{
					rep=1;
				}
			}
			if (dirty_count<num_frames && rep!=1)
			{
					if(dirty_array[dirty_count].rw=='W' && dirty_array[dirty_count].addr!=page_array[index].addr)//INCREMENT THE NUMBER OF WRITES WHEN A PAGE 
					{																							//GETS SWAPPED OUT OF THE DIRTY LIST
					writes=writes+1;
					}
			

				dirty_array[dirty_count].addr=page_array[index].addr;//UPDATE THE DIRTY LIST
				dirty_array[dirty_count].rw=page_array[index].rw;
				dirty_array[dirty_count].dirty=page_array[index].dirty;
				dirty_array[dirty_count].ref=page_array[index].ref;
				dirty_count++;
				if (dirty_count==num_frames)
				{
					dirty_count=0;
				}
			}
		}
		// if (page_array[index].dirty==1)
		// {
		// 	if (rw=='W' && h==1)
		// 	{
				
		// 	}
		// 	else if (hs!=1)
		// 	{
		// 		writes=writes+1;
		// 	}
			
		// }
		page_array[index].addr = addr>>12;
		page_array[index].rw = rw;
		if (h!=1)
		{
			page_array[index].ref = gcount;
		}
		
		if (rw=='W')
		{
			page_array[index].dirty = 1;
		}
		else
		{
			page_array[index].dirty = 0;
		}
		page_array[index].instance = gcount;
		gcount=gcount+1;
	}
void vms(FILE *filedata,int nf,int debug)
{
	int icount=0,hit=0,miss=0,victim=0;
	unsigned addr;
	char rw;
	main_page *main_array = (main_page*)malloc(sizeof(main_page)*nf);//CREATE ARRAYS FOR MAIN PAGE CLEAN PAGE AND DIRTY PAGE
	clean_page *clean_array = (clean_page*)malloc(sizeof(clean_page)*nf);
	dirty_page *dirty_array = (dirty_page*)malloc(sizeof(dirty_page)*nf);
	while(1)
	{
		int hitc=0,hit_clean=0,hit_dirty=0;
		fscanf(filedata,"%x %c",&addr,&rw);
		if ( feof(filedata) )//RUN TILL END OF FILE
            break;

        if(icount<nf)//COMPULSARY FAULTS DUE TO COLD START
		{
			for (i= 0; i < icount; ++i)
			{
				if(main_array[i].addr==addr>>12)
				{	
					//hit=hit+1;
					hitc+=1;
					update_main_table(i,addr,rw,main_array,dirty_array,clean_array,1,0);//INITIAL HIT
					//printf("Initial Hit\n");
				}
			}
			if (hitc==0)
			{
				update_main_table(icount,addr,rw,main_array,dirty_array,clean_array,0,0);//INITIAL MISS
				//miss=miss+1;	
				//printf("Initial miss\n");
				icount+=1;
			}
			
		}
		else//AFTER PAGE TABLE IS FILLED
		{
			//search main array
			for (i= 0; i < nf; ++i)
			{
				if(main_array[i].addr==addr>>12)
				{
					//hit=hit+1;
					hitc+=1;
					update_main_table(i,addr,rw,main_array,dirty_array,clean_array,1,0);//UPDATE MAIN PAGE
					//printf("Main table hit\n");
				}
			}
			//search clean array
			if(hitc==0)
			{
				for (i= 0; i < nf; ++i)
				{
					if(clean_array[i].addr==addr>>12)
					{
						//hit=hit+1;
						hitc+=1;
						hit_clean=1;
					//printf("Clean list hit\n");
					}
				}
				if (hit_clean==1)
				{
					int min = 9999999;
					for (i= 0; i < nf; ++i)
					{
						if (main_array[i].ref<min)
						{
							min=main_array[i].ref;
							victim=i;
						}
					}
					update_main_table(victim,addr,rw,main_array,dirty_array,clean_array,0,1);//SEND PAGE FROM CLEAN TO MAIN
					
				}
			}
			//search dirty array
			if (hitc==0)
			{
				for (i= 0; i < nf; ++i)
				{
					if(dirty_array[i].addr==addr>>12)
					{
						//hit=hit+1;
						hitc+=1;
						hit_dirty=1;
						//printf("Dirty List hit\n");
					}
				}
				if (hit_dirty==1)
				{
					int min = 9999999;
					for (i= 0; i < nf; ++i)
					{
						if (main_array[i].ref<min)
						{
							min=main_array[i].ref;
							victim=i;
						}
					}

					update_main_table(victim,addr,rw,main_array,dirty_array,clean_array,0,1);//SEND PAGE FROM DIRTY TO MAIN

				}
			}	
			//find victim
			if (hitc==0)
			{
				int min = 9999999;
				for (i= 0; i < nf; ++i)
				{
					if (main_array[i].ref<min)
					{
						min=main_array[i].ref;
						victim=i;
					}
				}
				//printf("MISS for %x\n",addr );
				update_main_table(victim,addr,rw,main_array,dirty_array,clean_array,0,0);//MISS READ PAGE FROM MEMMORY TO MAIN PAGE 
				//miss=miss+1;

			}

		}
		if (hitc==0)
		{
			miss+=1;
		}
		else     //COUNTING HITS AND MISSES
		{
			hit+=1;
		}
		// if (hitc>1)
		// {
		// 	printf("ERROR !!!!!!!!!!!!!!!\n");//
		// }

	}
	fclose( filedata );
	for (i= 0; i < nf; ++i)
	{
		if (debug)
		{
			printf("%d %8x %c %d %d %d\n",i,main_array[i].addr,main_array[i].rw,main_array[i].ref,main_array[i].dirty,main_array[i].instance );
		}
		
	}
	//printf("CLEAN ARRAY\n");
	for (i= 0; i < nf; ++i)
	{
		if (debug)
		{
			
			printf("%d %8x %c %d %d %d\n",i,clean_array[i].addr,clean_array[i].rw,clean_array[i].ref,clean_array[i].dirty,clean_array[i].instance );
		}
		
	}
	//printf("DIRTY ARRAY\n");
	for (i= 0; i < nf; ++i)
	{
		if (debug)
		{
			
			printf("%d %8x %c %d %d %d\n",i,dirty_array[i].addr,dirty_array[i].rw,dirty_array[i].ref,dirty_array[i].dirty,dirty_array[i].instance );
		}
		
	}
	printf("Total memmory frames: %d\nEvents in trace: %d\nTotal disk reads: %d\nTotal disk writes: %d\n",num_frames,hit+miss,miss,writes);

	
}

int main(int argc, char const *argv[])//ACCEPT COMMAND LINE ARGUMENTS
{
	int debugmode,i=0,j=0,file_end=0,ret=0;
	unsigned addr;
	char rw;
	char const *tracename;
	FILE *filedata;
	if (argc != 5)//CHECK IF ENOUGH ARGUMENTS HAVE BEEN ENTERED 
	{
		printf("The format is memsim <tracefile> <nframes> <vms|lru> <debug|quiet>\n");
		return(-1);
		// argv[1]=filemane
		// argv[2]=num_frames
		// argv[3]=type
		// argv[4]=debug type
	}
	else if (argc == 5)
	{
		tracename = argv[1];	
		filedata = fopen( tracename, "r");
        if (filedata == NULL ) {
            printf( "Cannot open trace file %s \n", tracename);
            return( -1);
         }
		num_frames = atoi(argv[2]);
		if (num_frames < 1) 
		{
         	printf( "Number of frames must be more than 1\n");
          	return(-1);
        }

	  	
	  	if (strcmp(argv[4], "quiet\0") == 0)
            debugmode = 0;
		else if (strcmp(argv[4], "debug\0") == 0)
            debugmode = 1;
        else 
		{
			printf( "Execution mode has to be either quiet or debug\n");
			return(-1);
		}

		if (strcmp(argv[3], "lru\0") == 0)
		{
			//printf("LRU\n");
			lru(filedata,num_frames,debugmode);
		}			
	    else if (strcmp(argv[3], "vms\0") == 0)
	    {
	    	vms(filedata,num_frames,debugmode);
			//printf("VMS\n");
	    }
		else
		{
			printf( "Replacement algorithm must be either vms or lru  \n");
			return(-1);
	  	} 

	}
	
	return 0;
}