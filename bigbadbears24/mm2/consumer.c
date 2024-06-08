// Taha Tas
// CSC 460
// April 24, 2024
// Memory Manager 1

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/queue.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define MUTEX 0
#define FULL 1
#define EMPTY 2
#define MAXJOBS 100

struct Request
{
    int pid, size, duration, semid;
};

struct Job
{
    char id;
    int pid, size, durationRemaining, semid, location;
};

struct PCB
{
    int jobCount;
    int ramSize;
    struct Job jobs[MAXJOBS];
};

struct BufData
{
    struct Request* buffer;
    int* front;
    int* rear;
    int bufferid;
    int semid;
};

void writeShutdown(int pid);
void parseArgs(int argc, char const *argv[], int* rowSize, int* colSize, int* bufferSize);
void spawnBufferConsumer(int bufferSize, int ramSize, int pcbid);
void manageMemory(int* pcbid, int rowSize, int colSize);

int main(int argc, char const *argv[])
{   
    int rowSize, colSize, bufferSize;
    parseArgs(argc, argv, &rowSize, &colSize, &bufferSize);
    int ramSize = rowSize*colSize;

    writeShutdown(getpid());

    static int pcbid;
    pcbid = setupProcessControlBlock(ramSize);

    spawnBufferConsumer(bufferSize, ramSize, pcbid);
    manageMemory(&pcbid, rowSize, colSize);
    
    return 0;
}

void writeShutdown(int pid)
{
    FILE* fp = fopen("shutdown","a+b");
    if(fp == NULL)
        printf("Failed to open shutdown file.\n");


    fseek(fp, 0, SEEK_SET);

    int readVal;
    while(fread(&readVal, sizeof(int), 1, fp) == 1)
    {
        if (readVal < 0)
        {
            fseek(fp, -1*sizeof(int), SEEK_CUR);
            break;      
        }  
    }

    fwrite(&pid, sizeof(int), 1, fp);

    fclose(fp);
}

char nextID()
{
    static int count = 0;
    char id = 'A' + count;
	
    count = (count + 1) % 26;

    return id;
}

bool isNum(const char* ch)
{
    const int n = strlen(ch);
    int i;
    for(i = 0; i < n; i++)
    {
        if (!isdigit(ch[i]))
            return false;
    }
    return true;
}


void cleanupBuffer(int status, void* args)
{
    int* args_array = (int*)args;
    int semid = args_array[0];
    int bufferid = args_array[1];
    if(semid != -1)
        if (semctl(semid, 0, IPC_RMID, 0) == -1)
            printf("Failed to remove sem.\n");

    if(bufferid != -1)
        if (shmctl(bufferid, IPC_RMID, NULL) == -1)
            printf("Failed to remove buffer shmem.\n");

    if(access("memory", F_OK) == 0)        
        if(remove("memory") != 0)
            printf("Failed to remove memory.\n");
}

void parseArgs(int argc, char const *argv[], int* rowSize, int* colSize, int* bufferSize)
{
    if(argc != 4)
    {
        printf("Invalid Arguments.\n");
        exit(1);
    }

    int i;
    for(i = 1; i < argc; i++)
    {
        if(!isNum(argv[i]))
        {
            printf("%s is not an integer.\n", argv[i]);
            exit(1);
        }
    }

    *rowSize = atoi(argv[1]);
    if(*rowSize < 1 || *rowSize > 20)
    {
        printf("The row size must be between 1 and 20.\n");
        exit(1);
    }

    *colSize = atoi(argv[2]);
    if(*colSize < 1 || *colSize > 50)
    {
        printf("The column size must be between 1 and 50.\n");
        exit(1);
    }
    
    *bufferSize = atoi(argv[3]);
    if(*bufferSize < 1 || *bufferSize > 26)
    {
        printf("The buffer size must be between 1 and 26. \n");
        exit(1);
    }
}

struct BufData setupBuffer(int bufferSize, int ramSize) 
{
    const int semid = semget(IPC_PRIVATE, 3, 0700);
    if (semid == -1)
    {
        printf("Failed to get sem.\n");
        exit(1);
    }

    semctl(semid, MUTEX, SETVAL, 1);
    semctl(semid, FULL, SETVAL, 0);
    semctl(semid, EMPTY, SETVAL, bufferSize);

    const int bufferid = shmget(IPC_PRIVATE, bufferSize*sizeof(struct Request) + 2*sizeof(int), 0700);
    if (bufferid == -1)
    {
        printf("Failed to get buffer shmem\n");
        exit(1);
    }
    static int args[2];
    args[0] = semid;
    args[1] = bufferid;
    on_exit(cleanupBuffer, args);

    struct Request* buffer = (struct Request *) shmat(bufferid, NULL, SHM_RND);
    int* front = (int*) (buffer+bufferSize);
    int* rear = front+1;

    *front = 0;
    *rear = 0;

    FILE* fp = fopen("memory","w");
    if(fp == NULL)
    {
        printf("Failed to open memory file.\n");
    }
    fprintf(fp, "%d\n%d\n%d\n%d", semid, bufferid, bufferSize, ramSize);
    fclose(fp);

    struct BufData result = {buffer, front, rear, bufferid, semid};
    return result;
}

bool fit(int jobSize, int location, char* RAM, int ramSize)
{
    int i;
    for(i = 0; i < jobSize; i++)
    {   
        if(location+i >= ramSize)
        {
            return false;
        }
        if(RAM[location+i] != '.')
        {
            return false;
        }
    }
    return true;
}

