#include "producer.h"
#include "globals.h"
void producer(message_queue* q, int sem_empty, int sem_fill, int sem_mutex) {
    unsigned int seed = time(NULL) ^ getpid();
    while(q->run) {        
        sem_P(sem_empty);           // ждем, пока будет место
        sem_P(sem_mutex);           // входим в критическую секцию

        if (!q->run) {
            sem_V(sem_mutex);
            sem_V(sem_empty);
            break;
        }

        message msg;
        generate_message(&msg, &seed);
        
        if (q->free_space == 0) {
            sem_V(sem_mutex);
            sem_V(sem_empty);
            break;
        }

        enqueue(q, &msg);  // просто добавляем сообщение

        int count = q->added_messages;

        sem_V(sem_mutex);  // выходим из критической секции
        sem_V(sem_fill);   // сообщаем, что появилось новое сообщение

        printf("Produced message %d: type=%d, size=%d, hash=%u\n",
               count, msg.type, msg.size, msg.hash);
        fflush(stdout);

        sleep(1);
    }
}
