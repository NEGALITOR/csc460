#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#define BALANCE shmem[0]

main(int argc, char *argv[])
{
int i, j, shmid, *shmem;
int localCopy;
int LoopCount = 100;

/*****  Make a note of "who" is the first Process *****/

int firstID = getpid();


/*****  Get Shared Memory and ID *****/

shmid  =  shmget(IPC_PRIVATE, sizeof(int), 0777);
if (shmid == -1)
    {
    printf("Could not get shared memory.\n");
    return(0);
    }


/****   Attach to the shared memory  ******/

shmem = (int *) shmat(shmid, NULL, SHM_RND);


/****  Initialize the shared memory ****/

BALANCE = 100;

printf("Initial Balance: %d\n\n",BALANCE);

/*************  BODY  OF  PROGRAM     ***********/

if (fork())
{                // PARENT //
   for (i = 0; i < LoopCount; i++)
   {
        localCopy = BALANCE;

        // Simulate doing some work
                for (j=0; j<100000; j++);
        localCopy++;
        BALANCE = localCopy;
   }
}
else
{               //  CHILD  //
   for (i = 0; i < LoopCount; i++)
   {
        localCopy = BALANCE;

        // Simulate doing some work
                for (j=0; j<100000; j++);
        localCopy--;
        BALANCE = localCopy;
   }
}

/****  Let each process show what the BALANCE is now ****/

printf("PID:%d sees BALANCE:%d\n",getpid(),BALANCE);
sleep(1);


/****   Detach and clean-up the shared memory  ******/

if (shmdt(shmem) == -1 ) printf("shmgm: ERROR in detaching.\n");

if (firstID == getpid())         // ONLY need one process to do this
if ((shmctl(shmid, IPC_RMID, NULL)) == -1)
    printf("ERROR removing shmem.\n");

printf("alldone\n");
}