bool placeIntoRAM(struct Job* job, char* RAM, int ramSize)
{
    int i;
    for(i = 0; i < ramSize; i++)
    {
        if(fit(job->size, i, RAM, ramSize))
        {
            int j;
            for(j = 0; j < job->size; j++)
                RAM[i+j] = job->id;

            job->location = i;
            return true;
        }
    }
    return false;
}

void addJob(struct PCB* pcb, struct Job job)
{
    if(pcb->jobCount >= MAXJOBS)
    {
        printf("The Process Control Block has been filled.\n");
        system("./shutdown");
        exit(1);
    }

    static int rear = 0;
    int i;
    for(i = 0; i < MAXJOBS; i++)
    {   
        if(pcb->jobs[rear].pid < 0)
        {
            pcb->jobs[rear] = job;
            break;
        }
        rear = (rear+1)%MAXJOBS;
    }
    pcb->jobCount++;
}

void consumeBuffer(int bufferSize, struct BufData data, struct PCB* pcb)
{ 
    while(true)
    {
        p(FULL, data.semid);
        p(MUTEX, data.semid);

        struct Request request = data.buffer[*(data.front)];
        struct Job job;
        job.durationRemaining = request.duration;
        job.pid = request.pid;
        job.semid = request.semid;
        job.size = request.size;
        job.location = -1;
        job.id = nextID();
        addJob(pcb, job);
        *(data.front) = (*(data.front)+1)%bufferSize;

        v(MUTEX, data.semid);
        v(EMPTY, data.semid);
    }
}

void spawnBufferConsumer(int bufferSize, int ramSize, int pcbid)
{
    if(fork() == 0)
    {   
        writeShutdown(getpid());
        struct BufData data = setupBuffer(bufferSize, ramSize);
        struct PCB* pcb = (struct PCB *) shmat(pcbid, NULL, SHM_RND);
        consumeBuffer(bufferSize, data, pcb);
        exit(1);
    }
}

void cleanupProcessControlBlock(int status, void* args)
{
    int id = *((int*)args);

    if (shmctl(id, IPC_RMID, NULL) == -1)
        printf("Failed to remove jobs shmem.\n");
}

int setupProcessControlBlock(int ramSize)
{   
    static int id;
    id = shmget(IPC_PRIVATE, sizeof(struct PCB), 0700);
    if (id == -1)
    {
        printf("Failed to get Process Control Block shmem.\n");
        exit(1);
    }

    struct PCB* pcb = (struct PCB *) shmat(id, NULL, SHM_RND);
    pcb->jobCount = 0;
    pcb->ramSize = ramSize;


    int i;
    for(i = 0; i < MAXJOBS; i++)
    {
        pcb->jobs[i].pid = -1;
    }

    if (shmdt(pcb) == -1) 
        printf("Failed to remove pcb shmem.\n");

    return id;
}

void display(struct PCB* pcb, char* RAM, int rowSize, int colSize)
{   
    printf("\n");
    
	printf("%-2s %-6s %-4s %s\n","ID", "PID", "Size", "Sec");
	printf("------------------------\n");
	
    int i;
    for(i = 0; i < MAXJOBS; i++)
    {
        struct Job* job = &(pcb->jobs[i]);
        if(job->pid < 0) 
            continue;
        printf("%-2c %-6d %-4d %d\n", job->id, job->pid, job->size, job->durationRemaining);
    }

	printf("\n");
	printf("Bob’s   Memory   Manager\n");
    printf("------------------------\n");

    for(i = 0; i < rowSize; i++)
    {
        int j;
        for(j = 0; j < colSize; j++)
        {   
            putchar(RAM[i*colSize+j]);
        }
        printf("\n");
    }

    printf("\n\n");
}

void decrementDurations(struct PCB* pcb)
{
    int i;
    for(i = 0; i < MAXJOBS; i++)
    {
        struct Job* job = &(pcb->jobs[i]);
        if(job->pid < 0)
            continue;
        if(job->location < 0)
            continue;
        job->durationRemaining--;
    }
}

void removeJobs(struct PCB* pcb, char* RAM)
{
    int i;
    for(i = 0; i < MAXJOBS; i++)
    {
        struct Job* job = &(pcb->jobs[i]);
        if(job->durationRemaining <= 0 && job->pid >= 0) 
        {
            job->pid = -1;
            pcb->jobCount--;

            int j;
            for(j = 0; j < job->size; j++)
                RAM[j+job->location] = '.';

            v(0, job->semid);
        }
    }
}

void placeJobsIntoRAM(struct PCB* pcb, char* RAM)
{   
    static int front = 0;
    const int start = front;
    int i;
    for(i = 0; i < MAXJOBS; i++)
    {
        struct Job* job = &(pcb->jobs[(start+i)%MAXJOBS]);
        if(job->pid < 0)
            continue;
        if(job->location >= 0)
            continue;
        if(placeIntoRAM(job, RAM, pcb->ramSize) && i == front)
            front=(front+1)%MAXJOBS;
    }
}

void manageMemory(int* pcbid, int rowSize, int colSize)
{
    struct PCB* pcb = (struct PCB*) shmat(*pcbid, NULL, SHM_RND);
    on_exit(cleanupProcessControlBlock, pcbid);

    char RAM[pcb->ramSize];
    int i;
    for(i = 0; i < pcb->ramSize; i++)
        RAM[i] = '.';

    while(true)
    {
        placeJobsIntoRAM(pcb, RAM);

        display(pcb, RAM, rowSize, colSize);

        sleep(1);

        decrementDurations(pcb);

        removeJobs(pcb, RAM);
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