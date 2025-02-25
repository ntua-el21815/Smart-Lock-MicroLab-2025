// Basic includes
#include <stdio.h>
#include <stdlib.h>
#include "esp_err.h"
#include "esp_log.h"

// Includes for accelerometer
#include "mpu6050.h"
#include "math.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Definitions for the MPU6050
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_MASTER_SCL_IO GPIO_NUM_7      /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO GPIO_NUM_6     /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_0  /*!< I2C port number for master dev */

// Definitions for MPU6050 thresholds for movement detection
#define THRESHOLD_X 0.25
#define THRESHOLD_Y 0.25
#define THRESHOLD_Z 0.25

static const char message[] = "Movement detected";

typedef struct {
    int* movement;
    mpu6050_handle_t mpu6050;
} acce_task_params_t;

mpu6050_acce_value_t acceleration;

// Definitions for HTTP POST request
#define DOMAIN "ntfy.sh"
#define PORT 80
#define PATH "/nik_bike_lock"


// Function for sending a POST request to a server
esp_err_t http_post(const char *dom, char *message, const char *path, int port)
{
    esp_http_client_config_t config = {
        .host = dom,
        .port = port,
        .path = path,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 1000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_header(client, "Content-Type", "application/json");

    char post_data[100];
    snprintf(post_data, sizeof(post_data), "{\"message\": \"%s\"}", message);

    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK)
    {
        ESP_LOGI("HTTP", "HTTP POST Status = %d, content_length = %d",
                 esp_http_client_get_status_code(client),
                 (int)esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE("HTTP", "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);

    return err;
}

int movement_detected(mpu6050_handle_t mpu6050){
    mpu6050_gyro_value_t gyro;
    mpu6050_temp_value_t temp;

    mpu6050_acce_value_t prev_acce = acceleration;

    ESP_ERROR_CHECK(mpu6050_get_acce(mpu6050, &acceleration));

    ESP_ERROR_CHECK(mpu6050_get_gyro(mpu6050, &gyro));

    ESP_ERROR_CHECK(mpu6050_get_temp(mpu6050, &temp));

    // Prints were there for debugging purposes

    //ESP_LOGI("MPU6050", "Acce: X=%.2f Y=%.2f Z=%.2f", acceleration.acce_x, acceleration.acce_y, acceleration.acce_z);
    //ESP_LOGI("MPU6050", "Gyro: X=%.2f Y=%.2f Z=%.2f", gyro.gyro_x, gyro.gyro_y, gyro.gyro_z);
    //ESP_LOGI("MPU6050", "Temp: %.2f", temp.temp);

    if (fabs(acceleration.acce_x - prev_acce.acce_x) > THRESHOLD_X || fabs(acceleration.acce_y - prev_acce.acce_y) > THRESHOLD_Y || fabs(acceleration.acce_z - prev_acce.acce_z) > THRESHOLD_Z) {
        return 1;
    }
    return 0;
}

void accelerometer_task(void *pvParameters){

    acce_task_params_t* params = (acce_task_params_t*) pvParameters;

    int* movement = params->movement;

    mpu6050_handle_t mpu6050 = params->mpu6050;

    if (mpu6050 == NULL) {
        ESP_LOGE("MPU6050", "MPU6050 init failed");
        return;
    }

    while (1) {
        *movement = movement_detected(mpu6050);
        if (*movement == 1) {
            if(http_post(DOMAIN, message, PATH, PORT) == ESP_OK){
                ESP_LOGI("HTTP", "POST request sent");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

static mpu6050_handle_t mpu6050_init(void)
{
    mpu6050_handle_t mpu6050;

    // Configure I2C
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &i2c_conf));

    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER, 0, 0, 0));

    // Initialize MPU6050
    mpu6050 = mpu6050_create(I2C_MASTER_NUM, MPU6050_I2C_ADDRESS);
    if (mpu6050 == NULL)
    {
        ESP_LOGE("MPU6050", "MPU6050 create failed");
        return NULL;
    }

    ESP_ERROR_CHECK(mpu6050_wake_up(mpu6050));

    ESP_ERROR_CHECK(mpu6050_config(mpu6050, ACCE_FS_2G, GYRO_FS_250DPS));

    return mpu6050;
}