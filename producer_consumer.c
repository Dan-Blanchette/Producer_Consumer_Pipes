/**
 * @file   producer_consumer.c
 * @author WRITE STUDENT NAME
 * @brief  WRITE BRIEF DESCRIPTION ABOUT THE PROGRAM
 * @date   WRITE DATE STARTED
 * 
 */
#include <stdio.h>

#define BBUFFER_SIZE 1000
#define READ         0
#define WRITE        1


int main(int argc, char *argv[])
{
    if (argc < 6)
    {
        printf("%s run_time num_producers max_sleep_producer num_consumers max_sleep_consumer\n", argv[0]);
        printf("%s 10 4 45 4 73\n", argv[0]);
        return 1;
    }

    // YOUR CODE HERE

    return 0;
}
