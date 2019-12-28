#include "periodicinterruptutil.h"
#include "tm4c123gh6pm.h"

#define TIMER_SYSCTL_CLOCK SYSCTL_RCGCTIMER_R0
#define TIMER_CTL TIMER0_CTL_R
#define TIMER_CFG TIMER0_CFG_R
#define TIMER_TAMR TIMER0_TAMR_R
#define TIMER_TAILR TIMER0_TAILR_R
#define TIMER_TAPR TIMER0_TAPR_R
#define TIMER_ICR TIMER0_ICR_R
#define TIMER_IMR TIMER0_IMR_R
#define TIMER_RIS TIMER0_RIS_R
#define INTERRUPT_PRIORITY_R NVIC_PRI4_R
#define INTERRUPT_PRIORITY 0x80000000
#define INTERRUPT_ENABLE_R NVIC_EN0_R
#define INTERRUPT_ENABLE 1 << 19

void (*PeriodicTask)(void);

void periodicInterruptInit(void (*task)(void), uint32_t period)
{
    SYSCTL_RCGCTIMER_R |= TIMER_SYSCTL_CLOCK;
    PeriodicTask = task;
    TIMER_CTL = 0x0;
    TIMER_CFG = TIMER_CFG_32_BIT_TIMER;
    TIMER_TAMR = TIMER_TAMR_TAMR_PERIOD;
    TIMER_TAILR = period - 1;
    TIMER_TAPR = 0;  // !!! ?                                   // 5) bus clock resolution
    TIMER_ICR = TIMER_ICR_TATOCINT;
    TIMER_IMR = TIMER_IMR_TATOIM;
    INTERRUPT_PRIORITY_R = (INTERRUPT_PRIORITY_R & 0x00FFFFFF) | INTERRUPT_PRIORITY; // ! !!!
    INTERRUPT_ENABLE_R = INTERRUPT_ENABLE;
    //TIMER_CTL |= 0x01;
}

void Timer0A_Handler(void)
{
    TIMER_ICR = TIMER_ICR_TATOCINT;
    (*PeriodicTask)();
}

void enableTimer(){
    TIMER_CTL |= 0x01;
}

void disableTimer(){
    TIMER_CTL = 0x0;
}
