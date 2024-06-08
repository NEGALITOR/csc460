#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>


main(int argc, char *argv[])
{
int shmid;
int *shmem;
FILE *fopen(), *fp;

/*****  Open File to hold Shared Memory ID  *****/

if((fp = fopen("/pub/csc460/sharedNum/BBID.txt","w")) == NULL)
    {
    printf("start:  could not open file.\n");
    return(0);
    }

/*****  Give everyone permission to read file.  *****/
system("chmod 777 /pub/csc460/sharedNum/BBID.txt");


/*****  Get Shared Memory and ID *****/
// 
//  Get enough shared memory so that each person in the class 
//  (plus Prof and mystery student) have
//  one 80-character line of space to write in (plus one end-of-string char)

shmid  =  shmget(IPC_PRIVATE, sizeof(int), 0777);
if (shmid == -1)
    {
    printf("Could not get shared memory.\n");
    fclose(fp);
    return(0);
    }


/*****  Write SHMID into file and close file.  *****/

fprintf(fp,"%d\n",shmid);
fclose(fp);



/****   Attach to the shared memory  ******/

shmem = (int *) shmat(shmid, NULL, SHM_RND);
shmem[0]=460;


/*****  PRINT MESSAGE AND TERMINATE  *****/

printf("\n\nShared Memory set up and ready for \"play.\"\n");
printf("shmid is %d\n\n",shmid);
printf("shmem[0] is %d\n\n",shmem[0]);
}










