#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
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

   int i, fID = getpid(), myID = 0;
   
   int sem_id = semget (IPC_PRIVATE, N, 0777);
   if (sem_id == -1)
   {
      printf("%s","SemGet Failed.\n");
      return (-1);
   }

   semctl(sem_id, myID, SETVAL, 1);
   for (i = 1; i < N; i++)
   {
      semctl(sem_id, i, SETVAL, 0);

      if (fork() == 0)
         myID++;
      else
         break;
   }

   

   for (i = 0; i < N; i++)
   {
      p(myID, sem_id);
      char letter = myID + 'A';
      printf("%c: %d\n", letter, getpid());

      if (myID == N - 1)
         v(0, sem_id);
      else
         v(myID + 1, sem_id);

   }

   // Parent needs to clean up semaphores.
   sleep(2);
   if (fID == getpid())
   {
      if ((semctl(sem_id, 0, IPC_RMID, 0)) == -1)
         printf("%s", "Parent: ERROR in removing sem\n");
   }
      


   
   return 0;
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


