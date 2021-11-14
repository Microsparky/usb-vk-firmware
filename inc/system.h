#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

extern volatile uint32_t system_ms;
extern volatile uint32_t system_us;

void delay_ms(uint32_t delay);
void delay_us(uint32_t delay);
void systick_setup(void (*systick_cb_setup)(void));
void clock_setup(void);

#endif // SYSTEM_H