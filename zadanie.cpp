#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>

#define PROCESS_NAME "zadanie"

union semun
{
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
};

int proc_p1(int pipeR1Write);
int proc_p2(int pipeR1Write);
int proc_pr(int pidP1,int pidP2, int pipeR1Read, int pipeR2Write);
int proc_t(int pipeR2Read, int shmemSM1, int semaphoreS1);
int proc_s(int shmemSM1, int semaphoreS1, int shmemSM2, int semaphoreS2);
int proc_d(int shmemSM2, int semaphoreS2, int tcpPort);
int proc_serv1(int tcpPort, int udpPort);
int proc_serv2(int udpPort);

int main(int argc, char const *argv[])
{
    printf("[" PROCESS_NAME "] (Status) : Process started.\n");
    
    //struct siginfo_t response;
    int childStatus;
    sigset_t sigusrMask;
    sigfillset(&sigusrMask);
    sigdelset(&sigusrMask, SIGUSR1);
    
    //Pipes
    //---------------------------------------------------------------------------------------------
    int pipeR1[2];
    int pipeR2[2];
    
    pipe(pipeR1);
    pipe(pipeR2);
    printf("[" PROCESS_NAME "] (Variable) : pipeR1 = %d, %d \n", pipeR1[0], pipeR1[1]);
    printf("[" PROCESS_NAME "] (Variable) : pipeR2 = %d, %d \n", pipeR2[0], pipeR2[1]);
    //---------------------------------------------------------------------------------------------
    
    
    //proc_p1, proc_p2, proc_pr
    //---------------------------------------------------------------------------------------------
    int proc_p1ID = proc_p1(pipeR1[1]);
    int proc_p2ID = proc_p2(pipeR1[1]);
    int proc_prID = proc_pr(proc_p1ID, proc_p2ID, pipeR1[0], pipeR2[1]);
    printf("[" PROCESS_NAME "] (Variable) : proc_p1ID = %d\n", proc_p1ID);
    printf("[" PROCESS_NAME "] (Variable) : proc_p2ID = %d\n", proc_p2ID);
    printf("[" PROCESS_NAME "] (Variable) : proc_prID = %d\n", proc_prID);
    
    printf("[" PROCESS_NAME "] (Status) : Process suspended until \"proc_p1\" exits.\n");
    waitpid(proc_p1ID, &childStatus, 0);
    printf("[" PROCESS_NAME "] (Status) : Process suspended until \"proc_p2\" exits.\n");
    waitpid(proc_p2ID, &childStatus, 0);
    printf("[" PROCESS_NAME "] (Status) : Process suspended until \"proc_pr\" exits.\n");
    waitpid(proc_prID, &childStatus, 0);
    //---------------------------------------------------------------------------------------------
    
    
    //Shared memories and semaphores
    //---------------------------------------------------------------------------------------------
    int shmemSM1, shmemSM2;
    int semaphoreS1, semaphoreS2;
    union semun semaphoreArgument;
    
    shmemSM1 = shmget(ftok(".", 'H'), 150, 0777);
    shmemSM2 = shmget(ftok(".", 'E'), 150, 0777);
    semaphoreS1 = semget(ftok(".", 'L'), 2, 0777);
    semaphoreS2 = semget(ftok(".", 'P'), 2, 0777);
    
    
    semaphoreArgument.val = 1;
    semctl(semaphoreS1, 0, SETVAL, semaphoreArgument);
    semctl(semaphoreS2, 0, SETVAL, semaphoreArgument);
    
    semaphoreArgument.val = 0;
    semctl(semaphoreS1, 1, SETVAL, semaphoreArgument);
    semctl(semaphoreS2, 1, SETVAL, semaphoreArgument);
    //---------------------------------------------------------------------------------------------
    
    
    //proc_t, proc_s
    //---------------------------------------------------------------------------------------------
    int proc_sID = proc_s(shmemSM1, semaphoreS1, shmemSM2, semaphoreS2);
    sigsuspend(&sigusrMask);
    int proc_tID = proc_t(pipeR2[0], shmemSM1, semaphoreS1);

    waitpid(proc_tID, &childStatus, 0);
    waitpid(proc_sID, &childStatus, 0);
    //---------------------------------------------------------------------------------------------
    
    
    
    
    //proc_d, proc_serv1, proc_serv2
    //---------------------------------------------------------------------------------------------
    int tcpPort = atoi(argv[1]);
    int udpPort = atoi(argv[2]);
    
    int proc_serv1ID = proc_serv1(tcpPort, udpPort);
    sigsuspend(&sigusrMask);
    int proc_dID = proc_d(shmemSM2, semaphoreS2, tcpPort);
    int proc_serv2ID = proc_serv2(udpPort);
    
    waitpid(proc_dID, &childStatus, 0);
    waitpid(proc_serv1ID, &childStatus, 0);
    waitpid(proc_serv2ID, &childStatus, 0);
    //---------------------------------------------------------------------------------------------
    
    //Clean up and exit
    //---------------------------------------------------------------------------------------------
    semctl(semaphoreS1, 0, IPC_RMID);
    semctl(semaphoreS2, 0, IPC_RMID);
    shmctl(shmemSM1, IPC_RMID, NULL);
    shmctl(shmemSM2, IPC_RMID, NULL);
    printf("[" PROCESS_NAME "] (Status) : Process finished.\n");
    return 0;
    //---------------------------------------------------------------------------------------------
}




