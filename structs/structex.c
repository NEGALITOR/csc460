// Bob Allen
// Struct example
//
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

struct myVals
{
int	pid;
int	amt;
char	name[25];
};  /////// NOTE THE semicolon!

// Prototype for prtStructs
void prtStructs(struct myVals *s, int count);

int main(int argc, char *argv[])
{
struct myVals oneStruct; // actual structure
struct myVals *recptr; // NOTE: Its a pointer to a struct.
struct myVals vals[10]; // Array of 10 structures

int i;
int ids = 460;

// Fill in one structure:
oneStruct.pid = getpid();
oneStruct.amt = 100;
strcpy(oneStruct.name,"CSC 460");

// Print out each field of a structure:
printf("Here are the values from one structure:\n");
printf("oneStruct's pid: %d\n",oneStruct.pid);
printf("oneStruct's amd: %d\n",oneStruct.amt);
printf("oneStruct's name: %s\n",oneStruct.name);

// Print out one structures values by passing its pointer to prtStructs
printf("Here are the same structures values printed via a pointer to the struct:\n");
prtStructs(&oneStruct,1);

// Build an array of 10 structures
printf("\nBUILD THE ARRAY OF STRUCTS:\n");
for (i=0; i<10; i++)
{
	vals[i].pid = ids++;
	vals[i].amt = ids/(i+1);
	strcpy(vals[i].name,"Be The Bear");
	int j;
	for (j=0; j<i;j++)
		vals[i].name[11+j]='!';
	vals[i].name[11+j] = 0;  // end of string char
}

printf("Printing the original vals in an array of 10 structs\n");
prtStructs(vals,10);

// Use a pointer into the array to make changes in the array
recptr = vals;

recptr->pid = 0;
recptr->amt = 0;
strcpy(recptr->name,"Operating Systems");

printf("Printing after first update.\n");
prtStructs(vals,10);

// Note that incrementing a pointer to a structure, increments by the size of each struct
recptr +=5;

recptr->pid = 5;
recptr->amt = 5;
strcpy(recptr->name,"Mercer University");

printf("Printing after recptr += 5 update.\n");
prtStructs(vals,10);

return 0;
}

void prtStructs(struct myVals *s, int count)
{
	int i=0;
	for (i=0;i<count;i++)
   		printf("s[%d]: %d\t%d\t%s\n",i, s[i].pid, s[i].amt, s[i].name);
	return;
}





