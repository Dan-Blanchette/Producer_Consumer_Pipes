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

#ifndef PROD_CON_H
#define PROD_CON_H  


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFER_SIZE 1000
#define READ 0
#define WRITE 1


int fd[2];

/** FUNCTIONS **/
void queueIncrement(int *value);
int queueSize(int qBegin, int qEnd);

int queueEmpty(int qBegin, int qEnd); // return 1 if queue empty, 0 if not empty
int queueFull(int qBegin, int qEnd);  // return 1 if full, 0 if not full

int queueAdd(char myQueue[], int *qBegin, int *qEnd, char newElement); // add cahracters to the buffer
char queueGetFirst(char myQueue[], int *qBegin, int *qEnd); // get the first charcter in the queue

pid_t forkProducer(int index, int fd_p[], int tMaxSleep, const char write_char); // "produces" character output from a pipe and adds it to the buffer
pid_t forkConsumer(int index, int fd_p[], int tMaxSleep);// "consumes" character input from the buffer by priority
void runB(time_t tStart, int iNumProducers, int iNumConsumers, int **fd_p, int **fd_c, int tRunTime, pid_t *children);


#endif