int proc_p1(int pipeR1Write)
{
    char processName[] = "proc_p1";
    char* proc_p1Arguments[] = {processName, NULL, NULL};
    char* proc_p1Enviroment[] = { NULL };
    char sprintfBuffer[20];
    
    sprintf(sprintfBuffer, "%d", pipeR1Write);
    proc_p1Arguments[1] = sprintfBuffer;
    
    int processID = fork();
    if(processID == -1) perror("[" PROCESS_NAME "] (Error) : proc_p1 fork() failed. Reason: ");
    
    if (processID == 0)
    {
        int returnValue = execve(proc_p1Arguments[0], proc_p1Arguments, proc_p1Enviroment);
        if(returnValue == -1) perror("[" PROCESS_NAME "] (Error) : proc_p1 execve() failed. Reason: ");
    }
    
    return processID;
}


int proc_p2(int pipeR1Write)
{
    char processName[] = "proc_p2";
    char* proc_p2Arguments[] = {processName, NULL, NULL};
    char* proc_p2Enviroment[] = { NULL };
    char sprintfBuffer[20];
    
    sprintf(sprintfBuffer, "%d", pipeR1Write);
    proc_p2Arguments[1] = sprintfBuffer;
    
    int processID = fork();
    if(processID == -1) perror("[" PROCESS_NAME "] (Error) : proc_p2 fork() failed. Reason: ");
    
    if (processID == 0)
    {
        int returnValue = execve(proc_p2Arguments[0], proc_p2Arguments, proc_p2Enviroment);
        if(returnValue == -1) perror("[" PROCESS_NAME "] (Error) : proc_p2 execve() failed. Reason: ");
    }
    
    return processID;
}

int proc_pr(int pidP1, int pidP2, int pipeR1Read, int pipeR2Write)
{
    char processName[] = "proc_pr";
    char* proc_prArguments[] = {processName, NULL, NULL, NULL, NULL, NULL};
    char* proc_prEnviroment[] = { NULL };
    
    char pidP1Buffer[20];
    sprintf(pidP1Buffer, "%d", pidP1);
    proc_prArguments[1] = pidP1Buffer;
    
    char pidP2Buffer[20];
    sprintf(pidP2Buffer, "%d", pidP2);
    proc_prArguments[2] = pidP2Buffer;
    
    char pipeR1ReadBuffer[20];
    sprintf(pipeR1ReadBuffer, "%d", pipeR1Read);
    proc_prArguments[3] = pipeR1ReadBuffer;
    
    char pipeR2WriteBuffer[20];
    sprintf(pipeR2WriteBuffer, "%d", pipeR2Write);
    proc_prArguments[4] = pipeR2WriteBuffer;
    
    printf("[" PROCESS_NAME "] (Variable) : proc_prArguments = %s %s %s %s", proc_prArguments[0], proc_prArguments[1], proc_prArguments[2], proc_prArguments[3]);
    
    int processID = fork();
    if(processID == -1) perror("[" PROCESS_NAME "] (Error) : proc_pr fork() failed. Reason: ");
    
    if (processID == 0)
    {
        int returnValue = execve(proc_prArguments[0], proc_prArguments, proc_prEnviroment);
        if(returnValue == -1) perror("[" PROCESS_NAME "] (Error) : proc_pr execve() failed. Reason: ");
    }
    
    return processID;
}

int proc_t(int pipeR2Read, int shmemSM1, int semaphoreS1)
{
    char processName[] = "proc_t";
    char* proc_tArguments[] = {processName, NULL, NULL, NULL, NULL};
    char* proc_tEnviroment[] = { NULL };
    
    char pipeR2ReadBuffer[20];
    sprintf(pipeR2ReadBuffer, "%d", pipeR2Read);
    proc_tArguments[1] = pipeR2ReadBuffer;
    
    char shmemSM1Buffer[20];
    sprintf(shmemSM1Buffer, "%d", shmemSM1);
    proc_tArguments[2] = shmemSM1Buffer;
    
    char semaphoreS1Buffer[20];
    sprintf(semaphoreS1Buffer, "%d", semaphoreS1);
    proc_tArguments[3] = semaphoreS1Buffer;
    
    int processID = fork();
    
    if (processID == 0)
    {
        int returnValue = execve(proc_tArguments[0], proc_tArguments, proc_tEnviroment);
        if(returnValue == -1) perror("[" PROCESS_NAME "] (Error) : proc_t execve() failed. Reason: ");
    }
    
    return processID;
}


