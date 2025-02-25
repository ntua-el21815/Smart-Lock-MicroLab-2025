// Basic includes
#include <stdio.h>
#include <stdlib.h>
#include "esp_err.h"
#include "esp_log.h"
// Includes for FreeRTOS (The Real-Time Operating System)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "led.c"
#include "wifi.c"
#include "rfid_reader.c"
#include "accelerometer.c"

void app_main(void)
{
    // Initializations 

    enum {LOCKED, UNLOCKED} state = UNLOCKED;

    static volatile int movement = 0;

    const char* TAG = "MAIN";

    ESP_LOGI(TAG, "Starting the system");

    esp_err_t ret = nvs_flash_init();

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize NVS: %s", esp_err_to_name(ret));
        return;
    }
    
    if(connect_wifi(DEFAULT_SSID,DEFAULT_PWD) == WIFI_FAILURE){
        ESP_LOGE(TAG, "Failed to connect to wifi");
        return;
    }

    vTaskDelay(pdMS_TO_TICKS(1000));

    rc522_spi_create(&driver_config, &driver);
    rc522_driver_install(driver);
    
    rc522_config_t scanner_config = {
            .driver = driver,
    };

    rc522_create(&scanner_config, &scanner);
    rc522_register_events(scanner, RC522_EVENT_PICC_STATE_CHANGED, on_picc_state_changed, NULL);

    mpu6050_handle_t mpu6050 = mpu6050_init();  // Initialize the MPU6050 accelerometer

    acce_task_params_t acce_task_params = {
        .movement = &movement,
        .mpu6050 = mpu6050,
    };
    
    rc522_start(scanner); // start scanner
    
    TaskHandle_t accelerometer_task_handle = NULL;
    TaskHandle_t led_task_handle = NULL;

    xTaskCreate(accelerometer_task, "Accelerometer Task", 4096, &acce_task_params, 5, &accelerometer_task_handle);
    vTaskSuspend(accelerometer_task_handle);
    xTaskCreate(led_task, "LED Task", 4096, &movement, 5, &led_task_handle);
    vTaskSuspend(led_task_handle);

    initialize_io();    // Initialize the GPIO pins to be used
    
    while (1) {
        if(confirmation_lock != 0){
            if(state==LOCKED){
                vTaskSuspend(accelerometer_task_handle);
                vTaskSuspend(led_task_handle);
                gpio_set_level(RED_LED, 0);
                gpio_set_level(GREEN_LED, 0);
                ESP_LOGI("MAIN", "UNLOCKED");
                state = UNLOCKED;
            }
            else{
                ESP_LOGI("MAIN", "LOCKED");
                blink_green();  // Blink green led 5 times to indicate that the system is reactivated
                movement = 0; // Reset movement when the user wants to reactivate the system
                vTaskResume(accelerometer_task_handle);
                vTaskResume(led_task_handle);
                state = LOCKED;
            }
            confirmation_lock = 0; 
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}