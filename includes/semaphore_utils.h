#ifndef SEMAPHORE_UTILS_H
#define SEMAPHORE_UTILS_H

#include <sys/sem.h>
#include "globals.h"
#include "message.h"
#include <stdio.h>
#include <stdlib.h>
#include "message_queue.h"

extern struct sembuf P;
extern struct sembuf V;


void sem_P(int semid);
void sem_V(int semid);
void init_semaphores(message_queue* q);
void wait_for_threads();
void expand_thread_arrays();
#endif 
