#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>

#define TURN shmem[0]

int main(int argc, char *argv[]) {

    if (argc != 2) 
    {
        printf("Usage: %s <N>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);

    if (N < 1 || N > 26) {
        printf("N must be between 1 and 26.\n");
        return 1;
    }

    int i, shmid, *shmem;
    int myID = 0;

    int firstID = getpid();


    shmid  =  shmget(IPC_PRIVATE, sizeof(int), 0770);

    if (shmid == -1)
    {
        printf("Could not get shared memory.\n");
        return 1;
    }

    shmem = (int *) shmat(shmid, NULL, SHM_RND);

    TURN = 0;

    for (i = 1; i < N; i++)
    {
        if (fork() > 0) break; // send parent on to Body
        myID++;
    }

    //printf("I'm Alive: %d - %d\n",getpid(),myID);
    sleep(1);

    for (i = 0; i < N; i++)
    {
        while(TURN != myID);  /** busy wait for Child **/

        char letter =  myID + 'A';
        printf("%c: %d\n", letter, getpid());
        TURN = (TURN + 1) % N;
    }

    if (shmdt(shmem) == -1 ) printf("shmgm: ERROR in detaching.\n");
    sleep(1);

    if (firstID == getpid())         // ONLY need one process to do this
    if ((shmctl(shmid, IPC_RMID, NULL)) == -1)
        printf("ERROR removing shmem.\n");
    
    
}