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

/******************************* DEFINES *********************************/

#define QUEUE_SIZE 5

enum event_t
{
    DSP_CHANGE_EQ,
    DSP_CHANGE_MUX
};

/******************************* TYPEDEFS ********************************/

typedef struct
{
    enum event_t event_type;

    equalizer_t  eq;
    mux_t        mux;
}communication_event_t;

/******************************* THE END *********************************/

#endif /* EVENT_H_ */