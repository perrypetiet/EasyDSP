/*
 * buffer.h
 *
 * Created: 02-02-2024 
 * Author: Perry Petiet
 *
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

#define DATA_BUF_LEN 255

/******************************* TYPEDEFS ********************************/

typedef struct
{
    uint8_t data[DATA_BUF_LEN];
    uint16_t w;
    uint16_t r;
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

bool ring_get_data(ring_t *ring, uint8_t *data, uint8_t len);

uint16_t data_len_available(ring_t *ring);

// Peeks in a buffer. Index starts at read pointer.
uint8_t buffer_peek(ring_t *ring, uint16_t index);




#endif /* BUFFER_H_ */