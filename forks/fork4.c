// Bob Allen
// fork() examples galor!
// Spring 2024
//
#include <stdio.h>
int main()
{
int b;

printf("Hello!!!\n");

if (fork() == 0)
{                    ////// Child
  for (b=0;b<10;b++)
    {
     printf("\t\t\tChild: %d\n",getpid());
     sleep(1);
    }
}
else                 ///// Parent
{
  for (b=0;b<5;b++)
    {
     printf("Parent: %d\n",getpid());
     sleep(2);
    }

}

printf("Bye!!!\n");
return 0;
}

