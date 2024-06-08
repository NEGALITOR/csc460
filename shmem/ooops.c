//  oooops.c  demonstrates how you can create shared memory and die --- leaving the shared memory active in RAM.
//  Bob Allen
//
//  Bad Idea - will need to learn how to clean up shared memory via bash command: ipcrm
//
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>


main(int argc, char *argv[])
{
int i, shmid, *shmem;

/*****  Get 5 Shared Memory IDs and attach  *****/

for (i=0; i<5; i++)
{
	shmid  =  shmget(IPC_PRIVATE, 2*sizeof(int), 0770);
	shmem = (int *) shmat(shmid, NULL, SHM_RND);
}

/****  Die, leaving the shared memory active.  *******/

printf("ByeByeWorld\n");
}

