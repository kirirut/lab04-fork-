#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H
#include "message.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

typedef struct {
    int head;
    int tail;
    int added_messages;
    int removed_messages;
    int free_space;
    int queue_size;
    volatile sig_atomic_t run;
    message *buffer;
} message_queue;


void init_queue(message_queue* q, int queue_size);
void enqueue(message_queue* q, const message* msg);
message dequeue(message_queue* q);
void print_queue_state(message_queue* q);
void destroy_queue(message_queue* q);


#endif 