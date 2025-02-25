// For http post request
#include "esp_http_client.h"

// Basic includes
#include <stdio.h>
#include <stdlib.h>
#include "esp_err.h"
#include "esp_log.h"

// Includes for FreeRTOS (The Real-Time Operating System)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Includes for ESP-IDF and GPIO
#include "driver/gpio.h"

// Definitions for LED, Buzzer and Button
#define RED_LED GPIO_NUM_1
#define GREEN_LED GPIO_NUM_0
#define BUZZER GPIO_NUM_10

// Blinks Green led 5 times in a second
void blink_green(){
    for(int i = 0; i < 5; i++){
        gpio_set_level(GREEN_LED, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_set_level(GREEN_LED, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    return; 
}

void led_task(void *pvParameters){

    int* movement = (int*) pvParameters;

    while (1) {
        if (*movement) {
            gpio_set_level(RED_LED, 1);
            gpio_set_level(GREEN_LED, 0);
            gpio_set_level(BUZZER, 1);
            for(int i=0 ; i<15 ; i++){
                gpio_set_level(BUZZER, 1);
                vTaskDelay(pdMS_TO_TICKS(100));
                gpio_set_level(BUZZER, 0);
                vTaskDelay(pdMS_TO_TICKS(100));
            };
            vTaskDelay(pdMS_TO_TICKS(2000));
        } else {
            gpio_set_level(RED_LED, 0);
            gpio_set_level(GREEN_LED, 1);
            gpio_set_level(BUZZER, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void initialize_io(){
    gpio_set_direction(RED_LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(GREEN_LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);

    gpio_pullup_en(RED_LED);
    gpio_pullup_en(GREEN_LED);
    gpio_pullup_en(BUZZER);

    gpio_set_level(RED_LED, 0);
    gpio_set_level(GREEN_LED, 0);
    gpio_set_level(BUZZER, 0);
}