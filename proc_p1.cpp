#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

#define SIGNAL "SIGUSR1"
#define PROCESS_NAME "proc_p1"

int main(int argc, char const *argv[])
{
    printf("[" PROCESS_NAME "] (Status) : Process started.\n");
    /*sleep(3);
    for(int i = 0; i < argc; i++)
    {
         printf("Argv[%d] = %s\n", i, argv[i]);
    }*/
    
    int pipeR1Write = atoi(argv[1]);
    
    //Wait for signal SIGUSR1, then continue
    sigset_t sigusrMask;
    sigfillset(&sigusrMask);
    sigdelset(&sigusrMask, SIGUSR1);
    printf("[" PROCESS_NAME "] (Status) : Process suspended until arrival of signal " SIGNAL ".\n");
    sigsuspend(&sigusrMask);
    printf("[" PROCESS_NAME "] (Status) : Signal " SIGNAL " received. Process resumed.\n");
    
    
    char buffer;
    //char* text = NULL;
    char text[150];
    int fileDescriptor = 0;
    long wordLength = 0;
    
    
    fileDescriptor = open("p1.txt", O_RDONLY); //Open file as read-only
    if(fileDescriptor == -1) perror("[" PROCESS_NAME "] (Error) : open() failed. Reason: ");
    
    //Find a lenghth of a word
    while(buffer != '\n')
    {
        read(fileDescriptor, &buffer, 1);
        wordLength++;
    }
    wordLength--; //Beacues of '\n'
    
    //Read word of length = wordLength 
    lseek(fileDescriptor, 0L, SEEK_SET);
    //text = (char*) malloc(wordLength);
    read(fileDescriptor, text, wordLength);
    
    printf("[" PROCESS_NAME "] (Variable) : text = %s", text);
    
    write(pipeR1Write, text, wordLength); //Write word to pipe
    
    //Clean up and exit
    //free(text);
    close(fileDescriptor);
    printf("[" PROCESS_NAME "] (Status) : Process finished.\n");
    exit(0);
}