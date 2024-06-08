#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>

main(int argc, char *argv[])
{
int shmid;
FILE *fopen(), *fp;


/*****  Open File to hold Shared Memory ID  *****/

if((fp = fopen("/pub/csc460/bb/BBID.txt","r")) == NULL)
    {
    printf("bb:  could not open file.\n");
    return(0);
    }


/*****  Get Shared Memory ID and close file  *****/

    fscanf(fp,"%d",&shmid);
    fclose(fp);


/****   Clean-up the shared memory  ******/

if ((shmctl(shmid, IPC_RMID, NULL)) == -1)
    printf("ERROR removing shmem.\n");


/****   Cleanup Everything and go home...  ****/

system("rm /pub/csc460/bb/BBID.txt");
}



