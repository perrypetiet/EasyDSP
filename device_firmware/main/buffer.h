/*
 * protocol.h
 *
 * Created: 02-02-2024 
 * Author: Perry Petiet
 *
 * This module contains the types and corresponding functions that
 * implement the communciation protocol. 
 * 
 */
#ifndef BUFFER_H
#define BUFFER_H
/******************************* INCLUDES ********************************/

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "esp_log.h"
#include "event.h"

/******************************* DEFINES *********************************/

#define DATA_BUF_LEN 32

/******************************* TYPEDEFS ********************************/

typedef struct
{
    uint8_t data[DATA_BUF_LEN];
    uint8_t w;
    uint8_t r;
} ring_t;

typedef struct
{
    ring_t rx;
    ring_t tx;
} data_buffer_t;

/******************************* LOCAL FUNCTIONS *************************/

/******************************* GLOBAL FUNCTIONS ************************/

data_buffer_t *buf_inst_create();

void buf_clear_data(data_buffer_t *buf);

bool ring_add_data(ring_t *ring, uint8_t *data, uint8_t len);

bool ring_get_data(ring_t *ring, uint8_t *data, uint8_t *len);


/******************************* THE END *********************************/

#endif /* BUFFER_H_ */