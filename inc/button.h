#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    PLAY_PAUSE	= (1 << 10),
    BTN1      	= (1 << 7),
    BTN2    	= (1 << 8)
} button_t;

void button_setup(void);
uint32_t button_poll(void);
bool button_get(button_t);

#endif // BUTTON_H