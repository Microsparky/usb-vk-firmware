// C Standard libraries
#include <stdint.h>
#include <stdbool.h>

// libopencm3 STM32
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

// Includes
#include "button.h"
#include "system.h"

#define CHECK_MS	10
#define HOLDOFF_MS	100

uint32_t next_ms;
uint32_t last_state, state;

void button_setup(void)
{
	// Enable GPIOC clock
	rcc_periph_clock_enable(RCC_GPIOC);

	// Set up GPIO button inputs as 'input floating'
	gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO7);		// BTN0
	gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO8);		// BTN1
	gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO10);	// ENSW
	gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO11);	// ENA
	gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO12);	// ENB

	// Initialise the button state
	// last_state = (GPIOD_IDR & 0xFF) | (GPIOB_IDR & 0xFF00) | ((GPIOC_IDR & 0x1D80);
	last_state = (GPIOC_IDR & 0x1D80);
}

// Button polling function - if button changes!
uint32_t button_poll(void)
{
	static uint32_t diff;

	// Read the IDR for port B, C, and D. Remap to 32-bit D[7:0], B[15:8], C[31:16]
	// state = (GPIOD_IDR & 0xFF) | (GPIOB_IDR & 0xFF00) | ((GPIOC_IDR & 0xFFFF) << 16);
	state = (GPIOC_IDR & 0x1D80); 
	
	// Return any changed bits (XOR)
	diff = state ^ last_state;

	last_state = state;
	return diff;
}

bool button_get(button_t mask){
	return (bool)(state & mask);
}