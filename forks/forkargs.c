// Bob Allen
// Using command line args
// Spring 2024
// 
#include <stdio.h>
int main(int argc, char * argv[])
{
int N;

if (argc == 1) 
  N = 5;
else
  N = atoi(argv[1]);

if (N < 1) N = 1;
if (N > 10) N = 10;

printf("N: %d\n\n",N);

printf("PID\tPPID\n\n");

int i;
for (i=0; i< N; i++)
    fork();

printf("%d\t%d\n",getpid(),getppid());

sleep(1);

printf("All Done.\n");

sleep(1);
return 0;
}

