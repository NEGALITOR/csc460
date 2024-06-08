// Taha Tas
// CSC 460
// March 1, 2024
// Shared Grocery List


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

  if (argc < 2)
  {
    printf("Invalid Arguments.\n");
    return 0;
  }


  int shmid, i;
  struct bbStruct *shmem;
  FILE *fopen(), *fp;

  /*****  Open File to hold Shared Memory ID  *****/

  if((fp = fopen("/pub/csc460/bb/BBID.txt","r")) == NULL)
  {
    printf("start:  could not open file.\n");
    return(0);
  }


  /*****  Write SHMID into file and close file.  *****/

  fscanf(fp,"%d",&shmid);
  fclose(fp);



  /****   Attach to the shared memory  ******/

  shmem = (struct bbStruct *) shmat(shmid, NULL, SHM_RND);


  /***** Initialize the Shared Memory to identify each student's row.  *****/

  for (i=0;i<22;i++)
  {
    if (strcmp(shmem->name, "Tas, Taha") == 0)
      strncpy(shmem->name, argv[1], sizeof(shmem->name) - 1);
    shmem++;
  }

}










