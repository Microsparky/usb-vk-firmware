#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

typedef enum {
    NONE    = 0b00,
    CW      = 0b10,
    CCW     = 0b11
} direction_t;

void exti_setup(void);

void encoder_setup(void);
direction_t encoder_poll(void);

#endif // ENCODER_H