#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>

main(int argc, char *argv[])
{
int i;

/*************  BODY  OF  PROGRAM     ***********/
if (fork())
{                // PARENT //
   for (i = 0; i < 100; i++)
   {
        printf("%d: PARENT's Turn!!\n",i);
   }
}
else
{               //  CHILD  //
   for (i = 0; i < 100; i++)
   {
        printf("\t\t%d: CHILD's Turn!!\n",i);
   }
}

printf("alldone\n");
}

