/**
 * @brief CS466 Lab4 SPI Bit-Bang
 * 
 * Copyright (c) 2022 Washington State University.
 */
// I keep needing to look this up:
// make && cp lab4.uf2 /media/ian/RPI-RP2/ && sleep 2 && screen /dev/ttyACM0

#include <stdio.h>
#include <time.h>

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "hardware/gpio.h"
#include "pico/stdlib.h"

#include "mGpio.h"
#include "mSpi.h"
#include "myAssert.h"

const uint8_t LED_PIN = 25;
//const uint8_t SW1_PIN = 17;
//const uint8_t SW2_PIN = 16;

time_t start_time;

// interrupt semaphore
static SemaphoreHandle_t _semClk = NULL;

uint32_t heartbeatDelay = 1000;  // ms
uint32_t debounceDelay = 25; // ms

uint32_t press_count = 0;

// ref: https://forum.microchip.com/s/topic/a5C3l000000MUvHEAW/t348927?comment=P-2709830
union MY_BYTE {
    uint8_t allBits;
    struct {
        uint8_t bit0    :1;
        uint8_t bit1    :1;
        uint8_t bit2    :1;
        uint8_t bit3    :1;
        uint8_t bit4    :1;
        uint8_t bit5    :1;
        uint8_t bit6    :1;
        uint8_t bit7    :1;
    };
};

void hardware_init(void)
{
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    mGpioInit();
}
//
// gpioVerifyReadWrite()
//
// This is the main function of a task that I'm using to verify that 
// my GPIO and SPI functionality is working correctly.  It woll be retired 
// as I move on to actual GPIO-Expander Functionality.
//
void gpioVerifyReadWrite(void * notUsed)
{   
    const uint32_t queryDelayMs = 100;  // ms
    uint8_t regValue;
    uint8_t count=0;

    vTaskDelay(5000);

    while (true) 
    {
        mGpioWriteByte(IODIRB, count++);
        vTaskDelay(0.1);
        regValue = mGpioReadByte(IODIRB);
        printf("IODIRB: 0x%02x,  ", regValue);
        vTaskDelay(0.2);

        regValue = mGpioReadByte(IODIRA);
        printf("IODIRA: 0x%02x,  ", regValue);
        vTaskDelay(0.2);

        regValue = mGpioReadByte(IPOLA);
        printf("IPOLA: 0x%02x\n", regValue);

        vTaskDelay(queryDelayMs);
    }
}

// IODIRB: 0x00,  IODIRA: 0xff,  IPOLA: 0x00
// IODIRB: 0x01,  IODIRA: 0xff,  IPOLA: 0x00
// IODIRB: 0x02,  IODIRA: 0xff,  IPOLA: 0x00
// IODIRB: 0x03,  IODIRA: 0xff,  IPOLA: 0x00
// IODIRB: 0x04,  IODIRA: 0xff,  IPOLA: 0x00
// IODIRB: 0x05,  IODIRA: 0xff,  IPOLA: 0x00
// IODIRB: 0x06,  IODIRA: 0xff,  IPOLA: 0x00
// etc....


// generic func to blink led
void blink_led(int on, int off, int num) {
    for(int i=0; i<num; i++) {
        gpio_put(LED_PIN, 0);
        sleep_ms(off);
        gpio_put(LED_PIN, 1);
        sleep_ms(on);
    }
}

void set_pin_a1(bool set_to) {

    union MY_BYTE state;
    state.allBits = mGpioReadByte(OLATA);
    state.allBits = 0xff;
    printf("read state: 0x%02x\n", state.allBits);
    if (set_to) {
        state.bit0 = 1;
        mGpioWriteByte(OLATA, state.allBits);
        printf("sent state: 0x%02x\n", state.allBits);
    } else {
        state.bit0 = 0;
        mGpioWriteByte(OLATA, state.allBits);
        printf("sent state: 0x%02x\n", state.allBits);
    }
    printf("-----\n");
}

