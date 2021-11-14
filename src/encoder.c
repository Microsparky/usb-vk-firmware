// C Standard libraries
#include <stdint.h>
// #include <math.h>

// libopencm3 STM32
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
// #include <libopencm3/stm32/spi.h>
// #include <libopencm3/stm32/exti.h>
// #include <libopencm3/stm32/syscfg.h>

// libopencm3 Cortex-M
// #include <libopencm3/cm3/nvic.h>
// #include <libopencm3/cm3/systick.h>

// Includes
#include "encoder.h"

#define ENCODER_VAL() ((GPIOC_IDR >> 11) & 0b11)

// Encoder reverse look up table
// CW:	00 -> 01 -> 11 -> 10 -> 00
// CCW:	00 -> 10 -> 11 -> 01 -> 00
direction_t encoder_direction[16] = {
			// Index   Last    Next    Increment   Direction
	NONE,	// 0000    00      00      0           invalid
	CW,		// 0001    00      01      1           CW			
	CCW,	// 0010    00      10      -1          CCW			
	NONE,	// 0011    00      11      0           invalid
	CCW,	// 0100    01      00      -1          CCW
	NONE,	// 0101    01      01      0           invalid
	NONE,	// 0110    01      10      0           invalid
	CW,		// 0111    01      11      1           CW
	CW,		// 1000    10      00      1           Cw
	NONE,	// 1001    10      01      0           invalid
	NONE,	// 1010    10      10      0           invalid
	CCW,	// 1011    10      11      -1          CCW
	NONE,	// 1100    11      00      0           invalid
	CCW,	// 1101    11      01      -1          CCW
	CW,		// 1110    11      10      1           CW
	NONE,	// 1111    11      11      0           invalid
};

volatile uint8_t encoder_state;

void encoder_setup(void)
{
	// Enable GPIO clock
	rcc_periph_clock_enable(RCC_GPIOC);

	// Configure GPIO as input
	gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO11);
	gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO12);

	// Initialise encoder state (GPIO 14 and 15)
	encoder_state = ENCODER_VAL();
}

// Encoder polling function 
// Returns encoder direction, must be called fast enough to catch all transitions (pulses 3ms)
direction_t encoder_poll(void)
{
	// Set the encoder state: last-state[4:3], current-state[2:0]
	encoder_state = ((encoder_state << 2) & 0b1100) | ENCODER_VAL();
	
	return encoder_direction[encoder_state];
}