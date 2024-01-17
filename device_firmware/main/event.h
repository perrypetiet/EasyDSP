/*
 * event.h
 *
 * Created: 13-11-2023 
 * Author: Perry Petiet
 *
 * This include file describes different events that can be triggered 
 * between tasks. Our tasks need to communicate in order to transfer data
 * between eacht other. Here is described what kind of data is transferred
 * in an event.
 * 
 */ 
#ifndef EVENT_H_
#define EVENT_H_

/******************************* INCLUDES ********************************/

#include "device_settings.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <stdbool.h>

/******************************* DEFINES *********************************/

#define QUEUE_SIZE              1
#define EVENT_STD_TIMEOUT_MS    5000
#define EVENT_STD_TIMEOUT_TICKS (EVENT_STD_TIMEOUT_MS / portTICK_PERIOD_MS)

// MODIFY SETTINGS EVENT TYPES
#define DSP_SET_EQ   0
#define DSP_SET_MUX  1
#define DSP_SET_GAIN 2
#define DSP_GET_EQ   3
#define DSP_GET_MUX  4
#define DSP_GET_GAIN 5

// EVENT RESPONSE EVENT TYPES
#define EVENT_RESPONSE_OK             0
#define EVENT_RESPONSE_ERROR          1
#define EVENT_RESPONSE_DSP_ERROR      2
#define EVENT_RESPONSE_SETTINGS_ERROR 3
#define EVENT_RESPONSE_SETTINGS_EQ    4
#define EVENT_RESPONSE_SETTINGS_MUX   5
#define EVENT_RESPONSE_SETTINGS_GAIN  6


/******************************* TYPEDEFS ********************************/

//TODO: add gain
typedef struct
{
    uint8_t      event_type;

    equalizer_t  eq;
    mux_t        mux;
    // gain_t      gain;
}dsp_event_t;

typedef struct
{
    uint8_t response_event_type;

    equalizer_t response_eq;
    mux_t       response_mux;
    //gain_t      response_gain;
}dsp_event_response_t;

typedef struct
{
    QueueHandle_t event_queue;
    QueueHandle_t event_response_queue;
}communication_t;

/******************************* LOCAL FUNCTIONS *************************/

/******************************* GLOBAL FUNCTIONS ************************/

// For communication between settings and dsp task
communication_t* dsp_communication_create();

// Waits for DSP event until timout
bool await_event(communication_t *communication, 
                 void *event, 
                 TickType_t timeout);

// Sends a single DSP event response
bool send_event_response(communication_t *communcation,
                         void *event_response,
                         TickType_t timeout);  

// Sends a DSP event and waits for response until timeout
bool send_event(communication_t *communication,
                void *event,
                void *response,
                TickType_t timeout);

// For communication between interfaces and command interface task
communication_t* command_communication_create();

/******************************* THE END *********************************/

#endif /* EVENT_H_ */