// Basic includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"

// Includes for RFID scanner
#include "rc522.h"
#include "driver/rc522_spi.h"
#include "rc522_picc.h"

// Definitions for the RFID scanner
#define RC522_SPI_BUS_GPIO_MISO    GPIO_NUM_21
#define RC522_SPI_BUS_GPIO_MOSI    GPIO_NUM_20
#define RC522_SPI_BUS_GPIO_SCLK    GPIO_NUM_19
#define RC522_SPI_SCANNER_GPIO_SDA GPIO_NUM_18
#define RC522_SCANNER_GPIO_RST     (-1) // soft-reset

static rc522_driver_handle_t driver;
static rc522_handle_t scanner;

// Variable to track whether the system is locked or unlocked
static volatile int confirmation_lock = 0;

static rc522_spi_config_t driver_config = {
    .host_id = SPI2_HOST,
    .bus_config = &(spi_bus_config_t){
        .miso_io_num = RC522_SPI_BUS_GPIO_MISO,
        .mosi_io_num = RC522_SPI_BUS_GPIO_MOSI,
        .sclk_io_num = RC522_SPI_BUS_GPIO_SCLK,
    },
    .dev_config = {
        .spics_io_num = RC522_SPI_SCANNER_GPIO_SDA,
    },
    .rst_io_num = RC522_SCANNER_GPIO_RST,
};


static void on_picc_state_changed(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    const char *TAG = "RFID READER";
    rc522_picc_state_changed_event_t *event = (rc522_picc_state_changed_event_t *)data;
    rc522_picc_t *picc = event->picc;
    if (picc->state == RC522_PICC_STATE_ACTIVE) {
        char uid_s[RC522_PICC_UID_STR_BUFFER_SIZE_MAX];
        char uid_card[]="39 25 CF 53";
        char uid_tag[]="23 E9 84 1D";
        
        confirmation_lock=0;
        char uid_phone[]="01 02 03 04";
        esp_err_t err = rc522_picc_uid_to_str(&picc->uid, uid_s, sizeof(uid_s));
        
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "UID: %s ", uid_s);
        } else {
            ESP_LOGE(TAG, "Failed to convert UID to string");
        }


        if (strcmp(uid_s, uid_card) == 0) {
            ESP_LOGI(TAG, "CARD!!!");
            confirmation_lock=1;  
        }
        if (strcmp(uid_s, uid_tag) == 0) {
            ESP_LOGI(TAG, "TAG!!!");
            confirmation_lock=2;
        }
        if (strcmp(uid_s, uid_phone) == 0) {
            confirmation_lock=3;
            ESP_LOGI(TAG, "This is my phone !");
        }  
        if((strcmp(uid_s, uid_card) != 0)&&(confirmation_lock==0)){
            ESP_LOGI(TAG, "!!!THIEF!!!");
           // ESP_LOGI(TAG, "UID: %s ", uid_s);
        }
    }

    
    else if (picc->state == RC522_PICC_STATE_IDLE && event->old_state >= RC522_PICC_STATE_ACTIVE) {
        ESP_LOGI(TAG, "Card has been removed");
    }
}