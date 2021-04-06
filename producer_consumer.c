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

#include "prod_con.h"

int main(int argc, char *argv[])
{
    srand(time(NULL));
    time_t start = time(NULL);
    
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

    /** declare dynamic 2D arrays of type int **/
    
    // Producers
    int **fd_p = (int **) malloc(iNumProducers * sizeof(int *)); 
    // Consumers
    int **fd_c = (int **) malloc(iNumConsumers * sizeof(int *)); 

    // intialize producers
    for (int i = 0; i < iNumProducers; ++i)
    {
        fd_p[i] = (int *) malloc(2 * sizeof(int));
    }

    // get the total size based on the amount of dynamically allocated child processes
    // stored in pid_t array *children
    pid_t *children = (pid_t *)malloc((iNumProducers + iNumConsumers) * sizeof(pid_t));
    pid_t *children2 = (pid_t *)malloc((iNumProducers + iNumConsumers) * sizeof(pid_t));

    // producers for loop copies entires from forkProducer()
    for (int i = 0; i < iNumProducers; ++i)
    {
        children[i] = forkProducer(i, fd_p[i], tMaxProducerSleep, ('A' + i));
    }

    // Initialize consumer for loop
    for (int i = 0; i < iNumConsumers; ++i)
    {
        fd_c[i] = (int *)malloc(2 * sizeof(int));
    }

    //consumer for loop copies entries from forkConsumer()
    for (int i = 0; i < iNumConsumers; ++i)
    {
        children2[i] = forkConsumer(i, fd_c[i], tMaxConsumerSleep);
    }

    // routine kills child processes
    runB(start, iNumProducers, iNumConsumers, fd_p, fd_c, tRunTime, children);

    return 0;
}