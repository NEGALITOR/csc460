// Taha Tas
// CSC 460
// Feb 14 2024
// Tortoise Synch 

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void process(int pid, char letter, FILE *syncFile, int N);

int main(int argc, char *argv[])
{

    /* Argument error handling */
    if (argc != 2)
    {
        printf("Incorrect Arguments (no number)");
        return 0;
    }

    int N = atoi(argv[1]);
    if (N < 1 || N > 26)
    {
        printf("Incorrect Arguments (1 - 26)");
        return 0;
    }

    
    /* Open File to write/read a value */

    FILE *fopen(), *fp;
    if((fp = fopen("syncfile","w+")) == NULL)
    {
        printf(":( could not open myLittleFile to write.\n");
        return(0);
    }


    /* Initialize syncfile */

    fprintf(fp,"%d", 1);
    fflush(fp);
    
    /* Do user defined amount for alphabet */
    int i;
    for (i = 0; i < N; i++)
    {
        /* Create Child process and give unique pid */
        pid_t pid = fork();

        if (pid == 0)
        {
            char letter = 'A' + i;
            
            int j;
            for (j = 0 ; j < 5; j++)
            {
                int currentPID, nextPID;
                
                /* Repeatedly check file until pid is currentPID */
                while (currentPID != pid)
                {
                    fscanf(fp,"%d", &currentPID);
                }

                /* Print to screen PID */

                printf("%c: %d\n", letter, getpid());
                fflush(stdout);

                /* Update file to allow nextPID to go */
                
                nextPID = (pid % N);

                fprintf(fp,"%d", nextPID);
                fflush(fp);

                usleep(90000); /* Sleep for a short time to avoid incorrect process ends */

            }
            fclose(fp);
            return 0;
        }

    }

    /* Wait for all children to finish */
    for (i = 0; i < N; i++) {
        wait(NULL);
    }

    fclose(fp);
    return(0);

}
