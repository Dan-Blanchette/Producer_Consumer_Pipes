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
    char string[] = {write_char, '\0'};

    int pipeSize = 32;

    pid_t childpid = fork();

    if (childpid == -1)
    {
        perror("fork");
        exit(1);
    }
    // child process
    if (childpid == 0)
    {
        // Child process closes input side of pipe
        close(fd_p[READ]);

        fcntl(fd_p[WRITE], F_SETPIPE_SZ, &pipeSize);

        while (1)
        {
            write(fd_p[WRITE], string, strlen(string) + 1);
            printf("P%d wrote %s\n", index, string);
            fflush(stdout);
            usleep(rand() % tMaxSleep * 1000); // Do not forget to seed the rand() function
        }
        exit(0);
    }
    else
    {
        // close the write end of pipe and return the pid.
        // Parent process closes up output side of pipe
        close(fd_p[WRITE]);
        fcntl(fd_p[READ], F_SETPIPE_SZ, &pipeSize);

        return childpid;
    }
}

/**
 * @brief This function reads the string from the buffer based on the producer's output 
 * and then prints it to the std console ouput
 * 
 * @param index 
 * @param fd_c 
 * @param tMaxSleep 
 * @return pid_t 
 */

pid_t forkConsumer(int index, int fd_c[], int tMaxSleep)
{
    pipe(fd_c);

    int pipeSize = 32;
    char data[2] = {'\0', '\0'};

    pid_t childpid = fork();

    if (childpid == -1)
    {
        perror("fork");
        exit(1);
    }

    if (childpid == 0)
    {
        // Child process closes output side of pipe
        close(fd_c[WRITE]);

        fcntl(fd_c[READ], F_SETPIPE_SZ, &pipeSize);

        while (1)
        {
            read(fd_c[READ], data, 2); // not sure if theis line is needed buffer stuff is confusing me;
            // Do not forget to seed the rand() function
            printf("C%d read %s\n", index, data);
            usleep(rand() % tMaxSleep * 1000);
        }
    }
    else
    {
        close(fd_c[READ]);
        fcntl(fd_c[WRITE], F_SETPIPE_SZ, &pipeSize);
        return childpid;
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
    //variables for the buffer
    char myQueue[BUFFER_SIZE]; // In runB
    int queueBegin = 0;        // In runB
    int queueEnd = 0;          // In runB

    // a char buffer
    char string[2] = {'\0', '\0'};

    // Read in a string from the pipe
    for (int i = 0; i < iNumProducers; ++i)
    {
        fcntl(fd_p[i][READ], F_SETFL, O_NONBLOCK);
    }
    // Write a string from the buffer
    for (int i = 0; i < iNumConsumers; ++i)
    {
        fcntl(fd_c[i][WRITE], F_SETFL, O_NONBLOCK);
    }

    int totalSumSize = 0;
    int count = 0;
    
    while ((time(NULL) - tStart) <= tRunTime)
    {
        for (int i = 0; i < iNumProducers; ++i)
        {
            if (queueFull(queueBegin, queueEnd))
            {
                break;
            }
            int nbytes = read(fd_p[i][READ], string, sizeof(string));
            if (nbytes > 0)
            {
                queueAdd(myQueue, &queueBegin, &queueEnd, string[0]);
                printf("B received string: %s from producer %d\n", string, i);
                printf("Current Size %d\n", queueSize(queueBegin,queueEnd));
                fflush(stdout);
            }
        }

        ++count;
        totalSumSize += queueSize(queueBegin, queueEnd);

        for (int i = 0; i < iNumConsumers; ++i)
        {
            if (queueEmpty(queueBegin, queueEnd))
            {
                break;
            }
            string[0] = queueGetFirst(myQueue, &queueBegin, &queueEnd); // gets first data
            write(fd_c[i][WRITE], string, strlen(string) + 1); // write the data
            printf("B wrote string: %s from consumer %d\n", string, i); // console output
            fflush(stdout); // flush buffer stdout 
        } 
    }
    printf("Time is up\n");
    for (int i = 0; i < iNumProducers; ++i)
    {
        kill(children[i], SIGKILL);
        printf("Producer %d terminated\n", i);
    }
    for (int i = 0; i < iNumConsumers; ++i)
    {
        kill(children[i+iNumProducers], SIGKILL);
        printf("Consumer %d terminated\n", i);
    }
    printf("Program Complete\n\n");
    printf("Average Buffer size: %f\n", (float)totalSumSize / count); // totalsumsize / count

}

/**
 * @brief Moves to the next element stored in the buffer
 * 
 * @param value 
 */

void queueIncrement(int *value)
{
    (*value)++;
    if (*value >= BUFFER_SIZE)
    {
        *value = 0;
    }
}

/**
 * @brief Returns the size of the queue
 * 
 * @param qBegin 
 * @param qEnd 
 * @return int 
 */

int queueSize(int qBegin, int qEnd)
{
    int size = qEnd - qBegin;

    if (size < 0)
    {
        size += BUFFER_SIZE + 1;
    }

    return size;
}

/**
 * @brief Check to see if the queue is full
 * 
 * @param qBegin 
 * @param qEnd 
 * @return int 
 */

int queueFull(int qBegin, int qEnd)
{
    return (queueSize(qBegin, qEnd) == BUFFER_SIZE); // return 1 if full, 0 if not full
}

/**
 * @brief Checks to see if the queue is empty
 * 
 * @param qBegin 
 * @param qEnd 
 * @return int 
 */

int queueEmpty(int qBegin, int qEnd) // return 1 if queue empty, 0 if not empty
{
    return (queueSize(qBegin, qEnd) == 0);
}

/**
 * @brief Adds new data as a random char to the queue
 * 
 * @param qBegin 
 * @param qEnd 
 * @return int 
 */

int queueAdd(char myQueue[], int *qBegin, int *qEnd, char newElement)
{
    if (queueFull(*qBegin, *qEnd))
    {
        printf("Error. The queue is full!");
        return 0; // False
    }
    queueIncrement(qEnd);
    myQueue[*qEnd] = newElement;
    return 1;
}

/**
 * @brief Get the entry that is first in the queue [FIFO]. Returns the char value after processing each position in the queue.
 * 
 * @param myQueue 
 * @param qBegin 
 * @param qEnd 
 * @return char 
 */

char queueGetFirst(char myQueue[], int *qBegin, int *qEnd)
{
    if (queueEmpty(*qBegin, *qEnd))
    {
        printf("Error. The queue is empty!");
        return 0; // False
    }
    queueIncrement(qBegin);
    if (*qBegin >= BUFFER_SIZE)
    {
        printf("Overflow!\n");
        exit(1);
    }
    char retVal = myQueue[*qBegin];
    return retVal;
}