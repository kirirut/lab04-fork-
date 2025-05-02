#ifndef SIGNAL_UTILS_H
#define SIGNAL_UTILS_H

#include <signal.h>

extern volatile sig_atomic_t sigterm_received;

void sigterm_handler(int sig);

#endif
