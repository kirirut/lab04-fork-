#include "semaphore_utils.h"

struct sembuf P = {0, -1, 0};
struct sembuf V = {0, 1, 0};

void sem_P(int semid) {
    semop(semid, &P, 1);
}

void sem_V(int semid) {
    semop(semid, &V, 1);
}

void init_semaphores(message_queue* q) {
    sem_empty = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    sem_fill = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    sem_mutex = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);

    if (sem_empty == -1 || sem_fill == -1 || sem_mutex == -1) {
        perror("semget error");
        exit(1);
    }

    // Устанавливаем начальные значения для семафоров
    if (semctl(sem_empty, 0, SETVAL, q->queue_size) == -1) {
        perror("semctl error (sem_empty)");
        exit(1);
    }
    if (semctl(sem_fill, 0, SETVAL, 0) == -1) {
        perror("semctl error (sem_fill)");
        exit(1);
    }
    if (semctl(sem_mutex, 0, SETVAL, 1) == -1) {
        perror("semctl error (sem_mutex)");
        exit(1);
    }
}
