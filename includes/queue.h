#ifndef QUEUE_H
#define QUEUE_H

#include <sys/sem.h>
#include "message.h"

#define QUEUE_SIZE 10

typedef struct {
    Message messages[QUEUE_SIZE];
    int head;
    int tail;
    int added_count;
    int removed_count;
    int free_space;
    int semid;
} Queue;

#endif 
