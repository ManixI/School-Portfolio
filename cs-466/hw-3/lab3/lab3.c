/**
 * @brief CS466 Lab1 Blink proigram based on pico blink example
 * 
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// make && cp lab3.uf2 /media/ian/RPI-RP2 && sleep 2 && screen /dev/ttyACM0

#include <stdio.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "myAssert.h"

const uint8_t LED_PIN = 25;
const uint8_t SW1_PIN = 17;
const uint8_t SW2_PIN = 16;

uint32_t heartbeatDelay = 1000;  // ms
uint32_t debounceDelay = 25; // ms

// struct of message that both sets up producers and what they send
typedef struct {
    int priority;
    int mean_delay;
    char *name;
    QueueHandle_t que;
} que_msg;

void hardware_init(void)
{
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(SW1_PIN);
    gpio_pull_up(SW1_PIN);
    gpio_set_dir(SW1_PIN, GPIO_IN);

    gpio_init(SW2_PIN);
    gpio_pull_up(SW2_PIN);
    gpio_set_dir(SW2_PIN, GPIO_IN);
}

// generic func to blink led
void blink_led(int on, int off, int num) {
    for(int i=0; i<num; i++) {
        gpio_put(LED_PIN, 0);
        sleep_ms(off);
        gpio_put(LED_PIN, 1);
        sleep_ms(on);
    }
}

// do this once every tic while idle
void heartbeat(void * que_v)
{   
    QueueHandle_t que = ((que_msg*) que_v)->que;
    int count = 0;
    // setup to check button state
    while (true) {
        count++;
        printf("tick %i, SW1: %i, SW2: %i\n", count, 1-gpio_get(SW1_PIN), 1-gpio_get(SW2_PIN));
        printf("msg in que: %i\n", uxQueueMessagesWaiting(que));
        gpio_put(LED_PIN, 1);
        vTaskDelay(heartbeatDelay);
        gpio_put(LED_PIN, 0);
        vTaskDelay(heartbeatDelay);
    }
}

// produces a rand range, more clear then the math even if one liner
int rand_range(int low, int high) {
    return (rand() % high - low + 1) + low;
}

// pops from que and dose things
void consumer(void * que_v) {
    // received as pointer to pointer, need to cast to correct value
    QueueHandle_t que = ((que_msg*) que_v)->que;
    que_msg * buf = (que_msg*) malloc(sizeof(que_msg));
    buf->name = (char*) calloc(255, sizeof(char));

    while(true)
    {
        // Note: que empty because writing to null is too fast to see things added to que
        xQueueReceive(que, buf, portMAX_DELAY);
        // if not buf, the probably a time out error, expressions likely incorrect though
        if(buf) { 
            if (!gpio_get(SW1_PIN) && buf->priority == 1) {
                printf("recieved message 1\n");
                // blink 3x a second for one second
                blink_led(1000/6, 1000/6, 1);
            } else if (!gpio_get(SW2_PIN) && buf->priority == 2){
                printf("recieved message 2\n");
                blink_led(1000/12, 1000/12, 2);
            }
        } 
        taskYIELD();
    }
}

// produces items to add to que
void producer(void * que_v) {
    // received as pointer to pointer, need to cast to value
    que_msg msg = *(que_msg*)que_v;
    QueueHandle_t que = msg.que;

    while(1) {
        vTaskDelay(rand_range(0, 2 * msg.mean_delay - 1));
        // send full struct in case name or average delay is important to consumer
        xQueueSendToBack(que, &msg, 0);
        // error if que is full
        myAssert((uxQueueSpacesAvailable(que) > 0));
        taskYIELD();
    }
}


void test_assert(void * unused) {
    printf("assert task started\n");
    while(1) {
        vTaskDelay(7000);
        printf("asserting\n");
        myAssert(true);
    }
}

int main()
{
    stdio_init_all();
    sleep_ms(2000);
    printf("lab3 Hello!\n");
    hardware_init();

    QueueHandle_t que = xQueueCreate(20, sizeof(que_msg));
    myAssert(que);

    que_msg msg_arr[] = {
        {1, 95, "producer1", que},
        {2, 95, "producer2", que}
    };
    printf("%i bytes\n", sizeof(que_msg));
    printf("%i bytes\n", sizeof(&msg_arr[0]));

    xTaskCreate(heartbeat, "heartbeat", 256, (void*) &msg_arr[0], 0, NULL);
    xTaskCreate(consumer, "consumer", 256, (void*) &msg_arr[0], 2, NULL);
    xTaskCreate(producer, "producer1", 256, (void*) &msg_arr[0], 3, NULL);
    xTaskCreate(producer, "producer2", 256, (void*) &msg_arr[1], 3, NULL);
    
    vTaskStartScheduler();

    while(1){};
}