#ifndef __KEY_H
#define __KEY_H
#include "bsp_system.h"

#define Polling 1
#define External_Interrupt 0

#define GPIO_KEY_PORT1 GPIOH
#define GPIO_KEY_PORT2 GPIOH
#define GPIO_KEY_PORT3 GPIOC


#define GPIO_KEY_PIN1 GPIO_PIN_3
#define GPIO_KEY_PIN2 GPIO_PIN_2
#define GPIO_KEY_PIN3 GPIO_PIN_13


extern uint8_t Key_Down;
uint8_t Key_Read(void);
void Key_proc(void);





#endif
