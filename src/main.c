#define _POSIX_C_SOURCE 200809L
#include "queue.h"
#include "sync.h"
#include "producer.h"
#include "consumer.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

int main() {
    Queue* queue = mmap(NULL, sizeof(Queue), PROT_READ | PROT_WRITE,
                        MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (queue == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    queue->head = queue->tail = 0;
    queue->added_count = queue->removed_count = 0;
    queue->free_space = QUEUE_SIZE;

    key_t key = ftok("/tmp", 'S');
    int semid = semget(key, 3, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget failed");
        exit(1);
    }

    queue->semid = semid;
    semctl(semid, EMPTY, SETVAL, QUEUE_SIZE);
    semctl(semid, FULL, SETVAL, 0);
    semctl(semid, MUTEX, SETVAL, 1);

    pid_t producers[100], consumers[100];
    int num_producers = 0, num_consumers = 0;

    printf("Commands:\n");
    printf("  p  - create producer\n");
    printf("  c  - create consumer\n");
    printf("  P  - kill producer\n");
    printf("  C  - kill consumer\n");
    printf("  s  - show queue status\n");
    printf("  q  - quit\n");

    char ch;
    while (1) {
        ch = getchar();
        if (ch == '\n') continue;

        if (ch == 'p' && num_producers < 100) {
            pid_t pid = fork();
            if (pid == 0) {
                producer(queue);
                exit(0);
            } else if (pid > 0) {
                producers[num_producers++] = pid;
            }
        } else if (ch == 'c' && num_consumers < 100) {
            pid_t pid = fork();
            if (pid == 0) {
                consumer(queue);
                exit(0);
            } else if (pid > 0) {
                consumers[num_consumers++] = pid;
            }
        } else if (ch == 'P' && num_producers > 0) {
            kill(producers[--num_producers], SIGTERM);
        } else if (ch == 'C' && num_consumers > 0) {
            kill(consumers[--num_consumers], SIGTERM);
        } else if (ch == 's') {
            struct sembuf lock = {MUTEX, -1, 0};
            semop(semid, &lock, 1);
            printf("Queue size: %d, occupied: %d, free: %d, added: %d, removed: %d, producers: %d, consumers: %d\n",
                   QUEUE_SIZE, QUEUE_SIZE - queue->free_space, queue->free_space,
                   queue->added_count, queue->removed_count,
                   num_producers, num_consumers);
            lock.sem_op = 1;
            semop(semid, &lock, 1);
        } else if (ch == 'q') {
            for (int i = 0; i < num_producers; i++) kill(producers[i], SIGTERM);
            for (int i = 0; i < num_consumers; i++) kill(consumers[i], SIGTERM);
            while (wait(NULL) > 0);
            semctl(semid, 0, IPC_RMID);
            munmap(queue, sizeof(Queue));
            exit(0);
        }
    }

    return 0;
}
