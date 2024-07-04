/**
 * @brief CS466 Lab1 Blink proigram based on pico blink example
 * 
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define LED_PIN 25
#define SWITCH_1 16
#define SWITCH_2 17
#define OUT 18

#include "pico/stdlib.h"

void my_gpio_init(void)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(OUT);
    gpio_set_dir(OUT, GPIO_OUT);

    gpio_init(SWITCH_1);
    gpio_init(SWITCH_2);
    gpio_set_dir(SWITCH_1, GPIO_IN);
    gpio_set_dir(SWITCH_2, GPIO_IN);
    gpio_pull_up(SWITCH_1);
    gpio_pull_up(SWITCH_2);
    //gpio_set_outover(SWITCH_1, GPIO_OVERRIDE_INVERT);
    //gpio_set_outover(SWITCH_2, GPIO_OVERRIDE_INVERT);
}

bool led_control(bool isOn) {
    if (!isOn) {
        gpio_put(LED_PIN, 1);
        gpio_put(OUT, 1);
        return 1;
    } else {
        gpio_put(LED_PIN, 0);
        gpio_put(OUT, 0);
        return 0;
    }
}

int main() 
{
    my_gpio_init();
    bool isOn=0;
    int s1, s2;

    while (true) {
        // get pin states
        s1 = gpio_get(SWITCH_1);
        s2 = gpio_get(SWITCH_2);

        if (!s1 && s2) {
            // switch one down
            for (int i=0; i<20; i++) {
                isOn = led_control(isOn);
                sleep_ms(10);
                isOn = led_control(isOn);
                sleep_ms(56);
            }
        } else if (s1 && !s2) {
            // switch two down
            for (int i=0; i<10; i++) {
                isOn = led_control(isOn);
                sleep_ms(10);
                isOn = led_control(isOn);
                sleep_ms(66);
            }
        } else if (!s1 && !s2) {
            // both switches down
            while (!s1 && !s2) {
                // both are held
                isOn = led_control(isOn);
                sleep_ms(50);
                isOn = led_control(isOn);
                sleep_ms(150);
                
                s1 = gpio_get(SWITCH_1);
                s2 = gpio_get(SWITCH_2);
            }
        } else {
            // no buttons down
            isOn = led_control(isOn);
            sleep_ms(100);
            isOn = led_control(isOn);
            sleep_ms(900);
        }
    }
}
