#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>

#define CHILD 0
#define PARENT 1

int main()
{
int     loopCount = 100;
int	shmid;
int	*syncVal;
int 	i;

//  Ask OS for Shared Memory ------- Buffer to hold our DATA
shmid  =  shmget(IPC_PRIVATE, sizeof(int), 0777);
if (shmid == -1)
    {
    printf("Could not get shared memory.\n");
    return(0);
    }

// Attach to the shared memory.
syncVal = (int *) shmat(shmid, NULL, SHM_RND);
*syncVal = PARENT;	// Let the parent go first!!!!!! ;)

if (fork() == 0)
   {
    for (i=0; i< loopCount; i++)
       {
         while (*syncVal != CHILD);	// busy wait
         printf("\t\t%s %d\n","Child:  ", i);
         *syncVal = PARENT;
       }
   }
else
   {
    for (i=0; i< loopCount; i++)
       {
         while (*syncVal != PARENT);	// busy wait
         printf("%s %d\n","Parent: ", i);
         *syncVal = CHILD;
       }

    // Parent needs to clean up semaphores.
    sleep(2);
    if ((shmctl(shmid, IPC_RMID, NULL)) == -1)
       printf("Parent: ERROR removing shmem.\n");
   }
}
