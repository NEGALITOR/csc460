#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#define TURN shmem[0]

main(int argc, char *argv[])
{
int i, shmid, *shmem;

/*****  Make a note of "who" is the first Process *****/

int firstID = getpid();


/*****  Get Shared Memory and ID *****/

shmid  =  shmget(IPC_PRIVATE, sizeof(int), 0770);
if (shmid == -1)
    {
    printf("Could not get shared memory.\n");
    return(0);
    }


/****   Attach to the shared memory  ******/

shmem = (int *) shmat(shmid, NULL, SHM_RND);


/****  Initialize the shared memory ****/

TURN = 0;


/*************  BODY  OF  PROGRAM     ***********/

if (fork())
{                // PARENT //
   for (i = 0; i < 100; i++)
   {
        while(TURN == 1);  /** busy wait for Child **/

        printf("%d: PARENT's Turn!!\n",i);
        TURN = 1;
   }
}
else
{               //  CHILD  //
   for (i = 0; i < 100; i++)
   {
        while(TURN == 0);  /** busy wait for Parent **/

        printf("\t\t%d: CHILD's Turn!!\n",i);
        TURN = 0;
   }
}

sleep(1);

/****   Detach and clean-up the shared memory  ******/

if (shmdt(shmem) == -1 ) printf("shmgm: ERROR in detaching.\n");

if (firstID == getpid())         // ONLY need one process to do this
if ((shmctl(shmid, IPC_RMID, NULL)) == -1)
    printf("ERROR removing shmem.\n");

printf("alldone\n");
}

