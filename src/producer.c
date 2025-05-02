#include "producer.h"
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

void producer(Queue* queue) {
    int semid = queue->semid;
    signal(SIGTERM, sigterm_handler);
    srand(time(NULL) + getpid());

    while (!sigterm_received) {
        Message msg;
        msg.type = rand() % 256;
        msg.size = rand() % 256;
        for (int i = 0; i < msg.size; i++) {
            msg.data[i] = rand() % 256;
        }
        msg.hash = compute_hash(&msg);

        struct sembuf sops[2] = {{EMPTY, -1, 0}, {MUTEX, -1, 0}};
        if (semop(semid, sops, 2) == -1) continue;

        if (sigterm_received) {
            struct sembuf rollback = {MUTEX, 1, 0};
            semop(semid, &rollback, 1);
            break;
        }

        memcpy(&queue->messages[queue->tail], &msg, sizeof(Message));
        queue->tail = (queue->tail + 1) % QUEUE_SIZE;
        queue->added_count++;
        queue->free_space--;

        struct sembuf unlock[2] = {{MUTEX, 1, 0}, {FULL, 1, 0}};
        semop(semid, unlock, 2);

        printf("Produced message %d: type=%d, size=%d, hash=%u\n",
               queue->added_count,
               msg.type,
               msg.size,
               msg.hash);

        struct timespec ts = {1, 0};
        nanosleep(&ts, NULL);
    }
}
