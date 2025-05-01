#ifndef GLOBALS_H
#define GLOBALS_H

#include <pthread.h>

extern int sem_empty, sem_fill, sem_mutex;
extern pthread_t *producer_threads;
extern pthread_t *consumer_threads;
extern int producer_count;
extern int consumer_count;


#endif
