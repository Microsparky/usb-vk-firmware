// C Standard libraries
#include <stdint.h>

// libopencm3 STM32
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/flash.h>

// libopencm3 Cortex-M
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

// Includes
#include "system.h"

#define MCO_ENABLE 0

// const struct rcc_clock_scale rcc_hse_8mhz = {
// 		.pllm = 4,
// 		.plln = 96,
// 		.pllp = 6,
// 		.pllq = 4,
// 		.pllr = 0,
// 		.pll_source = RCC_CFGR_PLLSRC_HSE_CLK,
// 		.hpre = RCC_CFGR_HPRE_DIV_2,
// 		.ppre1 = RCC_CFGR_PPRE_DIV_4,
// 		.ppre2 = RCC_CFGR_PPRE_DIV_2,
// 		.voltage_scale = PWR_SCALE1,
// 		.flash_config = FLASH_ACR_LATENCY_0WS,
// 		.ahb_frequency  = 16000000,
// 		.apb1_frequency = 4000000,
// 		.apb2_frequency = 8000000,
// 	};

// Globals
volatile uint32_t system_ms;
volatile uint32_t system_us;

// Function pointer for task-execute/scheduler function 
void (*systick_cb)(void);

// Called by systick interrupt 
void sys_tick_handler(void)
{
	// 100us interrupt
	system_us += 100;

	// system ms counter
	if((system_us % 1000) == 0){
		system_ms++;

		// systick call to basic task-execute/scheduler function 
		systick_cb();
	}
}

// Delay milliseconds
void delay_ms(uint32_t delay)
{
	uint32_t return_us = system_us + 10*delay;
	while (return_us > system_us);
}

// Delay microseconds
void delay_us(uint32_t delay)
{
	uint32_t return_us = system_us + delay;
	while (return_us > system_us);
}

// This function sets up the 1khz "system tick" count. The SYSTICK counter is a
// standard feature of the Cortex-M series.
void systick_setup(void (*systick_cb_setup)(void))
{
	// On reset the 16 MHz internal RC oscillator is selected as the default CPU clock.
	// clock rate / 10000 to get 1mS interrupt rate
	systick_set_reload(8400);
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_counter_enable();
	
	// Register systick_cb callback
	systick_cb = systick_cb_setup;

	// Enable the interrupt
	systick_interrupt_enable();
}

// Configure the STM32 system clock
void clock_setup(void)
{
 	// RCC Enable Bypass
	// Enable an external clock to bypass the internal clock (high speed and low speed clocks only).
	// The external clock must be enabled (see rcc_osc_on) and the internal clock must be disabled 
	// (see rcc_osc_off) for this to have effect.
	rcc_osc_bypass_enable(RCC_HSE);

	// Enable HSE and disable HSI
	rcc_osc_on(RCC_HSE);
	rcc_wait_for_osc_ready(RCC_HSE);

	//rcc_osc_off(RCC_HSI);

	// Configure the PLL with HSE as source
	rcc_clock_setup_pll(&rcc_hse_16mhz_3v3[RCC_CLOCK_3V3_84MHZ]);
	

#if MCO_ENABLE
	// Check clock output on MCO1
	// 00: HSI clock selected
	// 01: LSE oscillator selected
	// 10: HSE oscillator clock selected
	// 11: PLL clock selected
	rcc_set_mco(0b11);

	// Set the MCO1PRE: MCO1 prescaler... manually
	// 0xx: no division
	// 100: division by 2
	// 101: division by 3
	// 110: division by 4
	// 111: division by 5
	RCC_CFGR = (RCC_CFGR & ~(0x7 << 24)) | (0b111 << 24);

	// Enable GPIOA clocks
	rcc_periph_clock_enable(RCC_GPIOA);

	// Set PA8 to AF0
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);
	gpio_set_af(GPIOA, GPIO_AF0, GPIO8);
#endif // MCO_ENABLE
}