int proc_s(int shmemSM1, int semaphoreS1, int shmemSM2, int semaphoreS2)
{
    char processName[] = "proc_s";
    char* proc_sArguments[] = {processName, NULL, NULL, NULL, NULL, NULL};
    char* proc_sEnviroment[] = { NULL };
    
    char shmemSM1Buffer[20];
    sprintf(shmemSM1Buffer, "%d", shmemSM1);
    proc_sArguments[1] = shmemSM1Buffer;
    
    char semaphoreS1ReadBuffer[20];
    sprintf(semaphoreS1ReadBuffer, "%d", semaphoreS1);
    proc_sArguments[2] = semaphoreS1ReadBuffer;
    
    char shmemSM2Buffer[20];
    sprintf(shmemSM2Buffer, "%d", shmemSM2);
    proc_sArguments[3] = shmemSM2Buffer;
    
    char semaphoreS2Buffer[20];
    sprintf(semaphoreS2Buffer, "%d", semaphoreS2);
    proc_sArguments[4] = semaphoreS2Buffer;
    
    int processID = fork();
    
    if (processID == 0)
    {
        int returnValue = execve(proc_sArguments[0], proc_sArguments, proc_sEnviroment);
        if(returnValue == -1) perror("[" PROCESS_NAME "] (Error) : proc_s execve() failed. Reason: ");
    }
    
    return processID;
}


int proc_d(int shmemSM2, int semaphoreS2, int tcpPort)
{
    char processName[] = "proc_d";
    char* proc_dArguments[] = {processName, NULL, NULL, NULL, NULL};
    char* proc_dEnviroment[] = { NULL };
    
    char shmemSM2Buffer[20];
    sprintf(shmemSM2Buffer, "%d", shmemSM2);
    proc_dArguments[1] = shmemSM2Buffer;
    
    char semaphoreS2Buffer[20];
    sprintf(semaphoreS2Buffer, "%d", semaphoreS2);
    proc_dArguments[2] = semaphoreS2Buffer;
    
    char tcpPortBuffer[20];
    sprintf(tcpPortBuffer, "%d", tcpPort);
    proc_dArguments[3] = tcpPortBuffer;
    
    int processID = fork();
    
    if (processID == 0)
    {
        int returnValue = execve(proc_dArguments[0], proc_dArguments, proc_dEnviroment);
        if(returnValue == -1) perror("[" PROCESS_NAME "] (Error) : proc_d execve() failed. Reason: ");
    }
    
    return processID;
}


int proc_serv1(int tcpPort, int udpPort)
{
    char processName[] = "proc_serv1";
    char* proc_serv1Arguments[] = {processName, NULL, NULL, NULL};
    char* proc_serv1Enviroment[] = { NULL };
    
    char tcpPortBuffer[20];
    sprintf(tcpPortBuffer, "%d", tcpPort);
    proc_serv1Arguments[1] = tcpPortBuffer;
    
    char udpPortBuffer[20];
    sprintf(udpPortBuffer, "%d", udpPort);
    proc_serv1Arguments[2] = udpPortBuffer;
    
    int processID = fork();
    
    if (processID == 0)
    {
        int returnValue = execve(proc_serv1Arguments[0], proc_serv1Arguments, proc_serv1Enviroment);
        if(returnValue == -1) perror("[" PROCESS_NAME "] (Error) : proc_serv1 execve() failed. Reason: ");
    }
    
    return processID;
}


int proc_serv2(int udpPort)
{
    char processName[] = "proc_serv2";
    char* proc_serv2Arguments[] = {processName, NULL, NULL};
    char* proc_serv2Enviroment[] = { NULL };
    
    char udpPortBuffer[20];
    sprintf(udpPortBuffer, "%d", udpPort);
    proc_serv2Arguments[1] = udpPortBuffer;
    
    int processID = fork();
    
    if (processID == 0)
    {
        int returnValue = execve(proc_serv2Arguments[0], proc_serv2Arguments, proc_serv2Enviroment);
        if(returnValue == -1) perror("[" PROCESS_NAME "] (Error) : proc_serv2 execve() failed. Reason: ");
    }
    
    return processID;
}