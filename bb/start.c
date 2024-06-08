#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>

struct bbStruct
{
	int id;
	char name[20];
	int favNum;
	char favFood[30];
};

main(int argc, char *argv[])
{
int shmid, i;
struct bbStruct *shmem;
FILE *fopen(), *fp;

/*****  Open File to hold Shared Memory ID  *****/

if((fp = fopen("/pub/csc460/bb/BBID.txt","w")) == NULL)
    {
    printf("start:  could not open file.\n");
    return(0);
    }

/*****  Give everyone permission to read file.  *****/
system("chmod 777 /pub/csc460/bb/BBID.txt");


/*****  Get Shared Memory and ID *****/
// 
//  Get enough shared memory so that each person in the class 
//  (plus Prof and mystery student) have
//  one 80-character line of space to write in (plus one end-of-string char)

shmid  =  shmget(IPC_PRIVATE, 22*sizeof(struct bbStruct), 0777);
if (shmid == -1)
    {
    printf("Could not get shared memory.\n");
    fclose(fp);
    return(0);
    }


/*****  Write SHMID into file and close file.  *****/

fprintf(fp,"%d\n",shmid);
fclose(fp);



/****   Attach to the shared memory  ******/

shmem = (struct bbStruct *) shmat(shmid, NULL, SHM_RND);


/***** Initialize the Shared Memory to identify each student's row.  *****/

char *who[] = {"Allen, Bob","Anderson, Cleveland", "Cargin, Erin", "Gilliam, Charlie", "Hynds, Joshua", "Imlay, Gabe", "Jackson, Joseph", "Jackson, Penny", "Johnson, Maxwell", "Koenig, McGregor", "Kotze, Dirkie", "Mykytyn, Stefan", "Novak, Cooper", "Ong, Quyen", "Pangilinan, Ervin", "Patel, Manav", "Patel, Raj", "Rivers, Seiler", "Rodriguez, Kevin", "Tas, Taha", "Truong, Bryan", "Wilcox, Willem"};


for (i=0;i<22;i++)
   {
     shmem->id = i+1;
     strcpy(shmem->name,who[i]);
     shmem->favNum = 0;
     strcpy(shmem->favFood,"");
     shmem++;
   }

/*****  PRINT MESSAGE AND TERMINATE  *****/

printf("\n\nShared Memory set up and ready for \"play.\"\n");
printf("shared-BB-ID is %d\n\n",shmid);
}










