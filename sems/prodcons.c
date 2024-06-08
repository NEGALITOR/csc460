#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/sem.h>


#define SIZE 5
#define FRONT shmem[SIZE]
#define REAR  shmem[SIZE+1]
#define MUTEX 0
#define FULL 1
#define EMPTY 2

main(int argc, char *argv[])
{
int i, j, shmid, *shmem;
int localCopy;
int LoopCount = 100000;

/*****  Make a note of "who" is the first Process *****/

int firstID = getpid();

//  Ask OS for semaphores.
int  sem_id = semget (IPC_PRIVATE, 3, 0777);

//  See if you got the request.
  if (sem_id == -1)
     {
         printf("%s","SemGet Failed.\n");
         return (-1);
     }

// Init the Sems
semctl(sem_id, MUTEX, SETVAL, 1);
semctl(sem_id, FULL, SETVAL, 0);
semctl(sem_id, EMPTY, SETVAL, SIZE);

/*****  Get Shared Memory and ID *****/

shmid  =  shmget(IPC_PRIVATE, (SIZE+2)*sizeof(int), 0770);
if (shmid == -1)
    {
    printf("Could not get shared memory.\n");
    return(0);
    }


/****   Attach to the shared memory  ******/

shmem = (int *) shmat(shmid, NULL, SHM_RND);


/****  Initialize the shared memory ****/

FRONT = 0;
REAR  = 0;


/*************  BODY  OF  PROGRAM     ***********/

if (fork())
{                // PRODUCERS - produce their pid //
	fork();
	fork();
	fork();
/////  Eight Producers

        p(EMPTY,sem_id);
        p(MUTEX,sem_id);
///  Critical Section

	shmem[REAR] = getpid();
	REAR = (REAR + 1) % SIZE;

        v(MUTEX,sem_id);
        v(FULL,sem_id);
}
else
{               //  CONSUMER - consumes (prints) values sent //

	int i;
        for (i=0;i<8;i++)   // Note: 8 matches num of Producers
	{
          p(FULL,sem_id);
          p(MUTEX,sem_id);
	  
	  //  Critical Section
	
	  printf("Consumer found: %d\n",shmem[FRONT]);
	  FRONT = (FRONT + 1) % SIZE;

          v(MUTEX,sem_id);
          v(EMPTY,sem_id);
	}

// Consumer cleans up Shm and Sems
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


