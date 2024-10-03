#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "event.h"
#include "ble.h"
#include "buffer.h"
#include "event.h"

/*
Tasks initialises the supported interfaces 
*/
static const char *TAG = "Interfaces task";

void task_interfaces(void* pvParameters)
{
    communication_t* toSettings = (communication_t*)pvParameters;
    uint8_t device_name[] = "easydsp";

    // Initialize the BLE, it will run as a task on its own.
    // The buffer is used for the serial ble options, not currently
    // operational.

    init_ble(device_name, toSettings);
    
    dsp_event_t          event;
    dsp_event_response_t event_response;

    for(;;)
    {
        // vTaskDelay(10 / portTICK_PERIOD_MS);
        // if(send_event(toSettings, 
        //               &event, 
        //               &event_response, 
        //               EVENT_STD_TIMEOUT_TICKS))
        // {
        //     if(event_response.response_event_type != EVENT_RESPONSE_OK)
        //     {
        //         ESP_LOGW(TAG, "Response to event is not OK");
        //     }
        //     else
        //     {
        //         ESP_LOGI(TAG, "Response received from settings!");
        //     }
        // }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}