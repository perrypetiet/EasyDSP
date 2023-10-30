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
    printf("RAM left %d\n", (int)esp_get_free_heap_size());

    init_device_settings();
    device_settings_t * settings = get_device_settings_address();
 
    device_settings_load_nv();

    //settings->input2.eq5.filter_type = FILTER_TYPE_LOWSHELF;
    //device_settings_store_nv(); 
    //device_settings_load_nv();

    // printf("Output 3 eq1 freq: %f\n", settings->output3.eq1.freq);
    // printf("Input 1 eq1 q: %f\n", settings->input1.eq1.q);
    // printf("Input 2 eq5 type: %d\n", settings->input2.eq5.filter_type);


  
    

    
    // //SCL, SDA, and I2C interface number (0 or 1), and EEPROM address
    // init_eeprom(14, 15, 1, 0x50);

    // uint8_t write_data[5] = {1,2,3,4,5};
    // uint8_t dataStore[5] = {0,0,0,0,0};

    // uint16_t data_address = 0b0000000000000000;

    // if(eeprom_write_page(data_address, write_data, sizeof(write_data)))
    // {
    //     ESP_LOGI(TAG, "write success");
    // }
    
    // eeprom_sequential_read(data_address, dataStore, sizeof(dataStore));

    // // Display data in storage array

    // for(int i = 0; i < sizeof(dataStore); i++)
    // {
    //     printf("Data: %d\n", dataStore[i]);
    // }

    for(;;)
    {
        printf("RAM left %d\n", (int)esp_get_free_heap_size());
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}



void app_main(void)
{
    xTaskCreatePinnedToCore(task_one, "task_one", 4096, NULL, 2, NULL, 0);
    //xTaskCreatePinnedToCore(task_two, "task_two", 4096, NULL, 2, NULL, 1);
}

