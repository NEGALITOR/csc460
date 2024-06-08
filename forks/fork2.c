// Bob Allen
// Yet another fork() example
// Spring 2024
//
#include <stdio.h>
int main()
{
int a,b,c;

printf("Hello!!!\n");
printf("A\tB\tC\tPID\tPPID\n\n");
a = fork();
b = fork();
c = fork();

printf("%d\t%d\t%d\t%d\t%d\n",a,b,c,getpid(),getppid());

sleep(1);

printf("Bye!!!- %d\n",getpid());

sleep(1);
return 0;
}

