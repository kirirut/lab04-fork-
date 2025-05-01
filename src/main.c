#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include "message_queue.h"
#include "producer.h"
#include "consumer.h"
#include "semaphore_utils.h"
#include "globals.h"

// Семафоры
int sem_empty, sem_fill, sem_mutex;

// Общая память
message_queue *q;
int shmid;

// Массивы для хранения PID производителей и потребителей
pid_t producer_pids[10];  // Ограничение на 10 производителей
pid_t consumer_pids[10];  // Ограничение на 10 потребителей
int producer_count = 0;
int consumer_count = 0;

int main() {
    int queue_size;
    printf("Enter the size of the message queue: ");
    if (scanf("%d", &queue_size) != 1 || queue_size <= 0) {
        fprintf(stderr, "Invalid queue size.\n");
        exit(1);
    }

    // Создание общей памяти
    shmid = shmget(IPC_PRIVATE, sizeof(message_queue), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    // Привязка общей памяти
    q = (message_queue *)shmat(shmid, NULL, 0);
    if (q == (message_queue *)-1) {
        perror("shmat");
        exit(1);
    }

    init_queue(q, queue_size);  // Инициализация очереди
    init_semaphores(q);  // Инициализация семафоров

    printf("Message queue created. Press:\n");
    printf("  'p' - create producer\n");
    printf("  'c' - create consumer\n");
    printf("  's' - show queue status\n");
    printf("  'q' - quit\n");

    char command;

    while (q->run) {
        command = getchar();
        if (command == '\n') continue;

        if (command == 'p') {
            pid_t pid = fork();
            if (pid == 0) {
                producer(q, sem_empty, sem_fill, sem_mutex);
                exit(0);
            }
            // Сохраняем PID процесса производителя
            producer_pids[producer_count++] = pid;
        } else if (command == 'c') {
            pid_t pid = fork();
            if (pid == 0) {
                consumer(q, sem_empty, sem_fill, sem_mutex);
                exit(0);
            }
            // Сохраняем PID процесса потребителя
            consumer_pids[consumer_count++] = pid;
        }
        else if (command == 's') {
            print_queue_state(q);
        }
        else if (command == 'q') {
            q->run = 0;
            break;
        }
    }

    sleep(2);

    // Завершаем все процессы (производители и потребители)
    for (int i = 0; i < producer_count; ++i) {
        kill(producer_pids[i], SIGTERM);
        waitpid(producer_pids[i], NULL, 0);
    }
    for (int i = 0; i < consumer_count; ++i) {
        kill(consumer_pids[i], SIGTERM);
        waitpid(consumer_pids[i], NULL, 0);
    }

    semctl(sem_empty, 0, IPC_RMID);
    semctl(sem_fill, 0, IPC_RMID);
    semctl(sem_mutex, 0, IPC_RMID);
    destroy_queue(q);

    if (shmdt(q) == -1) {
        perror("shmdt");
        exit(1);
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(1);
    }

    return 0;
}
