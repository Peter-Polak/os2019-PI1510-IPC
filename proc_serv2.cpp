#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <sys/types.h>
#include <sys/socket.h> 
#include <arpa/inet.h>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>
#include <netinet/in.h>

#define PROCESS_NAME "proc_serv2"
#define IP_ADDRESS "127.0.0.1"
#define FILE_NAME "p2.txt"

int main(int argc, char const *argv[])
{
    printf("[" PROCESS_NAME "] : Process started.\n");
    
    int udpPort = atoi(argv[1]);
    
    //UDP connection for Server 1 - Read
    int socketDescriptor;
    struct sockaddr_in thisAddress;
    struct sockaddr_in requestAddress;
    socklen_t requestAdressLength = sizeof(struct sockaddr);
    
    char text[150];
    
    requestAddress.sin_family = AF_INET;
    requestAddress.sin_port = htons(udpPort);
    inet_aton(IP_ADDRESS, &requestAddress.sin_addr);
    bzero(&(requestAddress.sin_zero), 8);
    
    thisAddress.sin_family = AF_INET;
    thisAddress.sin_port = htons(udpPort);
    inet_aton(IP_ADDRESS, &thisAddress.sin_addr);
    bzero(&(thisAddress.sin_zero), 8);
    
    socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    bind(socketDescriptor, (struct sockaddr *) &thisAddress, sizeof(struct sockaddr));
    //connect(socketDescriptor, &requestAddress, sizeof(struct sockaddr_in));
    
    //recv();
    recvfrom(socketDescriptor, text, 150, 0, (struct sockaddr *)&requestAddress, (socklen_t *)sizeof(struct sockaddr)); 
    printf("[" PROCESS_NAME "] : text = %s", text);
    
    //File p2.txt - Write 
    int fileDescriptor;
    
    fileDescriptor = open(FILE_NAME, O_WRONLY);
    write(fileDescriptor, text, 150);
    
    //Clean up and exit
    close(socketDescriptor);
    close(fileDescriptor);
    printf("[" PROCESS_NAME "] Process finished.\n");
    exit(0);
}
