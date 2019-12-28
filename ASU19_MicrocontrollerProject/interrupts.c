#include "tm4c123gh6pm.h"

void EnableInterrupts(void)
{
    // NVIC_EN0_R = (1<<6); //uart1 6
    // NVIC_EN0_R = NVIC_EN0_INT_M;
}
void DisableInterrupts(void)
{
    // NVIC_EN0_R = 0;
}