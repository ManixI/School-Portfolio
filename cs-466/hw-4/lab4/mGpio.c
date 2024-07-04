#include <stdio.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "mGpio.h"
#include "mSpi.h"
#include "myAssert.h"


const uint8_t GPIO_INT = 13;


const uint8_t SW1_PIN = 15;
const uint8_t SW2_PIN = 14;


void mGpioInit(void) {
	// setup inputs
	// switches
	gpio_init(SW1_PIN);
    gpio_pull_up(SW1_PIN);
    gpio_set_dir(SW1_PIN, GPIO_IN);

    gpio_init(SW2_PIN);
    gpio_pull_up(SW2_PIN);
    gpio_set_dir(SW2_PIN, GPIO_IN);

    // serial data in
    gpio_init(MISO_PIN);
    gpio_pull_down(MISO_PIN);
    gpio_set_dir(MISO_PIN, GPIO_IN);

    // setup inputs
    // set default values so state is known at startup
    // clock
    gpio_init(CLK_PIN);
    gpio_set_dir(CLK_PIN, GPIO_OUT);
    gpio_put(CLK_PIN, LOW);

    // chip select
    gpio_init(CS_PIN);
    gpio_set_dir(CS_PIN, GPIO_OUT);
    // set low to read, so default high
    gpio_put(CS_PIN, HIGH);

    // serial data out
    gpio_init(MOSI_PIN);
    gpio_set_dir(MOSI_PIN, GPIO_OUT);
    gpio_put(MOSI_PIN, LOW);

    // interrupt gpio
    gpio_init(GPIO_INT);
    gpio_pull_down(GPIO_INT);
    gpio_set_dir(GPIO_INT, GPIO_IN);
    
}

void setClk(bool state) {
	// fairly sure this is being treated as a no-op rather then actually sleeping for intended time
	sleep_ms(0.1); 
	gpio_put(CLK_PIN, state);
}

void setOut(uint8_t out) {
	gpio_put(MOSI_PIN, out);
}

uint8_t getIn() {
	return gpio_get(MISO_PIN);
}

uint8_t transfer(uint8_t out) {
	uint8_t i, in=0;

	setClk(LOW);
	for(i=0; i<8; i++) {
		in <<= 1;
		setOut(out & MASK);
		setClk(HIGH);
		// another no-op like above, but solves timing issue reading as long as set to a number < 1
		sleep_ms(0.1); 
		in += getIn();
		setClk(LOW);
		out <<= 1;
	}
	//in <<= 1;
	//in +=getIn();
	setOut(0);

    //printf("in: 0x%02x\n", in);

	return (in);
}


void mGpioWriteByte(uint8_t address, uint8_t byte) {
	
	uint8_t preWrite = 0x40;

	gpio_put(CS_PIN, LOW);
	transfer(preWrite);
	transfer(address);
	transfer(byte);
	gpio_put(CS_PIN, HIGH);

	
}

uint8_t mGpioReadByte (uint8_t address) {

	uint8_t value, preRead = 0x41;

	gpio_put(CS_PIN, LOW);
	transfer(preRead);
	transfer(address);
	value = transfer(0);
	gpio_put(CS_PIN, HIGH);

	return value;
}