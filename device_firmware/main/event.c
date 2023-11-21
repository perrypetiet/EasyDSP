/*
 * event.c
 *
 * Created: 21-11-2023 
 * Author: Perry Petiet
 *
 * This include file describes different events that can be triggered 
 * between tasks. Our tasks need to communicate in order to transfer data
 * between eacht other. Here is described what kind of data is transferred
 * in an event.
 * 
 */ 
/******************************* INCLUDES ********************************/

#include "event.h"

/******************************* GLOBAL VARIABLES ************************/

static const char *TAG = "Event";
 
/******************************* LOCAL FUNCTIONS *************************/

/******************************* GLOBAL FUNCTIONS ************************/

communication_t* dsp_communication_create()                                       
{
    communication_t* ret = 
                  (communication_t*)malloc(sizeof(communication_t));

    if(ret != NULL)
    {
        ret->event_queue = xQueueCreate(QUEUE_SIZE, sizeof(dsp_event_t));
        ret->event_response_queue = xQueueCreate(QUEUE_SIZE, 
                                          sizeof(dsp_event_response_t));
        return ret;        
    }
    return ret;
}

// Combination of communication and event has to be correct, queues in the 
// communication have the be created for the event types using.
bool await_event(communication_t *communication,
                 void *event,
                 TickType_t timeout)
{
    if(communication != NULL && event != NULL)
    {
        if(xQueueReceive(communication->event_queue, event, timeout) == pdTRUE)
        {
            return true;
        }
    }
    return false;
}

bool send_event_response(communication_t *communication,
                         void *event_response,
                         TickType_t timeout)
{
    if(communication != NULL && event_response != NULL)
    {
        if(xQueueSend(communication->event_response_queue, 
                      event_response, 
                      timeout) == pdTRUE)
        {
            return true;
        }
    }
    return false;
}

bool send_event(communication_t *communication,
                void *event,
                void *response,
                TickType_t timeout)
{
    if(communication != NULL && event != NULL && response != NULL)
    {
        // Send the event,
        if(xQueueSend(communication->event_queue, 
                      event, 
                      timeout) == pdTRUE)
        {
            // And await a response
            if(xQueueReceive(communication->event_response_queue, 
                             response, 
                             timeout) == pdTRUE)
            {
                return true;
            }
        }
    }
    return false;
}


/******************************* THE END *********************************/