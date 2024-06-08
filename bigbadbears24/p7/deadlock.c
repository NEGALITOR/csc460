// Taha Tas
// CSC 460
// Mar 25 2024
// Bob's Deadly Diner

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define LEFT (id + 5 - 1) % 5
#define RIGHT (id + 1) % 5

int main()
{
    int i, j, cpu, id = 0;

    int semid = semget(IPC_PRIVATE, 5, 0777);
    if (semid == -1)
	{
		printf("Failed to acquire chopstick sems.\n");
		return 1;
	}

    for (i = 0; i < 4; i++)
    {
        if (fork() == 0)
            id++;
        else 
            break;
    }

    semctl(semid, id, SETVAL, 1);

    while (1)
    {
        // Thinking
        for (i = 0; i < id; i++) printf("\t");
		printf("%d THINKING\n", id);

        // Eat up CPU cycles
		for (i = 0; i < 100; i++)
			for (j = 0; j < i; j++)
				cpu = i * j / 10;
        
        // Hungry
		for (i = 0; i < id; i++) printf("\t");
		printf("%d HUNGRY\n", id);
        
		p(LEFT, semid);
        
		for (i = 0; i < 100; i++)
			for (j = 0; j < i; j++)
				cpu = i * j / 10;
        
		p(RIGHT, semid);

        // Eating
		for (i = 0; i < id; i++) printf("\t");
		printf("%d EATING\n", id);

		for (i = 0; i < 100; i++)
			for (j = 0; j < i; j++)
				cpu = i * j / 10;
        
		v(LEFT, semid);
		v(RIGHT, semid);
    }
}

p(int s, int semid)
{
   struct sembuf sops;

   sops.sem_num = s;
   sops.sem_op = -1;
   sops.sem_flg = 0;
   if((semop(semid, &sops, 1)) == -1) printf("%s", "'P' error\n");
}

v(int s, int semid)
{
   struct sembuf sops;

   sops.sem_num = s;
   sops.sem_op = 1;
   sops.sem_flg = 0;
   if((semop(semid, &sops, 1)) == -1) printf("%s","'V' error\n");
}