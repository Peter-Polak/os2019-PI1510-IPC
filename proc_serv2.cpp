#include <stdlib.h>
#include <stdio.h>
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

#define IP_ADDRESS "127.0.0.1"
#define FILE_NAME "p2.txt"

int main(int argc, char const *argv[])
{
    int udpPort = atoi(argv[1]);
    
    //UDP connection for Server 1 - Read
    int socket;
    struct sockaddr_in serverAddress;
    struct sockaddr_in requestAddress;
    char text[150];
    
    requestAddress.sin_family = AF_INET;
    requestAddress.sin_port = htons(udpPort);
    inet_aton(IP_ADDRESS, &requestAddress.sin_addr.s_addr);
    bzero(&(requestAddress.sin_zero), 8);
    
    thisAddress.sin_family = AF_INET;
    thisAddress.sin_port = htons(udpPort);
    inet_aton(IP_ADDRESS, &thisAddress.sin_addr.s_addr);
    bzero(&(thisAddress.sin_zero), 8);
    
    socket = socket(AF_INET, SOCK_DGRAM, 0);
    bind(socket, (struct sockaddr_in *) &serverAddress, sizeof(struct sockaddr_in));
    //connect(socket, &requestAddress, sizeof(struct sockaddr_in));
    
    //recv();
    recvfrom(socket, text, 150, 0, &requestAddress, sizeof(struct sockaddr_in)); 
    
    //File p2.txt - Write 
    int fileDescriptor;
    
    fileDescriptor = open(FILE_NAME, O_WRONLY);
    write(fileDescriptor, text, 150);
    
    //Clean up and exit
    close(socket);
    close(fileDescriptor);
    exit(0);
}
