#include "dsp_control.h"
#include "device_settings.h"
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "event.h"

static const char *TAG = "Settings task";

/* Function: settings_task 
 *
 * This function is run as a thread by FreeRTOS. This task handles the 
 * initialization of the settings of the EasyDSP. After loading the 
 * settings, it sends all settings to a given queue to other tasks.
 *
 */
void settings_task(void* pvParameters)
{
    init_device_settings();
    device_settings_load_nv();

    //device_settings_load_factory();
    //device_settings_store_nv();
    device_settings_t * settings = get_device_settings_address();

    dsp_event_t          event;
    dsp_event_response_t event_response;
    event_response.response_event_type = EVENT_RESPONSE_ERROR;
    communication_t *communication = (communication_t*)pvParameters;

    // After initializing the settings, let send all the settings to the
    // DSP task using our communication
    for(int i = 0; i < DEVICE_SETTINGS_INPUT_AMOUNT; i++)
    {
        for(int j = 0; j < DEVICE_SETTINGS_INPUT_EQ_AMOUNT; j++)
        {
            event.event_type = DSP_SET_EQ;
            event.eq = settings->inputs[i].eq[j];

            if(send_event(communication, 
                          &event, 
                          &event_response, 
                          EVENT_STD_TIMEOUT_TICKS))
            {
                if(event_response.response_event_type != EVENT_RESPONSE_OK)
                {
                    ESP_LOGW(TAG, "Response to event is not OK");
                }
            }
        }
    }
    for(int i = 0; i < DEVICE_SETTINGS_OUTPUT_AMOUNT; i++)
    {
        event.event_type = DSP_SET_MUX;
        event.mux = settings->outputs[i].mux;
        if(send_event(communication, 
                      &event, 
                      &event_response, 
                      EVENT_STD_TIMEOUT_TICKS))
        {
            if(event_response.response_event_type != EVENT_RESPONSE_OK)
            {
                ESP_LOGW(TAG, "Response to event is not OK");
            }
        }
        for(int j = 0; j < DEVICE_SETTINGS_OUTPUT_EQ_AMOUNT; j++)
        {
            event.event_type = DSP_SET_EQ;
            event.eq = settings->outputs[i].eq[j];
            if(send_event(communication, 
                      &event, 
                      &event_response, 
                      EVENT_STD_TIMEOUT_TICKS))
            {
                if(event_response.response_event_type != EVENT_RESPONSE_OK)
                {
                    ESP_LOGW(TAG, "Response to event is not OK");
                }
            }
        }
    }  


    // Infinite loop, waits for event from command interface.
    for(;;)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}