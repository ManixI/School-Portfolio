/**
 * @brief CS466 Lab1 Blink proigram based on pico blink example
 * 
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "hardware/gpio.h"
#include "pico/stdlib.h"

const uint8_t LED_PIN = 25;
const uint8_t SW1_PIN = 17;
const uint8_t SW2_PIN = 16;

uint32_t heartbeatDelay = 1000;  // ms
uint32_t debounceDelay = 25; // ms

int LED_ISFREE = 1;

int timer = 0;

static SemaphoreHandle_t _semBtn1 = NULL;
static SemaphoreHandle_t _semBtn2 = NULL;
static SemaphoreHandle_t _semRem = NULL;

void gpio_int_callback(uint gpio, uint32_t events_unused) 
{
    printf("sw1_callback: GPIO ISR %d\n", gpio);
    // on callback pass appropriate semaphore
    if (gpio == SW1_PIN) {
        xSemaphoreGiveFromISR(_semBtn1, NULL);
    } else if (gpio == SW2_PIN) {
        xSemaphoreGiveFromISR(_semBtn2, NULL);
    }
}

void hardware_init(void)
{
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(SW1_PIN);
    gpio_pull_up(SW1_PIN);
    gpio_set_dir(SW1_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(SW1_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_int_callback);

    gpio_init(SW2_PIN);
    gpio_pull_up(SW2_PIN);
    gpio_set_dir(SW2_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(SW2_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_int_callback);
}

#if 1
void sw1_handler(void * notUsed)
{
    // reset timer
    timer = 0;
    while (true)
    {
        xSemaphoreTake( _semBtn1, portMAX_DELAY);
        printf("sw1 Semaphore taken..\n");
        // flash led as per lab handout
        if (!gpio_get(SW2_PIN)) {
            printf("both\n");
            while(!gpio_get(SW2_PIN)) {
                printf("Blink!\n");
                gpio_put(LED_PIN, 0);
                sleep_ms(1000/10);
                gpio_put(LED_PIN, 1);
                sleep_ms(1000/10);
            }
        } else {
            for (int i=0; i<20; i++) {
                printf("%i ", i);
                gpio_put(LED_PIN, 0);
                sleep_ms(1000/15);
                gpio_put(LED_PIN, 1);
                sleep_ms(1000/15);
            }
            printf("\n");
        }
       

    }
}
#endif

void sw2_handler(void * notUsed)
{
    // reset timer
    timer = 0;
    while (true)
    {
        xSemaphoreTake( _semBtn2, portMAX_DELAY);
        printf("sw2 Semaphore taken..\n");
        // flash led as per lab handout
        if (!gpio_get(SW1_PIN)) {
            printf("both\n");
            while(!gpio_get(SW1_PIN)) {
                printf("Blink!\n");
                gpio_put(LED_PIN, 0);
                sleep_ms(1000/2.5);
                gpio_put(LED_PIN, 1);
                sleep_ms(1000/2.5);
            }
        } else {
            for (int i=0; i<10; i++) {
                printf("%i ", i);
                gpio_put(LED_PIN, 0);
                sleep_ms(1000/15);
                gpio_put(LED_PIN, 1);
                sleep_ms(1000/15);
            }
            printf("\n");
        }
        while(gpio_get(SW2_PIN)) {
            gpio_put(LED_PIN, 0);
            sleep_ms(1000/10);
            gpio_put(LED_PIN, 1);
            sleep_ms(1000/10);
        }
    }
}

void heartbeat(void * notUsed)
{   
    // setup to check button state
    while (true) {
        if (timer == 60) {
            printf("----------\n");
            printf("Reminder!\n");
            printf("----------\n");
            for (int i = 0; i < 20; i++){
                gpio_put(LED_PIN, 0);
                sleep_ms(1000/40);
                gpio_put(LED_PIN, 1);
                sleep_ms(1000/40);
                timer = 0;
            }
            // looks better if ends on LED off
            gpio_put(LED_PIN, 0);
            vTaskDelay(heartbeatDelay);  
        } else {
            if (LED_ISFREE) {
                printf("Heartbeat %i\n", timer);
                gpio_put(LED_PIN, 1);
                vTaskDelay(heartbeatDelay);
                timer++;
                gpio_put(LED_PIN, 0);
                vTaskDelay(heartbeatDelay);
                timer++;
            }
        }
    }

}

// reminder flash function
void reminder() {
    while(1) {
        xSemaphoreTake(_semRem, portMAX_DELAY);
        printf("----------\n");
        printf("Reminder!\n");
        printf("----------\n");
        for (int i = 0; i < 20; i++){
            gpio_put(LED_PIN, 0);
            sleep_ms(1000/40);
            gpio_put(LED_PIN, 1);
            sleep_ms(1000/40);
            timer = 0;
        }
    }
}

int main()
{
    stdio_init_all();
    printf("lab2 Hello!\n");
    hardware_init();

    _semBtn1 = xSemaphoreCreateBinary();
    _semBtn2 = xSemaphoreCreateBinary();
    _semRem = xSemaphoreCreateBinary();

    xTaskCreate(heartbeat, "LED_Task", 256, NULL, 1, NULL);
    xTaskCreate(sw1_handler, "SW1_Task", 256, NULL, 2, NULL);
    xTaskCreate(sw2_handler, "SW2_Task", 256, NULL, 2, NULL);
    xTaskCreate(reminder, "Reminder", 256, NULL, 2, NULL);

    vTaskStartScheduler();

    while(1){};
}