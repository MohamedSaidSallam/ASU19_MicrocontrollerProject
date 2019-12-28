#include "tm4c123gh6pm.h"
#include "builtinswitchutil.h"

#define sw1 1U << 4
#define sw2 1U
#define NULL 0

void builtin_switch_init(void)
{
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
    //dummy loop
    while (!(SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R5))
    {
    }
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R = 0x01f;   // -> 0001 1111
    GPIO_PORTF_DIR_R &= ~0x11; // -> 0001 0001 // sets the 2 switches as input
    GPIO_PORTF_DEN_R |= 0x011; // -> 0001 0001 // eneables both
    GPIO_PORTF_PUR_R |= sw1;
    GPIO_PORTF_PUR_R |= sw2;
}

int isSw1Pressed()
{
    return !(GPIO_PORTF_DATA_R & sw1);
}

int isSw2Pressed()
{
    return !(GPIO_PORTF_DATA_R & sw2);
}

void (* sw1OnPress)(void) = NULL;
void (* sw2OnPress)(void) = NULL;
void (* sw12OnPress)(void) = NULL;

PressedSwitch pressedSwitch;
void do_builtin_switch_functions(void)
{
    pressedSwitch = GPIO_PORTF_DATA_R & (sw1 | sw2);
    switch (pressedSwitch)
    {
        case switch_1_pressed:
            if(sw1OnPress != NULL)
                (* sw1OnPress)();
            break;
        case switch_2_pressed:
            if(sw2OnPress != NULL)
                (* sw2OnPress)();
            break;
        case switch_12_pressed:
            if(sw12OnPress != NULL)
                (* sw12OnPress)();
            break;
        default:
        break;
    }
}


void set_sw1_function(void (* func)(void))
{
    sw1OnPress = func;
}

void set_sw2_function(void (* func)(void))
{
    sw2OnPress = func;
}

void set_sw12_function(void (* func)(void))
{
    sw12OnPress = func;
}
