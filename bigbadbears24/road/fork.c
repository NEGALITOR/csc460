// Taha Tas
// CSC 460
// Feb 1 2024
// Fork in the Road

#include<stdio.h>

int main()
{
	int a;
	int n = 3;
	int gen = 0;

	// print some headers for the output
	printf("Gen\tPID\tPPID\n");

	// The fork command spawns a child process, both of which will continue to 'run' starting just past the fork().
	int i = n;
	for (; i>0; i--)
	{
		a = fork();
		if (a == 0)
		{
			i = n;
			n--;
			gen++;
		}
	}

	printf("%d\t%d\t%d\n", gen, getpid(), getppid());

	sleep(1);

	//printf("Bye!!!\n");

	return 0;
}

