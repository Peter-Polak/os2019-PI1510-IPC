#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

int main(int argc, char const *argv[])
{
    int pipeR1Write = atoi(argv[1]);
    
    //Wait for signal SIGUSR1, then continue
    sigset_t sigusrMask;
    sigfillset(&sigusrMask);
    sigdelset(&sigusrMask, SIGUSR1);
    sigsuspend(&sigusrMask);
    
    
    char buffer;
    //char* text = NULL;
    char text[150];
    int fileDescriptor = 0;
    long wordLength = 0;
    
    
    fileDescriptor = open("p1.txt", O_RDONLY); //Open file as read-only
    
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
    
    //printf("char* text = \"%s\";\n", &text); //Test print
    
    write(pipeR1Write, text, wordLength); //Write word to pipe
    
    //Clean up and exit
    //free(text);
    close(fileDescriptor);
    exit(0);
}