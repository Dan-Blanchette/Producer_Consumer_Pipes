/**
 * @file producer_consumer.c
 * @author Dan Blanchette
 * @brief A program that will emulate dynamic interprocess communications using forks and pipes
 * @version 0.1
 * @date 2021-03-24
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif


#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BBUFFER_SIZE 1000
#define READ 0
#define WRITE 1

pid_t forkProducer(int index, int fd_p[], int tMaxSleep, const char write_char);
pid_t forkConsumer(int index, int fd_p[], int tMaxSleep);
void runB(time_t tStart, int iNumProducers, int iNumConsumers, int **fd_p, int **fd_c, int tRunTime, pid_t *children);

int main(int argc, char *argv[])
{
    srand(time(NULL));
    time_t start = time(NULL);
    int fd[2];

    if (argc < 6)
    {

        printf("%s run_time num_producers max_sleep_producer num_consumers max_sleep_consumer\n", argv[0]);
        printf("%s 10 4 45 4 73\n", argv[0]);
        return 1;
    }

    int tRunTime = atoi(argv[1]);          // The time in seconds the program will run.
    int iNumProducers = atoi(argv[2]);     // The number of Producers
    int tMaxProducerSleep = atoi(argv[3]); // The maximum sleep time between writes for a producer (In milliseconds)
    int iNumConsumers = atoi(argv[4]);     // The number of Consumers
    int tMaxConsumerSleep = atoi(argv[5]); // The max sleep time between reads for a consumer (In milliseconds)


    int **fd_p = (int **) malloc(iNumProducers * sizeof(int*)); ///< Producers
    int **fd_c = (int **) malloc(iNumConsumers * sizeof(int*)); ///< Consumers

    for (int i = 0; i < iNumProducers; ++i)
    {
        fd_p = (int *) malloc(2 * sizeof(int));
    }

    // get the total size based on the amount of dynamically allocated child processes
    // stored in pid_t array *children
    pid_t *children = (pid_t *)malloc((iNumProducers + iNumConsumers) * sizeof(pid_t));
    pid_t *children2 = (pid_t *)malloc((iNumProducers + iNumConsumers) * sizeof(pid_t));

    // producers for loop
    for (int i = 0; i < iNumProducers; ++i)
    {
        children[i] = forkProducer(i, fd_p[i], tMaxProducerSleep, ('A' + i));
    }

    //add comsumers for loop here
    for (int i = 0; i < iNumConsumers; ++i)
    {
        children2[i] = forkConsumer(i, fd_c[i], tMaxConsumerSleep);
    }

    // routine kills child processes
    runB(start, iNumProducers, iNumConsumers, **fd_p, **fd_c, tRunTime, *children);

    return 0;
}

pid_t forkProducer(int index, int fd_p[], int tMaxSleep, const char write_char)
{
    pipe(fd_p);
    char string[] = {write_char, '\0'};


    int pipeSize =32;

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
        fcntl(fd_p[READ], F_SETPIPE_SZ, &pipeSize);
    }
}

pid_t forkConsumer(int index, int fd_p[], int tMaxSleep)
{

}

void runB(time_t tStart, int iNumProducers, int iNumConsumers, int **fd_p, int **fd_c, int tRunTime, pid_t *children)
{
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
    }

    for(int i = 0; i < (iNumProducers + iNumConsumers); ++i)
    {
        kill(children[1], SIGKILL);
    }
}