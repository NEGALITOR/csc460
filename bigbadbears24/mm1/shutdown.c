// Taha Tas
// CSC 460
// April 24, 2024
// Memory Manager 1

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{   
    FILE* fp = fopen("shutdown","rb");
    if(fp == NULL)
    {
        printf("Failed to open shutdown file.\n");
        exit(1);
    }

    int pid;
    while(fread(&pid, sizeof(int), 1, fp) == 1)
    {             
        if(pid > 0)
            kill(pid, SIGTERM);
    }

    fclose(fp);
    remove("shutdown");
    return 0;
}