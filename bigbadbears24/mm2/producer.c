// Taha Tas
// CSC 460
// April 24, 2024
// Memory Manager 1

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define REAR bufControl[1]
#define MUTEX 0
#define FULL 1
#define EMPTY 2

struct Request
{
    int pid, size, time, semid;
};

struct Request* buffer = NULL;
int* bufControl = NULL;
int bufid = -1;
int semid = -1;
int bufSize = -1;
int ramSize = -1;
int jsemid = -1;

void writeShutdown(int pid);
void delShutdown(int pid);
void setup();
void reqJob(int size, int time, int jsemid);
bool isNum(const char* s);

int main(int argc, char const *argv[])
{
	if(jsemid != -1)
        if (semctl(jsemid, 0, IPC_RMID, 0) == -1)
            printf("%s", "Error removing job sem.\n");

    writeShutdown(getpid());
    
	FILE* fp = fopen("memory","r");
    if(fp == NULL)
    {
        printf("Failed to open memory file.\n");
        exit(1);
    }
    fscanf(fp, "%d\n%d\n%d\n%d", &semid, &bufid, &bufSize, &ramSize);
    fclose(fp);

    buffer = (struct Request *) shmat(bufid, NULL, SHM_RND);
    bufControl = (int*) (buffer+bufSize);

    int size, time;
    
	if(argc != 3)
    {
        printf("Invalid number of arguments.\n");
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

    size = atoi(argv[1]);
    if(size > ramSize)
    {
        printf("The size must not exceed RAM size.\n");
        exit(1);
    }
    if(size < 1)
    {
        printf("The size must be >0.\n");
        exit(1);
    }

    time = atoi(argv[2]);
    if(time < 1 || time > 30)
    {
        printf("The time must be between 1 and 30.\n");
        exit(1);
    }

    jsemid = makeSem();

    reqJob(size, time, jsemid);

    p(0, jsemid);

    printf("Producer %d finished it's request of %d blocks for %d seconds.\n", getpid(), size, time);

    delShutdown(getpid());
}

void writeShutdown(int pid)
{
    FILE* fp = fopen("shutdown","a+b");
    if(fp == NULL)
        printf("Failed to open shutdown file\n");
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

void delShutdown(int pid)
{
    FILE* fp = fopen("shutdown","a+b");
    if(fp == NULL)
        printf("Failed to open shutdown file\n");
    fseek(fp, 0, SEEK_SET);


    int readVal;
    while(fread(&readVal, sizeof(int), 1, fp) == 1)
    {
        if (readVal == pid)
        {
            fseek(fp, -1*sizeof(int), SEEK_CUR);
            int negativeOne = -1;
            fwrite(&negativeOne, sizeof(int), 1, fp);
            break;      
        }  
    }
    fclose(fp);
}
void setup() 
{
    
}

void reqJob(int size, int time, int jsemid)
{
    struct Request currRequest;
    currRequest.pid = getpid();
    currRequest.time = time;
    currRequest.size = size;
    currRequest.semid = jsemid;

    p(EMPTY, semid);
    p(MUTEX, semid);

    printf("Producer %d is requesting %d blocks of RAM for %d seconds.\n", getpid(), size, time);
    buffer[REAR] = currRequest;
    REAR = (REAR+1)%bufSize;

    v(MUTEX,semid);
    v(FULL,semid);
}

bool isNum(const char* s)
{
    const int n = strlen(s);
    int i;
    for(i = 0; i < n; i++)
    {
        if (!isdigit(s[i]))
        {
            return false;
        }
    }
    return true;
}

int makeSem()
{
    int jsemid = semget(IPC_PRIVATE, 1, 0700);
    if (jsemid == -1)
    {
        printf("Failed to get job sem.\n");
        exit(1);
    }

    semctl(jsemid, 0, SETVAL, 0);
    return jsemid;
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