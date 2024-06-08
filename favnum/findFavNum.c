#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>


main(int argc, char *argv[])
{
int shmid;
int *shmem;
FILE *fopen(), *fp;


/*****  Open File to hold Shared Memory ID  *****/

if((fp = fopen("/pub/csc460/bb/BBID.txt","r")) == NULL)
    {
    printf("watch:  could not open file.\n");
    return(0);
    }


/*****  Get Shared Memory ID and close file  *****/

    fscanf(fp,"%d",&shmid);
    fclose(fp);

/****   Attach to the shared memory  ******/

shmem = (int *) shmat(shmid, NULL, SHM_RND);

printf("FavNum: %d\n",*shmem);
printf("\n\n");


}









