#ifndef PERIODIC_INTERRUPT_UTIL
#define PERIODIC_INTERRUPT_UTIL

#include <stdint.h>

void periodicInterruptInit(void (*task)(void), uint32_t period);
void enableTimer();
void disableTimer();

#endif // PERIODIC_INTERRUPT_UTIL