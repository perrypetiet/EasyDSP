#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "device_settings.h"
#include "eeprom.h"
#include "esp_err.h"

static const char *TAG = "Main";

void task_one(void* arg)
{
    // SCL, SDA, and I2C interface number (0 or 1), and EEPROM address
    init_eeprom(14, 15, 1, 0x50);

    uint8_t dataStore[3] = {0,0,0};

    uint16_t data_address = 0b0000000000000000;

    //eeprom_write_random_byte(data_address, 69);

    //vTaskDelay(1000 / portTICK_PERIOD_MS);

    eeprom_sequential_read(data_address, dataStore, sizeof(dataStore));

    // Display data in storage array

    for(int i = 0; i < sizeof(dataStore); i++)
    {
        printf("Data: %d\n", dataStore[i]);
    }

    for(;;)
    {
        //printf("task one running\n");
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}



void app_main(void)
{
    xTaskCreatePinnedToCore(task_one, "task_one", 4096, NULL, 2, NULL, 0);
    //xTaskCreatePinnedToCore(task_two, "task_two", 4096, NULL, 2, NULL, 1);
}

