/*
 * easydsp_firmware.c
 *
 * Author: Perry Petiet
 * 
 * This file is the main file ran by ESP-IDF for the firmware of the EasyDSP
 * It handles the creation of all nescessary FreeRTOS tasks and the 
 * synchronisation between them. 
 * 
 * 
 */ 
/******************************* INCLUDES ********************************/

#include "dsp_control.h"
#include "device_settings.h"
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "event.h"

/******************************* GLOBAL VARIABLES ************************/

static const char *TAG = "Main";

/******************************* DEFINES *********************************/

/******************************* TASK FUNCTIONS **************************/

/* Function: settings_task 
 *
 * This function is run as a thread by FreeRTOS. This task handles the 
 * initialization of the settings of the EasyDSP. After loading the 
 * settings, it sends all settings to a given queue to other tasks.
 *
 *
 */
void settings_task(void* pvParameters)
{
    init_device_settings();

    device_settings_load_nv();

    // device_settings_load_factory();
    // device_settings_store_nv();

    //device_settings_t * settings = get_device_settings_address();

    for(;;)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void dsp_task(void* pvParameters)
{
    init_dsp_control();
    // Create a set with the queues.
    QueueHandle_t dsp_control = (QueueHandle_t)pvParameters;
    communication_event_t event;

    for(;;)
    {
        xQueueReceive(dsp_control, &event, portMAX_DELAY);
        if(event.event_type == DSP_CHANGE_EQ)
        {
            ESP_LOGI(TAG, "DSP change EQ event received!");
            dsp_control_eq_secondorder(&event.eq); 
        }
        else if(event.event_type == DSP_CHANGE_MUX)
        {
            ESP_LOGI(TAG, "DSP change mux received!");
            dsp_control_mux(&event.mux);
        }
        else
        {
            ESP_LOGW(TAG, "Unknown event received by dsp task!");
        }
    }
    vTaskDelete(NULL);
}

void memory_watcher(void* arg)
{
    for(;;)
    {
        printf("RAM left %d\n", (int)esp_get_free_heap_size());
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);   
}

/******************************** PROGRAM ENTRY **************************/

void app_main(void)
{
    QueueHandle_t dsp_control = xQueueCreate(QUEUE_SIZE, 
                                             sizeof(communication_event_t));

    xTaskCreatePinnedToCore(memory_watcher, 
                            "Memory", 
                            4096, 
                            NULL, 
                            2, 
                            NULL, 
                            tskNO_AFFINITY);

    xTaskCreatePinnedToCore(dsp_task, 
                            "DSP_handler", 
                            4096, 
                            (void *) dsp_control, 
                            2, 
                            NULL, 
                            tskNO_AFFINITY);

    xTaskCreatePinnedToCore(settings_task, 
                            "Settings_handler", 
                            4096, 
                            (void *) dsp_control, 
                            2, 
                            NULL, 
                            tskNO_AFFINITY);


    communication_event_t event;
    event.event_type            = DSP_CHANGE_MUX;
    event.mux.index             = MUX_SELECT_INPUT1;
    event.mux.sigma_dsp_address = MOD_OUTPUT1_SELECT_MONOSWSLEW_ADDR;

    xQueueSend(dsp_control, (void *)&event, (TickType_t) 10 );

}

/******************************* THE END *********************************/