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
    //device_settings_load_nv();

    device_settings_load_factory();

    device_settings_t * settings = get_device_settings_address();

    dsp_event_t          event;
    dsp_event_response_t event_response;
    event_response.response_event_type = EVENT_RESPONSE_ERROR;

    settings_task_communications_t* queues = (settings_task_communications_t*)pvParameters;

    communication_t* communicationDsp        = queues->settings_dsp;
    communication_t* communicationInterfaces = queues->settings_interfaces;

    // After initializing the settings, let send all the settings to the
    // DSP task using our communication
    for(int i = 0; i < DEVICE_SETTINGS_INPUT_AMOUNT; i++)
    {
        for(int j = 0; j < DEVICE_SETTINGS_INPUT_EQ_AMOUNT; j++)
        {
            event.event_type = DSP_SET_EQ;
            event.eq = settings->inputs[i].eq[j];

            if(send_event(communicationDsp, 
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
        if(send_event(communicationDsp, 
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
            if(send_event(communicationDsp, 
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
        if(await_event(communicationInterfaces, &event, EVENT_STD_TIMEOUT_TICKS))
        {
            ESP_LOGI(TAG, "Received event!!");
            event_response.response_event_type = EVENT_RESPONSE_ERROR;

            /* BLE REQUESTING EQ DATA */
            if(event.event_type == DSP_GET_EQ)
            {
                uint8_t channelNum = event.chan_num;
                uint8_t eqNum      = event.eq_num;
                bool    output     = event.output;
                
                if(output)
                {
                    if((channelNum < DEVICE_SETTINGS_OUTPUT_AMOUNT) &&
                       (eqNum      < DEVICE_SETTINGS_OUTPUT_EQ_AMOUNT))
                    {
                        event_response.response_eq = settings->outputs[channelNum].eq[eqNum];
                        event_response.response_event_type = EVENT_RESPONSE_OK;
                    }
                }
                else
                {
                    if((channelNum < DEVICE_SETTINGS_INPUT_AMOUNT) &&
                       (eqNum      < DEVICE_SETTINGS_INPUT_EQ_AMOUNT))
                    {
                        event_response.response_eq = settings->inputs[channelNum].eq[eqNum];
                        event_response.response_event_type = EVENT_RESPONSE_OK;
                    }
                }
            }

            /* BLE REQUESTING MUX DATA */
            if(event.event_type == DSP_GET_MUX)
            {
                 uint8_t channelNum = event.chan_num;
                 bool    output     = event.output;
                 if(output && (channelNum < DEVICE_SETTINGS_OUTPUT_AMOUNT))
                 {
                    event_response.response_mux = settings->outputs[channelNum].mux;
                    event_response.response_event_type = EVENT_RESPONSE_OK;
                 }
            }

            /* BLE SETTING EQ DATA */
            if(event.event_type == DSP_SET_EQ)
            {
                bool settingsUpdated = false;
                // Copy from event the the new channel EQ ->
                // Send the new eq data in settings to dsp task ->
                uint8_t channelNum = event.chan_num;
                uint8_t eqNum      = event.eq_num;
                bool    output     = event.output;
                if(output)
                {
                    if(channelNum < DEVICE_SETTINGS_OUTPUT_AMOUNT)
                    {
                        uint16_t old_address = settings->outputs[channelNum].eq[eqNum].sigma_dsp_address;
                        event.eq.sigma_dsp_address = old_address;
                        settings->outputs[channelNum].eq[eqNum] = event.eq;
                        settings->outputs[channelNum].eq[eqNum].sigma_dsp_address = old_address;
                        settingsUpdated = true;
                    }
                }
                else
                {
                    if(channelNum < DEVICE_SETTINGS_INPUT_AMOUNT)
                    {
                        uint16_t old_address = settings->inputs[channelNum].eq[eqNum].sigma_dsp_address;
                        event.eq.sigma_dsp_address = old_address;
                        settings->inputs[channelNum].eq[eqNum] = event.eq;
                        settings->inputs[channelNum].eq[eqNum].sigma_dsp_address = old_address;
                        settingsUpdated = true;
                    }
                }
                if(settingsUpdated)
                {
                    // Event is now the same as stored in settings, send to dsp
                    event.event_type = DSP_SET_EQ;
                    if(send_event(communicationDsp, 
                       &event, 
                       &event_response, 
                       EVENT_STD_TIMEOUT_TICKS))
                    {
                        event_response.response_event_type = EVENT_RESPONSE_OK;       
                    }
                }
            }

            /* BLE SETTING MUX DATA */
            if(event.event_type == DSP_SET_MUX)
            {
                // Copy from event the the new channel MUX ->
                // Send the new mux data in settings to dsp task ->
                uint8_t channelNum = event.chan_num;
                bool    output     = event.output;
                if(output && channelNum < DEVICE_SETTINGS_OUTPUT_AMOUNT)
                {
                    uint16_t old_address = settings->outputs[channelNum].mux.sigma_dsp_address;
                    event.mux.sigma_dsp_address = old_address;
                    settings->outputs[channelNum].mux = event.mux;
                    settings->outputs[channelNum].mux.sigma_dsp_address = old_address;
                }
                event.event_type = DSP_SET_MUX;
                if(send_event(communicationDsp, 
                              &event, 
                              &event_response, 
                              EVENT_STD_TIMEOUT_TICKS))
                {
                    event_response.response_event_type = EVENT_RESPONSE_OK;       
                }
            }

            send_event_response(communicationInterfaces, 
                                &event_response, 
                                EVENT_STD_TIMEOUT_TICKS);
        }
    }
    vTaskDelete(NULL);
}