// USB-VK - The USB Volume Knob
//
// Peter Keogh
// 2021-07

// C Standard libraries
#include <stdint.h>
#include <stdbool.h>

// TODO Remove these
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

// Includes
#include "system.h"
#include "button.h"
#include "dotstar.h"
#include "encoder.h"
#include "console.h"
#include "usb_hid_consumer.h"

uint32_t colours[8] = {	0x000000,
						0x0000FF,
						0x00FF00,
						0x00FFFF,
						0xFF0000,
						0xFF00FF,
						0xFFFF00,
						0xFFFFFF
						};

volatile uint8_t colour_mode = 5;

// Report (Consumer Page)
// B3 	Fastforward		OOC		On/Off Control
// B4	Rewind			OOC
// B5	Scan Next		OSC
// B6	Scan Previous	OSC
// B7	Stop			OSC		One Shot Control
// CD	Play/Pause		OSC
// E2	Mute			OOC
// E9	Volume Up		RTC		Re-trigger Control
// EA	Volume Down		RTC
volatile uint16_t report_buf[2] = {0, 0};
volatile uint16_t *report;

volatile direction_t encoder_dir;

// Called by systick interrupt at 1ms interval
void system_execute(void){
	
	// Reset the report
	report_buf[0] = 0;
	report_buf[1] = 0;
	report = report_buf;

	// Check the encoder
	encoder_dir = encoder_poll();
	if(encoder_dir == CW){
		*report = 0xe9;	// Volume Up
		report++;
	}
	else if(encoder_dir == CCW){
		*report = 0xea;	// Volume Down
		report++;
	}

	// Check the button
	if(button_poll()){
		if(!button_get(PLAY_PAUSE)){
			*report = 0xcd;
			report++;
		}
		if(!button_get(BTN1)){
			*report = 0xb5;
			report++;
		}
		if(!button_get(BTN2)){
			*report = 0xb6;
			report++;
		}
	}

	// USB poll every 1ms
	usb_report(report_buf);
	usb_poll();

}

int main(void) {
	
	uint32_t leds[7];
	uint32_t colour;

	clock_setup();
	button_setup();
	spi_init();
	encoder_setup();
	console_init();
	usb_setup();
	systick_setup(system_execute);

	// console_print("Program hardware to do silly things!\r\n");

	// colour = colours[colour_mode];
	colour = 0x660066;
	dotstar_set_colour(leds, 6, colour);
	dotstar_set_hex(leds, 6);

	// // PC09
	// rcc_periph_clock_enable(RCC_GPIOC);
	// gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO9);
	// gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO7);
	// gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO8);
	// gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO10);
	// gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO11);
	// gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO12);

	// rcc_periph_clock_enable(RCC_GPIOA);
	// gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);
	// gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);
	// gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);

	while(1)
	{
	// 	button_poll();
	// 	if(!button_get(PLAY_PAUSE)){
	// 		gpio_toggle(GPIOC, GPIO9);
	// 	}
	// 	if(!button_get(BTN1)){
	// 		gpio_toggle(GPIOC, GPIO9);
	// 	}
	// 	if(!button_get(BTN2)){
	// 		gpio_toggle(GPIOC, GPIO9);
	// 	}

	// 	encoder_dir = encoder_poll();
	// 	if(encoder_dir == CW){
	// 		colour = colours[colour_mode++%8];
	// 	}
	// 	if(encoder_dir == CCW){
	// 		colour = colours[colour_mode--%8];
	// 	}
	// 	dotstar_set_colour(leds, 6, colour);
	// 	dotstar_set_hex(leds, 6);

	// 	for (uint32_t i = 0; i < 1000000; i++) {
	// 		__asm__("nop");
	// 	}
	}
}
