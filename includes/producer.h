#include "message.h"
#include "message_queue.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <string.h>
#include "semaphore_utils.h"

void producer(message_queue* q, int sem_empty, int sem_fill, int sem_mutex);
