#include "signal_utils.h"

volatile sig_atomic_t sigterm_received = 0;

void sigterm_handler(int sig) {
    sigterm_received = 1;
}
