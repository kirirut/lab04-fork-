#include "message.h"

uint16_t compute_hash(const Message* msg) {
    uint16_t sum = msg->type + msg->size;
    for (int i = 0; i < msg->size; i++) {
        sum += msg->data[i];
    }
    return sum;
}
