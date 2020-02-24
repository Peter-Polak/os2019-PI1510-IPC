#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h> 

// struct sembuf
// {
//  short sem_num;
//  short sem_op;
//  short sem_flg;
// } 

int main(int argc, char const *argv[])
{
    printf("Process \"proc_t\" started.\n");
    for(int i = 0; i < argc; i++)
    {
         printf("%s ", argv[i]);
    }
    
    int pipeR2Read = atoi(argv[1]);
    int shmemSM1 = atoi(argv[2]);
    int semaphoreS1 = atoi(argv[3]);
    
    //Pipe R2 - Read
    char text[150];
    
    read(pipeR2Read, text, 150);
    
    //Shared memory 1 - Write
    char *sharedMemory = NULL; 
    
    sharedMemory = (char*) shmat(shmemSM1, NULL, 0);
    
    strncpy(sharedMemory, text, 150);
    
    //Semaphore 1
    struct sembuf operation[2];
    
    operation[0].sem_num = 0;
    operation[0].sem_op = -1;
    operation[0].sem_flg = SEM_UNDO;
    
    operation[1].sem_num = 1;
    operation[1].sem_op = 1;
    operation[1].sem_flg = SEM_UNDO;
    
    semop(semaphoreS1, operation, 2);
    
    //Clean up and exit
    shmdt(sharedMemory);
    printf("Process \"proc_t\" finished.\n");
    exit(0);
}