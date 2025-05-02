#include "consumer.h"
#include "sync.h"
#include "message.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/sem.h>
#include "signal_utils.h"


void consumer(Queue* queue) {
    int semid = queue->semid;
    signal(SIGTERM, sigterm_handler);
    srand(time(NULL) + getpid());

    while (!sigterm_received) {
        struct sembuf sops[2] = {{FULL, -1, 0}, {MUTEX, -1, 0}};
        if (semop(semid, sops, 2) == -1) continue;

        if (sigterm_received) {
            struct sembuf rollback = {MUTEX, 1, 0};
            semop(semid, &rollback, 1);
            break;
        }

        Message msg;
        memcpy(&msg, &queue->messages[queue->head], sizeof(Message));
        queue->head = (queue->head + 1) % QUEUE_SIZE;
        queue->removed_count++;
        queue->free_space++;

        struct sembuf unlock[2] = {{MUTEX, 1, 0}, {EMPTY, 1, 0}};
        semop(semid, unlock, 2);

        uint16_t computed_hash = compute_hash(&msg);
        printf("Consumed message %d: type=%d, size=%d, hash=%u — %s\n",
               queue->removed_count,
               msg.type,
               msg.size,
               msg.hash,
               (computed_hash == msg.hash) ? "OK" : "ERROR");

        struct timespec ts = {1, 0};
        nanosleep(&ts, NULL);
    } 
}
