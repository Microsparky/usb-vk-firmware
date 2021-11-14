// C Standard libraries
#include <stdint.h>
// #include <math.h>

// libopencm3 STM32
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
// #include <libopencm3/stm32/exti.h>
// #include <libopencm3/stm32/syscfg.h>

// libopencm3 Cortex-M
// #include <libopencm3/cm3/nvic.h>
// #include <libopencm3/cm3/systick.h>

// Includes
#include "dotstar.h"

// Configure SPI1
void spi_init(void)
{
    // Enable GPIOA clock
	rcc_periph_clock_enable(RCC_GPIOA);

	// Set pin mode for SPI managed pins to alternate function
	// // SCLK (PA5), MISO (PA6), MOSI (PA7)
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO5 | GPIO6 | GPIO7);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);
	
    // Set alternate function for SPI managed pins to AF5 for SPI1
    gpio_set_af(GPIOA, GPIO_AF5, GPIO5 | GPIO6 | GPIO7);

    // Enable SPI periperal clock
    rcc_periph_clock_enable(RCC_SPI1);

    // Initialize SPI1 as master
    spi_init_master(
        SPI1,
        SPI_CR1_BAUDRATE_FPCLK_DIV_16,		// 16MHz DIV_16: 1MHz
        SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,   	// CPOL: Clock high when idle
        SPI_CR1_CPHA_CLK_TRANSITION_2,     	// CPHA: Clock phase: read on rising edge of clock
        SPI_CR1_DFF_8BIT,
        SPI_CR1_MSBFIRST);

    spi_disable_crc(SPI1);

    // Have SPI peripheral manage NSS pin (pulled low when SPI enabled)
    // spi_enable_ss_output(SPI1);

	// Enable 5V level translators
	gpio_clear(GPIOA, GPIO4);

    spi_enable(SPI1);
}

void dotstar_set_hex(uint32_t* colour, uint8_t len)
{
	// Send the start of frame 32-bit zero
 	spi_send(SPI1, 0x0);
	spi_send(SPI1, 0x0);
	spi_send(SPI1, 0x0);
	spi_send(SPI1, 0x0);

	for(uint8_t i = 0; i < len; i++)
	{
		spi_send(SPI1, 0xFF);
		spi_send(SPI1, (colour[i] & 0xFF0000) >> 16);	// Red		(Datasheet is incorrect! (Blue, Green, Red))
		spi_send(SPI1, (colour[i] & 0x0000FF) >> 0);	// Blue
		spi_send(SPI1, (colour[i] & 0x00FF00) >> 8);	// Green
	}

	// Send end of Frame 32-bit all ones
	spi_send(SPI1, 0xFF);
	spi_send(SPI1, 0xFF);
	spi_send(SPI1, 0xFF);
	spi_send(SPI1, 0xFF);
}

uint8_t dotstar_ramp(uint8_t angle)
{
	if(angle < 128)
	{
		return 2*angle;
	}
	else
	{
		return 255 - (2*angle);
	}
}

void dotstar_animation(uint32_t* leds, uint8_t len)
{
	static uint32_t angle = 0;

	if(angle >= 255)
		angle = 0;
	else
		angle += 5;
	
	leds[0] = (dotstar_ramp((angle + 32) % 255) << 16) | (dotstar_ramp((angle + 160) % 255));
	leds[1] = (dotstar_ramp((angle + 96) % 255) << 16) | (dotstar_ramp((angle + 224) % 255));
	leds[2] = (dotstar_ramp((angle + 160) % 255) << 16) | (dotstar_ramp((angle + 32) % 255));
	leds[3] = (dotstar_ramp((angle + 224) % 255) << 16) | (dotstar_ramp((angle + 96) % 255));
}

void dotstar_set_colour(uint32_t* leds, uint8_t len, uint32_t colour)
{
	static uint32_t angle = 0;

	for(uint32_t i = 0; i < len; i++)
	{
		leds[i] = colour;
	}
}

void dotstar_breath(uint32_t* leds, uint8_t len, uint32_t colour)
{
	static int16_t amp = 32;
    static uint32_t amp_mask;
    static int8_t delta = 2;

    amp += delta;

	if(amp > 255)
    {
		amp = 255;
        delta = -delta;
    }
	else if (amp < 32)
	{
        amp = 32;
        delta = -delta;
    }
	
    amp_mask = ((uint8_t)amp << 16) | ((uint8_t)amp << 8) | ((uint8_t)amp);
	leds[0] = amp_mask & colour;
	leds[1] = amp_mask & colour;
	leds[2] = amp_mask & colour;
	leds[3] = amp_mask & colour;
}
