#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#define SIGNAL "SIGUSR1"
#define PROCESS_NAME "proc_p1"
#define STATUS_MESSAGE "{%d} [" PROCESS_NAME "] (Status) : "
#define ERROR_MESSAGE "{%d} [" PROCESS_NAME "] (Error) : "
#define VARIABLE_MESSAGE "{%d} [" PROCESS_NAME "] (Bariable) : "

int main(int argc, char const *argv[])
{
    printf(STATUS_MESSAGE "Process started.\n", getpid());
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
    printf(STATUS_MESSAGE "Process suspended until arrival of signal " SIGNAL ".\n", getpid());
    //sigsuspend(&sigusrMask);
    printf(STATUS_MESSAGE "Signal " SIGNAL " received. Process resumed.\n", getpid());
    
    
    char buffer;
    //char* text = NULL;
    char text[150];
    int fileDescriptor = 0;
    long wordLength = 0;
    
    
    fileDescriptor = open("p1.txt", O_RDONLY); //Open file as read-only
    if(fileDescriptor == -1) printf(ERROR_MESSAGE "open() failed. Reason: %s\n", getpid(), strerror(errno));
    
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
    
    printf(VARIABLE_MESSAGE "text = %s", getpid(), text);
    
    write(pipeR1Write, text, wordLength); //Write word to pipe
    
    //Clean up and exit
    //free(text);
    close(fileDescriptor);
    printf(STATUS_MESSAGE "Process finished.\n", getpid());
    exit(0);
}