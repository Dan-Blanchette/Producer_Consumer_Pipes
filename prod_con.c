/** DEFINITIONS FILE **/
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif


#include "prod_con.h"



/**
 * @brief creates output value based on the user's parameters entered from the command line and  
 * 
 * @param index 
 * @param fd_p 
 * @param tMaxSleep 
 * @param write_char 
 * @return pid_t 
 */

pid_t forkProducer(int index, int fd_p[], int tMaxSleep, const char write_char)
{
    pipe(fd_p);
    int prodNum = 0;
    char string[] = {write_char, '\0'};


    int pipeSize = 32;

    pid_t childpid = fork();

    if (childpid == -1)
    {
        perror("fork");
        exit(1);
    }

    if (childpid == 0)
    {
        // Child process closes up input side of pipe
        close(fd_p[READ]);

        fcntl(fd_p[WRITE], F_SETPIPE_SZ, &pipeSize);

        while (1)
        {
            write(fd_p[WRITE], string, strlen(string) + 1);
            usleep(rand() % tMaxSleep * 1000); // Do not forget to seed the rand() function
        }

        // Send "string" through the output side of pipe
        write(fd_p[WRITE], string, (strlen(string) + 1));
        exit(0);
    }
    else
    {
        // close the write end of pipe and return the pid.
        // Parent process closes up output side of pipe
        close(fd_p[WRITE]);
        prodNum -= prodNum + 1;
        fcntl(fd_p[READ], F_SETPIPE_SZ, &pipeSize);
        
    }
}

/**
 * @brief This function reads the string from the buffer based on the producer's output 
 * and then prints it to the std console ouput
 * 
 * @param index 
 * @param fd_p 
 * @param tMaxSleep 
 * @return pid_t 
 */

pid_t forkConsumer(int index, int fd_c[], int tMaxSleep)
{
    pipe(fd_c);

    int pipeSize = 32;

    pid_t childpid = fork();

    if (childpid == -1)
    {
        perror("fork");
        exit(1);
    }

    if (childpid == 0)
    {
        // Child process closes up input side of pipe
        close(fd_c[READ]);

        fcntl(fd_c[WRITE], F_SETPIPE_SZ, &pipeSize);

        while (1)
        {
            // printf("%c recieved string: %c from producer, buffChar1, buffChar2");
        }
    }
    else
    {

    }


}

/**
 * @brief This function is the buffer queue that takes input from the producer and queues it to be
 * read by the consumer. After a string value is read from the buffer, the function prints the output to stdout 
 * and kills the child process thereby removing the char value from the queue.
 * @param tStart 
 * @param iNumProducers 
 * @param iNumConsumers 
 * @param fd_p 
 * @param fd_c 
 * @param tRunTime 
 * @param children 
 */

void runB(time_t tStart, int iNumProducers, int iNumConsumers, int **fd_p, int **fd_c, int tRunTime, pid_t *children)
{
    // a char buffer 
    char readbuffer[2] = {'\0','\0'};

    // Read in a string from the pipe
     for (int i = 0; i < iNumProducers; ++i)
    {
        fcntl(fd_p[i][0], F_SETFL, O_NONBLOCK);
    }

    while((time(NULL) - tStart) <= tRunTime)
    {
        for (int i = 0; i < iNumProducers; ++i)
        {
            int nbytes = read(fd_p[i][READ], readbuffer, sizeof(readbuffer));
            printf("Received string: %s", readbuffer);
        }

        for (int i = 0; i < iNumConsumers; ++i)
        {
            fd_c[i][0];
        }
    }

    for(int i = 0; i < (iNumProducers + iNumConsumers); ++i)
    {
        kill(children[1], SIGKILL);
    }
}