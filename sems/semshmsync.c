#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/sem.h>


#define TURN shmem[0]
#define BALANCE shmem[1]

main(int argc, char *argv[])
{
int i, j, shmid, *shmem;
int localCopy;
int LoopCount = 100;

//  Ask OS for semaphores.
int  sem_id = semget (IPC_PRIVATE, 1, 0777);


//  See if you got the request.
  if (sem_id == -1)
     {
         printf("%s","SemGet Failed.\n");
             return (-1);
                }
semctl(sem_id, 0, SETVAL, 1);
//
//
/*****  Make a note of "who" is the first Process *****/

int firstID = getpid();


/*****  Get Shared Memory and ID *****/

shmid  =  shmget(IPC_PRIVATE, 2*sizeof(int), 0770);
if (shmid == -1)
    {
    printf("Could not get shared memory.\n");
    return(0);
    }


/****   Attach to the shared memory  ******/

shmem = (int *) shmat(shmid, NULL, SHM_RND);


/****  Initialize teh shared memory ****/

BALANCE = 100;
TURN = 0;

printf("Initial Balance: %d\n\n",BALANCE);


/*************  BODY  OF  PROGRAM     ***********/

if (fork())
{                // PARENT //
   for (i = 0; i < LoopCount; i++)
   {
        //while(TURN == 1);  /** busy wait for Parent **/
        p(0,sem_id);

        localCopy = BALANCE;

        // Simulate doing some work
                for (j=0; j<100000; j++);
        localCopy++;
        BALANCE = localCopy;

        v(0,sem_id);
        //TURN = 1;
   }
}
else
{               //  CHILD  //
   for (i = 0; i < LoopCount; i++)
   {
        //while(TURN == 0);  /** busy wait for Child **/
        p(0,sem_id);

        localCopy = BALANCE;

        // Simulate doing some work
                for (j=0; j<100000; j++);
        localCopy--;
        BALANCE = localCopy;

        //TURN = 0;
        v(0,sem_id);
   }
}

/****  Let each process show what the BALANCE is now ****/

sleep(1);
printf("PID:%d sees BALANCE:%d\n",getpid(),BALANCE);
sleep(1);


/****   Detach and clean-up the shared memory  ******/

if (shmdt(shmem) == -1 ) printf("shmgm: ERROR in detaching.\n");

if (firstID == getpid())         // ONLY need one process to do this
{
if ((shmctl(shmid, IPC_RMID, NULL)) == -1)
    printf("ERROR removing shmem.\n");
if ((semctl(sem_id, 0, IPC_RMID, 0)) == -1)
    printf("%s", "Parent: ERROR in removing sem\n");


}

printf("alldone\n");
}

p(int s,int sem_id)
{
struct sembuf sops;

sops.sem_num = s;
sops.sem_op = -1;
sops.sem_flg = 0;
if((semop(sem_id, &sops, 1)) == -1) printf("%s", "'P' error\n");
}

v(int s, int sem_id)
{
struct sembuf sops;

sops.sem_num = s;
sops.sem_op = 1;
sops.sem_flg = 0;
if((semop(sem_id, &sops, 1)) == -1) printf("%s","'V' error\n");
}


