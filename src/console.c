// C Standard libraries
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// libopencm3 STM32
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

// Includes
#include "console.h"

void console_init(void)
{
	// Enable clocks for USART3 (STLINK)
	rcc_periph_clock_enable(RCC_USART1);
    
    // Enable GPIO clock
	rcc_periph_clock_enable(RCC_GPIOB);

    // Set PD8, PD9 as alternate function AF7 for UART3
    gpio_set_af(GPIOB, GPIO_AF7, GPIO6 | GPIO7);

    // Set pin mode PD8, PD9 as alternate function AF7
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6 | GPIO7);

	// Setup USART3 parameters.
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX_RX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	// Finally enable the USART
	usart_enable(USART1);
}

void console_print(uint8_t* str)
{
    static uint8_t len;
    len = strlen(str);
    
    for(uint8_t i=0; i<len; i++)
    {
        usart_send_blocking(USART1, str[i]);
    }
}