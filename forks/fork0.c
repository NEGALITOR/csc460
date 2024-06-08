// CSC 460 Fork example
// fork0.c
// Bob Allen
// Spring 2024
//

#include <stdio.h>
int main()
{
int a;

// print some headers for the output
printf("A\tPID\tPPID\n\n");

// The fork command spawns a child process, both of which will continue to 'run' starting just past the fork().
a = fork();

printf("%d\t%d\t%d\n",a,getpid(),getppid());

sleep(1);

printf("Bye!!!\n");

return 0;
}

