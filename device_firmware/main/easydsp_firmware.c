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

void settings_task(void* pvParameters);

void dsp_task(void* pvParameters);

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

    communication_t *settingstodsp = dsp_communication_create();    

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
                            (void *) settingstodsp, 
                            2, 
                            NULL, 
                            tskNO_AFFINITY);

    xTaskCreatePinnedToCore(settings_task, 
                            "Settings_handler", 
                            4096, 
                            (void *) settingstodsp, 
                            2, 
                            NULL, 
                            tskNO_AFFINITY);
}

/******************************* THE END *********************************/