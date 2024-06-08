#include <stdio.h>

int main()
{
int	i;
int     loopCount = 100;

if (fork() == 0)
   {
    for (i=0; i< loopCount; i++)
       {
         printf("\t\t%s %d\n","Child:  ", i);
       }
   }
else
   {
    for (i=0; i< loopCount; i++)
       {
         printf("%s %d\n","Parent: ", i);
       }

   }

sleep(2);
}
