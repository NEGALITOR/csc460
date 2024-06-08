// Taha Tas
// CSC 460
// Mar 19 2024
// Crypto Club

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define PARENT 0 // 0 | being used only
#define CHILD 0 // 1

void initializeSharedMemory(int *shmid, FILE *fp);
void initializeSemaphores(int *semid, FILE *fp);
void cleanup(int shmid, int semid);
void deposit(int shmid, int semid, int amount, int N, int uniquePID);
void withdraw(int shmid, int semid, int amount, int N, int uniquePID);

int main(int argc, char *argv[]) 
{
    int shmid, semid, *coins;
    //printf("%d\n", argc);

    if (argc == 1)
    {
        FILE *fp = fopen("cryptodata", "r");
        if (fp == NULL)
        {
            fp = fopen("cryptodata", "w");
            initializeSharedMemory(&shmid, fp);
            initializeSemaphores(&semid, fp);
            printf("Initialized: 460 Coins\n");

            fprintf(fp, "%d\n%d", shmid, semid);
            fclose(fp);
        }
        else
        {
            fp = fopen("cryptodata", "r");
            fscanf(fp, "%d", &shmid);
            fscanf(fp, "%d", &semid);
            fclose(fp);

            //printf("%d\n", shmid);

            if ((coins = shmat(shmid, NULL, SHM_RND)) == (int *) -1) 
            {
                perror("shmat");
                exit(1);
            }
            printf("Current Coin Count: %d\n", *coins);

        }
        
    }
    else if (argc == 2)
    {
        FILE *fp = fopen("cryptodata", "r");
        if (fp == NULL) 
        {
            printf("System has not been initialized. Run the program with no arguments first.\n");
            exit(1);
        }
        fscanf(fp, "%d", &shmid);
        fscanf(fp, "%d", &semid);
        fclose(fp);

        if (strcmp(argv[1], "cleanup") == 0)
        {
            cleanup(shmid, semid);
            return 0;
        }
        else
        {
            int N = atoi(argv[1]);
            if (N < 1 || N > 100)
                printf("Invalid Value for N. Must be between 1-100.\n");

            int i;
            for (i = 0; i < 16; i++)
            {
                if (fork() != 0) {
                    deposit(shmid, semid, 1 << i, N, i); //deposit(1 << i, 1, i, N);
                    return 0;
                }
                else
                    withdraw(shmid, semid, 1 << i, N, i + 16); //withdraw(1 << i, 0, i + 16, N);
            }
        }
        
    }
    
    return 0;
}

void initializeSharedMemory(int *shmid, FILE *fp) 
{
    // Create shared memory
    *shmid  =  shmget(IPC_PRIVATE, 1*sizeof(int), 0777);
    if (*shmid == -1)
    {
        perror("shmget");
        exit(0);  
    }

    // Attach shared memory
    int *coins = shmat(*shmid, NULL, SHM_RND);
    if (coins == (void*) -1) 
    {
        perror("shmat");
        exit(1);
    }

    
    // Initialize shared memory to 460 coins
    *coins = 460;
}

void initializeSemaphores(int *semid, FILE *fp) 
{

    *semid = semget (IPC_PRIVATE, 1*sizeof(int), 0777);
    if (*semid == -1)
    {
      perror("semget");
      exit(1);
    }

    semctl(*semid, 0, SETVAL, 1);
}

void deposit(int shmid, int semid, int amount, int N, int uniquePID)
{
    int *coins = shmat(shmid, NULL, SHM_RND);
    if (coins == (void *) -1) 
    {
        perror("shmat failed");
        exit(1);
    }

    int i;
    for (i = 0; i < N; i++)
    {
        p(PARENT, semid);
        //p(semid);

        *coins += amount;
        printf("%d: %d + %d = %d\n", getpid(), *coins - amount, amount, *coins);
        //printf("%d | %d: %d + %d = %d\n", uniquePID, getpid(), *coins - amount, amount, *coins);
        //printf("%d: %d + %d = %d\n", uniquePID, *coins - amount, amount, *coins);

        v(PARENT, semid);
        //v(semid);
    }

    if (shmdt(coins) == -1) {
        perror("shmdt");
        exit(1);
    }
}

void withdraw(int shmid, int semid, int amount, int N, int uniquePID)
{
    int *coins = shmat(shmid, NULL, SHM_RND);
    if (coins == (void *) -1) 
    {
        perror("shmat");
        exit(1);
    }

    int i;
    for (i = 0; i < N; i++)
    {
        p(PARENT, semid);
        //p(semid);

        *coins -= amount;
        printf("\t%d: %d - %d = %d\n", getpid(), *coins + amount, amount, *coins);
        //printf("\t%d | %d: %d - %d = %d\n", uniquePID, getpid(), *coins + amount, amount, *coins);
        //printf("\t%d: %d - %d = %d\n", uniquePID, *coins + amount, amount, *coins);

        v(PARENT, semid);
        //v(semid);
    }

    if (shmdt(coins) == -1) {
        perror("shmdt");
        exit(1);
    }
}

void cleanup(int shmid, int semid) 
{

    FILE *fp = fopen("cryptodata", "r");
    if (fp == NULL) 
    {
        perror("fopen");
        exit(1);
    }

    fscanf(fp, "%d", &shmid);
    fscanf(fp, "%d", &semid);

    int *coins = shmat(shmid, NULL, SHM_RND);
    if (coins == (void *) -1) 
    {
        perror("shmat");
        exit(1);
    }

    printf("Deleting Coin Count: %d\n", *coins);

    // Detach shared memory
    if (shmdt(coins) == -1) 
    {
        perror("shmdt");
        exit(1);
    }

    // Delete shared memory
    if (shmctl(shmid, IPC_RMID, NULL) == -1) 
    {
        perror("shmctl");
        exit(1);
    }
    

    // Delete semaphore
    if (semctl(semid, 0, IPC_RMID, 0) == -1) 
    {
        perror("semctl");
        exit(1);
    }
    fclose(fp);

    if (remove("cryptodata") != 0)
    {
        perror("remove");
        exit(1);
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


/*
p(int semid)
{
   struct sembuf sops;

   sops.sem_num = 0;
   sops.sem_op = -1;
   sops.sem_flg = 0;
   if((semop(semid, &sops, 1)) == -1) printf("%s", "'P' error\n");
}

v(int semid)
{
   struct sembuf sops;

   sops.sem_num = 0;
   sops.sem_op = 1;
   sops.sem_flg = 0;
   if((semop(semid, &sops, 1)) == -1) printf("%s","'V' error\n");
}
*/