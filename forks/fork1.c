// CSC 460 Another Fork example
// fork1.c
// Bob Allen
// Spring 2024
//

#include <stdio.h>
int main()
{
int a,b;

printf("Hello!!!\n");
printf("A\t\tB\t\tPID\t\tPPID\n\n");

// Spawn a child process.
a = fork();

// Now, both processes from above will each spawn a child process.
b = fork();

printf("%d\t\t%d\t\t%d\t\t%d\n",a,b,getpid(),getppid());

sleep(1);

printf("Bye!!!\n");

return 0;
}

