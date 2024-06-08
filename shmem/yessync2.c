#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#define TURN shmem[0]

main(int argc, char *argv[])
{
int i, shmid, *shmem;
int N;                  // Holds the number of procs to generate
int myID = 0;           // used to identify procs in sync
int LoopCount = 10;     // Times each proc will "speak"

/*****  Get the command line argument for how many procs. Default of 5 ****/
if (argc < 2)
        N = 5;
else
        N = atoi(argv[1]);      //  Not error testing here :(


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


/****  Initialize teh shared memory ****/

TURN = 0;


/*************  Spawn all the Processes *********/

for (i = 1; i < N; i++)
{
  if (fork() > 0) break; // send parent on to Body
  myID++;
}
printf("I'm Alive: %d - %d\n",getpid(),myID);
sleep(1);


/*************  BODY  OF  PROGRAM     ***********/

   for (i = 0; i < LoopCount; i++)
   {
        while(TURN != myID);  /** busy wait for Child **/

        printf("%d's Turn!!\n",myID);
        TURN = (TURN + 1) % N;
   }


/****   Detach and clean-up the shared memory  ******/

if (shmdt(shmem) == -1 ) printf("shmgm: ERROR in detaching.\n");

sleep(1);

if (firstID == getpid())         // ONLY need one process to do this
if ((shmctl(shmid, IPC_RMID, NULL)) == -1)
    printf("ERROR removing shmem.\n");
printf("alldone\n");
}

