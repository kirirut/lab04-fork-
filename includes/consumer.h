#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include "message.h"
#include "message_queue.h"
#include "semaphore_utils.h"

uint16_t recalculate_hash(const message* msg);

void consumer(message_queue* q, int sem_empty, int sem_fill, int sem_mutex);