#ifndef DOTSTAR_H
#define DOTSTAR_H

#include <stdint.h>

void spi_init(void);
void dotstar_set_hex(uint32_t* colour, uint8_t len);
uint8_t dotstar_ramp(uint8_t angle);
void dotstar_animation(uint32_t* leds, uint8_t len);
void dotstar_set_colour(uint32_t* leds, uint8_t len, uint32_t colour);
void dotstar_breath(uint32_t* leds, uint8_t len, uint32_t colour);

#endif // DOTSTAR_H