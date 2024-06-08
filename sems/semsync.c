#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>

#define CHILD 0
#define PARENT 1

int main()
{
   int i, sem_id, HowManySems = 2;
   int loopCount = 100;

   //  Ask OS for semaphores.
   int *sem_id = semget (IPC_PRIVATE, HowManySems, 0777);

   //  See if you got the request.
   if (sem_id == -1)
   {
      printf("%s","SemGet Failed.\n");
      return (-1);
   }

   //  Initialize your sems.
   semctl(sem_id, CHILD, SETVAL, 1);
   semctl(sem_id, PARENT, SETVAL, 0);


   if (fork() == 0)
   {
      for (i=0; i < loopCount; i++)
      {
         p(CHILD,sem_id);
         printf("\t\t%s %d\n","Child:  ", i);
         v(PARENT,sem_id);
      }
   }
   else
   {
      for (i=0; i < loopCount; i++)
      {
         p(PARENT,sem_id);
         printf("%s %d\n","Parent: ", i);
         v(CHILD,sem_id);
      }

      // Parent needs to clean up semaphores.
      sleep(2);
      if ((semctl(sem_id, 0, IPC_RMID, 0)) == -1)
         printf("%s", "Parent: ERROR in removing sem\n");

   }
}

p(int s,int sem_id)
{
   struct sembuf sops;

   sops.sem_num = s;
   sops.sem_op = -1;
   sops.sem_flg = 0;
   if((semop(sem_id, &sops, 1)) == -1) printf("%s", "'P' error\n");
}

v(int s, int sem_id)
{
   struct sembuf sops;

   sops.sem_num = s;
   sops.sem_op = 1;
   sops.sem_flg = 0;
   if((semop(sem_id, &sops, 1)) == -1) printf("%s","'V' error\n");
}


