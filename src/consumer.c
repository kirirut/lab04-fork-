#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include "consumer.h"

uint16_t recalculate_hash(const message* msg) {
    uint16_t checksum = 0;
    checksum += msg->type;
    checksum += msg->size;
    for (int i = 0; i < msg->size; ++i) {
        checksum += msg->data[i];
    }
    return checksum;
}

void consumer(message_queue* q, int sem_empty, int sem_fill, int sem_mutex) {
    while (q->run) {
        sem_P(sem_fill);    // Ожидаем, пока есть элементы в очереди
        sem_P(sem_mutex);   // Блокируем очередь для извлечения элемента

        if (!q->run) {      // Если очередь не работает, выходим
            sem_V(sem_mutex);
            break;
        }

        message msg = dequeue(q);   // Извлекаем сообщение из очереди
        int count = q->removed_messages;  // Получаем количество удаленных сообщений

        sem_V(sem_mutex);   // Освобождаем семафор после работы с очередью
        sem_V(sem_empty);   // Увеличиваем количество пустых мест в очереди

        uint16_t actual_hash = recalculate_hash(&msg);  // Пересчитываем хеш
        int is_valid = (actual_hash == msg.hash);        // Проверяем валидность хеша

        printf("Consumed message %d: type=%d, size=%d, hash=%u — %s\n",
               count, msg.type, msg.size, msg.hash,
               is_valid ? "OK" : "CORRUPTED");

        fflush(stdout);
        sleep(1);  // Пауза для имитации обработки
    }
}
