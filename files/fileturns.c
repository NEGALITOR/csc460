// Bob Allen
// Demo two procs alternating printing
// CSC 460

#include <stdio.h>

main(int argc, char *argv[])
{
FILE *fopen(), *fp;


/*****  Open File to write a value  *****/

if((fp = fopen("syncfile","w")) == NULL)
    {
    printf(":( could not open myLittleFile to write.\n");
    return(0);
    }


/*****  Write into syncfile ID of whose turn.  *****/

fprintf(fp,"%d",0);
fclose(fp);


/*****  Create Child process and give unique myID *****/

int  myID = 0;
int  otherID = 1;

if (fork() == 0)
{
   myID = 1;
   otherID = 0;
}


/*****  LOOP 100 Times  *****/

int  i = 0;
int  found;

for (i=0; i<100; i++)
{

     /***** Repeatedly check file until myID is found *****/

     found = -1;
     while (found != myID)
     {
         if((fp = fopen("syncfile","r")) == NULL)
             {
             printf(":( could not open syncfile to read.\n");
             return(0);
             }

             fscanf(fp,"%d",&found);
             fclose(fp);
     }
     
     // It must be my turn to do something.....

     printf("%d: %d\n",myID,getpid());

     // Update file to allow otherID to go 

     if((fp = fopen("syncfile","w")) == NULL)
         {
         printf(":( could not open myLittleFile to write.\n");
         return(0);
         }
     fprintf(fp,"%d",otherID);
     fclose(fp);

}

    return(0);

}
