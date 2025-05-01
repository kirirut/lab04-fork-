#include "message_queue.h"
#include "semaphore_utils.h"
#include <stdlib.h>
#include <stdio.h>

void init_queue(message_queue* q, int queue_size) {
    // Проверка на корректность размера очереди
    if (queue_size <= 0) {
        fprintf(stderr, "Queue size must be greater than 0.\n");
        exit(1);
    }

    // Инициализация переменных очереди
    q->head = 0;
    q->tail = 0;
    q->added_messages = 0;
    q->removed_messages = 0;
    q->free_space = queue_size;
    q->queue_size = queue_size;
    q->run = 1;

    // Выделение памяти для буфера сообщений
    q->buffer = (message*)malloc(queue_size * sizeof(message));
    if (q->buffer == NULL) {
        fprintf(stderr, "Error allocating memory for the message queue.\n");
        exit(1);
    }

    // Печать информации о размерах очереди
    printf("Queue initialized with size %d\n", queue_size);
}

void enqueue(message_queue* q, const message* msg) {
    // Проверка на переполнение очереди
    if (q->free_space == 0) {
        fprintf(stderr, "Queue is full, cannot enqueue.\n");
        return;
    }

    // Добавление сообщения в очередь
    q->buffer[q->tail] = *msg;
    q->tail = (q->tail + 1) % q->queue_size;  // Циклический сдвиг
    q->free_space--;
    q->added_messages++;

    // Сигнализация о том, что данные добавлены в очередь
    sem_V(sem_fill);
}

message dequeue(message_queue* q) {
    // Проверка на пустую очередь
    if (q->free_space == q->queue_size) {
        fprintf(stderr, "Queue is empty, cannot dequeue.\n");
        exit(1);  // Или обработать ошибку по-другому
    }

    // Извлечение сообщения из очереди
    message msg = q->buffer[q->head];
    q->head = (q->head + 1) % q->queue_size;  // Циклический сдвиг
    q->free_space++;
    q->removed_messages++;

    return msg;
}

void print_queue_state(message_queue* q) {
    // Печать состояния очереди
    printf("________________________________________\n");
    printf("Queue state:\n");
    printf("Head: %d, Tail: %d\n", q->head, q->tail);
    printf("Added messages: %d, Removed messages: %d\n", q->added_messages, q->removed_messages);
    printf("Free space: %d\n", q->free_space);
    printf("Queue size: %d\n", q->queue_size);
    printf("________________________________________\n");
}

void destroy_queue(message_queue* q) {
    // Освобождение памяти для буфера сообщений
    if (q->buffer != NULL) {
        free(q->buffer);
        q->buffer = NULL;  // Установка указателя в NULL после освобождения памяти
    }
}
