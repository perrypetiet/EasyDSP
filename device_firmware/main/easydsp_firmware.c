#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "device_settings.h"
#include "sigma_dsp.h"
#include "esp_err.h"
#include "driver/gpio.h"

static const char *TAG = "Main";

void settings_task(void* arg)
{
    printf("RAM left %d\n", (int)esp_get_free_heap_size());

    init_device_settings();
    device_settings_t * settings = get_device_settings_address();
 
    device_settings_load_nv();

    printf("Output 3 eq1 freq: %f\n", settings->output3.eq1.freq);
    printf("Input 1 eq1 q: %f\n", settings->input1.eq1.q);
    printf("Input 2 eq5 type: %d\n", settings->input2.eq5.filter_type);

    for(;;)
    {
        printf("RAM left %d\n", (int)esp_get_free_heap_size());
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void dsp_task(void* arg)
{

    init_sigma_dsp(12, 13, 0, (0x68 >> 1) & 0xFE, GPIO_NUM_16);
    for(;;)
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}


void app_main(void)
{
    xTaskCreatePinnedToCore(dsp_task, "DSP_handler", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(settings_task, "Settings_handler", 4096, NULL, 2, NULL, 0);
}

