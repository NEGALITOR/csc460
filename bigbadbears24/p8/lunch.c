// Taha Tas
// CSC 460
// April 1, 2024
// Cristina's Lively Diner

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

void pickupChop(int i, char* state, int chopID, int* clockMemory);
void dropChop(int i, char* state, int chopID);
void canEat(int i, char* state, int chopID);

#define LEFT (i + 4) % 5
#define RIGHT (i + 1) % 5

int main() 
{

	int i, clockID, phID, chopID, PID;

	char* state;
	int* clockMemory;

	clockID = shmget(IPC_PRIVATE, sizeof(int), 0777);
	if(clockID == -1) 
	{
		printf("Error Getting Clock Shared Memory...\n");
		return(-1);
	}
	clockMemory = shmat(clockID, NULL, SHM_RND);


	phID = shmget(IPC_PRIVATE, 5*sizeof(char)*8, 0777);
	if(phID == -1) 
	{
		printf("Error Getting Philosophers' Shared Memory...\n");
		return(-1);
	}
	state = shmat(phID, NULL, SHM_RND);


	for(i = 0; i < 5; i++) 
	{
		strncpy((state + (8*i)), "think  ", 8);
	}


	chopID = semget(IPC_PRIVATE, 6, 0777);
	if(chopID == - 1) 
	{
		printf("Error Getting Utensil Semaphores...\n");
		return(-1);
	}


	for(i = 0; i < 5; i++) 
		semctl(chopID, i, SETVAL, 0);


	semctl(chopID, 5, SETVAL, 1);


	PID = 0;
	for(i = 0; i < 5; i++) 
	{
		if(fork() != 0) break;
		PID++;
	}

	srand(getpid());

	*clockMemory = 0;

	if(PID != 5) 
	{
		while(*clockMemory < 60) 
		{

			// Think
			sleep(4 + rand()%(10 - 4 + 1));

			pickupChop(PID, state, chopID, clockMemory);

			if(*clockMemory > 60) break;

			// EAT
			sleep(rand()%2 + 1);

			if(*clockMemory > 60) break;

			dropChop(PID, state, chopID);
		}
		strncpy((state + (8*PID)), "dead  ", 8);
	}
	else 
	{
		while(1) 
		{
			printf("%d:\t", *clockMemory);
			printf("%.*s\t%.*s\t%.*s\t%.*s\t%.*s\n", 8, state, 8, state + 8, 8, state + 16, 8, state + 24, 8, state + 32);
			sleep(1);


			(*clockMemory)++;

			if((*clockMemory >= 60) &&
				(strcmp(state, "dead  ") == 0) &&
				(strcmp(state + 8, "dead  ") == 0) &&
				(strcmp(state + 16, "dead  ") == 0) &&
				(strcmp(state + 24, "dead  ") == 0) &&
				(strcmp(state + 32, "dead  ") == 0)) 
			{
				(*clockMemory)++;
				break;
			}

			if(*clockMemory >= 60 && (strcmp(state + 8*i, "dead  ") != 0)) 
			{
				(*clockMemory)++;
				strncpy((state + (8*PID)), "dead  ", 8);
				break;
			}
		}
	}

	strncpy((state + (8*PID)), "dead  ", 8);

	(*clockMemory)++;
	printf("%d:\t", *clockMemory);
	printf("%.*s\t%.*s\t%.*s\t%.*s\t%.*s\n", 8, state, 8, state + 8, 8, state + 16, 8, state + 24, 8, state + 32);


	if((strcmp(state, "dead  ") == 0) &&
		(strcmp(state+8, "dead  ") == 0) &&
		(strcmp(state+16, "dead  ") == 0) &&
		(strcmp(state+24, "dead  ") == 0) &&
		(strcmp(state+32, "dead  ") == 0)) 
	{


		if((shmctl(phID, IPC_RMID, NULL)) == -1)
			printf("Error Removing Shared Memory\n");
		else
			printf("Shared Memory Removed Successfully!\n");

		if((shmctl(clockID, IPC_RMID, NULL)) == -1)
			printf("Error Removing Shared Memory\n");
		else
			printf("Shared Clock Removed Successfully!\n");

		if((semctl(chopID, 0, IPC_RMID, 0)) == -1)
			printf("Error Removing Semaphores\n");
		else
			printf("Semaphores Removed Successfully!\n");

        printf("Finished!\n");
	}

	return 0;
}

void pickupChop(int i, char* state, int chopID, int* clockMemory) 
{

	p(5, chopID);
	strncpy((state + (8*i)), "hungry ", 8);

	if(*clockMemory > 60) 
	{
		v(5, chopID);
		return;
	}
	canEat(i, state, chopID);
	v(5, chopID);
	p(i, chopID);
}

void dropChop(int i, char* state, int chopID) 
{

	p(5, chopID);
	strncpy((state + (8*i)), "think  ", 8);
	canEat(LEFT, state, chopID);
	canEat(RIGHT, state, chopID);
	v(5, chopID);

}

void canEat(int i, char* state, int chopID) 
{

	if((strcmp((state + (8*i)), "hungry ") == 0) &&
		(strcmp((state + (8*LEFT)), "eat ") != 0) &&
		(strcmp((state + (8*RIGHT)), "eat ") != 0)) 
	{

		strncpy((state + (8*i)), "eat ", 8);
		v(i, chopID);
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