#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h> 

#include <string.h> 
#include <netinet/in.h>
#include <sys/socket.h> 
#include <arpa/inet.h>

#define IP_ADDRESS "127.0.0.1"

// struct sembuf
// {
//     short sem_num;
//     short sem_op;
//     short sem_flg;
// } 

int main(int argc, char const *argv[])
{
    printf("Process \"proc_d\" started.\n");
    
    int shmemSM2 = atoi(argv[1]);
    int semaphoreS2 = atoi(argv[2]);
    int tcpPort = atoi(argv[3]);
    
    //Semaphore S2
    struct sembuf operation;
    
    operation.sem_num = 0;
    operation.sem_op = -1;
    operation.sem_flg = SEM_UNDO;
    
    semop(semaphoreS2, &operation, 1);
    
    //Shared Memory M2 - Read
    char *sharedMemory; 
    char text[150];
    
    sharedMemory = (char *) shmat(shmemSM2, NULL, 0);
    
    strncpy(text, sharedMemory, 150);
    
    //TCP connection to Server 1 - Write
    int socketDescriptor;
    struct sockaddr_in serverAddress;
    
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(tcpPort);
    inet_aton(IP_ADDRESS, &serverAddress.sin_addr);
    bzero(&(serverAddress.sin_zero), 8);
    
    socketDescriptor = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    connect(socketDescriptor, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr));
    
    write(socketDescriptor, text, sizeof(text));
    
    //Clean up and exit
    shmdt(sharedMemory);
    close(socketDescriptor);
    printf("Process \"proc_d\" finished.\n");
    exit(0);
}