void isr(uint gpio, uint32_t events_unused) {

    union MY_BYTE int_flags;
    union MY_BYTE int_state;

    int_flags.allBits = 0x00;
    int_state.allBits = 0x00;

    // stuff to print timestamp of interrupt
    time_t cur_time;

    if (gpio == 13) {
        press_count++;
        cur_time = time(NULL) - start_time;
        printf("interrupt recieved from button at %i ms since start\n", cur_time);
        int_flags.allBits = mGpioReadByte(INTFB);
        int_state.allBits = mGpioReadByte(INTCAPB);
        printf("interrupt flag register: 0x%02x\n", int_flags.allBits);
        printf("gpio state at interrupt: 0x%02x\n", int_state.allBits);
    }
}

// do this once every tic while idle
void heartbeat(void * _)
{   
    int led_on = 0;
    // setup to check button state
    union MY_BYTE state;
    state.allBits = 0xff;
    // NOTE: this write is failing for some reason
    mGpioWriteByte(OLATA, state.allBits);

    state.allBits = mGpioReadByte(GPIOA);
    printf("read state after setting 0xff: 0x%02x\n", state.allBits);

    while (true) {
        if (led_on == 0) {
            gpio_put(LED_PIN, 1);
            // turn slave chip LED on
            set_pin_a1(1);
            led_on = 1;
        } else {
            gpio_put(LED_PIN, 0);
            // turn slave chip OFF on
            set_pin_a1(0);
            led_on = 0;
        }
        
        union MY_BYTE reg_b;
        reg_b.allBits = mGpioReadByte(GPIOB);
        printf("read state of reg b: 0x%02x\n", reg_b.allBits);
        printf("current number of button presses: %i\n", press_count);
        vTaskDelay(heartbeatDelay);
    }
}

void cycle_clock(void * _) {
    vTaskDelay(100);
    while(true){
         //if(xSemaphoreTake(_semClk, portMAX_DELAY)) {
            //xSemaphoreTake(_semClk, portMAX_DELAY);
            //printf("clk: take sem\n");
            gpio_put(CS_PIN, 0);
            for (int i=0; i<8; i++) {
                setClk(0);
                setClk(1);
            }
            //sleep_ms(1);
            gpio_put(CS_PIN, 1);
            vTaskDelay(1);
            //vTaskDelay(1);
            //printf("clk: give sem\n");
            //xSemaphoreGive(_semClk);
        //}
    }
}

int main()
{
    stdio_init_all();
    printf("lab4 Hello!\n");

    sleep_ms(2000);

    hardware_init();

    xTaskCreate(heartbeat, "LED_Task", 256, NULL, 1, NULL);
    //xTaskCreate(cycle_clock, "Clock", 256, NULL, 1, NULL);
    //xTaskCreate(gpioVerifyReadWrite, "GPIO_Task", 256, NULL, 2, NULL);

    // set all pins to output
    mGpioWriteByte(IODIRA, 0x00);
    // set polarity of pins
    mGpioWriteByte(IPOLA, 0x00);

    // enable pull up resistors on all pins (only affects ones set as inputs)
    mGpioWriteByte(GPPUA, 0xff);
    mGpioWriteByte(GPPUB, 0xff);


    // set pin b1 as input
    mGpioWriteByte(IODIRB, 0x04);
    // set active low
    mGpioWriteByte(IPOLB, 0x04);
    // configure pin b1 as interrupt
    mGpioWriteByte(GPINTENB, 0x04);
    // set pin b1 to interrupt when low
    mGpioWriteByte(DEFVALB, 0x04);
    mGpioWriteByte(INTCONB, 0x04);

    gpio_set_irq_enabled_with_callback(13, GPIO_IRQ_EDGE_RISE, true, &isr);

    start_time = time(NULL);

    vTaskStartScheduler();

    while(1){};
}