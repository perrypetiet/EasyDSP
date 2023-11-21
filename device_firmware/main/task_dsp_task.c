#include "dsp_control.h"
#include "device_settings.h"
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "event.h"

static const char *TAG = "DSP task";

/* Function: dsp_task 
 *
 * This function runs as a freeRTOS task. It handles everything related
 * to the DSP module. It initialises the DSP module and loads the program
 * onto the Analog Devices chip. The EQ's and Muxes can be changed by 
 * sending an event to the task using a given communication type.
 *
 * Events are described in "event.h".
 *
 */
void dsp_task(void* pvParameters)
{
    init_dsp_control();

    communication_t      *communication = (communication_t*)pvParameters;
    dsp_event_t          event;
    dsp_event_response_t event_response;
    
    for(;;)
    {
        event_response.response_event_type = EVENT_RESPONSE_ERROR;

        if(await_event(communication, &event, EVENT_STD_TIMEOUT_TICKS))
        {
            if(event.event_type == DSP_SET_EQ)
            {
                if(dsp_control_eq_secondorder(&event.eq))
                {
                    event_response.response_event_type = EVENT_RESPONSE_OK;
                }
                else
                {
                    event_response.response_event_type = EVENT_RESPONSE_DSP_ERROR;
                }
            }
            else if(event.event_type == DSP_SET_MUX)
            {
                if(dsp_control_mux(&event.mux))
                {
                    event_response.response_event_type = EVENT_RESPONSE_OK;
                }
                else
                {
                    event_response.response_event_type = EVENT_RESPONSE_DSP_ERROR;
                }
            }
            
            if(!send_event_response(communication, 
                                    &event_response, 
                                    EVENT_STD_TIMEOUT_TICKS))
            {
                ESP_LOGE(TAG, "Unable to put event response in response queue!");
            }
        }
    }
    vTaskDelete(NULL);
}