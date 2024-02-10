/*
 * protocol.c
 *
 * Created: 02-02-2024 
 * Author: Perry Petiet
 *
 * This module contains the types and corresponding functions that
 * implement the communciation protocol. 
 * 
 */
/******************************* INCLUDES ********************************/

#include "Buffer.h"

/******************************* GLOBAL VARIABLES ************************/

static const char *TAG = "data_buffer";

/******************************* LOCAL FUNCTIONS *************************/

/******************************* GLOBAL FUNCTIONS ************************/

data_buffer_t *buf_inst_create()
{
    data_buffer_t *ret = NULL;

    ret = malloc(sizeof(data_buffer_t));

    if (ret != NULL)
    {
        buf_clear_data(ret);
    }
    return ret;
}

void buf_clear_data(data_buffer_t *buf)
{
    if(buf != NULL)
    {
        memset(buf->rx.data, 0, DATA_BUF_LEN);
        buf->rx.w = 0;
        buf->rx.r = 0;
        memset(buf->tx.data, 0, DATA_BUF_LEN);
        buf->tx.w = 0;
        buf->tx.r = 0;
    }
}

bool ring_add_data(ring_t *ring, uint8_t *data, uint8_t len)
{
    ESP_LOGI(TAG, "Adding data to buffer...");  
    if (ring != NULL)
    {
        if (len + ring->w > DATA_BUF_LEN) // overflow  
        {
            /* code */
        }
    }
    return false;
}

bool ring_get_data(ring_t *ring, uint8_t *data, uint8_t *len)
{
    ESP_LOGI(TAG, "Getting data from buffer...");
    return true;
}


/******************************* THE END *********************************/