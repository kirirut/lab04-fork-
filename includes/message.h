#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

#define MAX_DATA_SIZE 256

typedef struct {
    uint8_t type;
    uint16_t hash;
    uint8_t size;
    uint8_t data[MAX_DATA_SIZE];
} Message;

uint16_t compute_hash(const Message* msg);

#endif