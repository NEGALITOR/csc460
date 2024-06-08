#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>

main(int argc, char *argv[])
{
int i;
int N;                  // Holds the number of procs to generate
int myID = 0;           // used to identify procs in sync
int LoopCount = 10;     // Times each proc will "speak"

/*****  Get the command line argument for how many procs. Default of 5 ****/
if (argc < 2)
        N = 5;
else
        N = atoi(argv[1]);      //  Not error testing here :(


/*************  Spawn all the Processes *********/

for (i = 1; i < N; i++)
{
  if (fork() > 0) break; // send parent on to Body
  myID++;
}
printf("I'm Alive: %d - %d\n",getpid(),myID);
sleep(1);

/*************  BODY  OF  PROGRAM     ***********/

   for (i = 0; i < LoopCount; i++)
   {

        printf("%d's Turn!!\n",myID);
   }


sleep(1);

printf("alldone\n");
}

