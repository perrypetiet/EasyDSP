/*
 * buffer.c
 *
 * Created: 02-02-2024 
 * Author: Perry Petiet
 *
 * 
 */
/******************************* INCLUDES ********************************/

#include "buffer.h"

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

//add given data to buffer.
bool ring_add_data(ring_t *ring, uint8_t *data, uint8_t len)
{
    ESP_LOGI(TAG, "Adding data to buffer...");  
    
    for(int i = 0; i < len; i++)
    {
        ring->data[ring->w] = data[i];
        // increment write pointer.
        if(ring->w == DATA_BUF_LEN - 1)
        {
            ring->w = 0;
        }
        else
        {
            ring->w++;
        }
    }
    return true;
}

//get data from buffer
bool ring_get_data(ring_t *ring, uint8_t *data, uint8_t len)
{
    ESP_LOGI(TAG, "Getting data from buffer...");
    for(int i = 0; i < len; i++)
    {
        data[i] = ring->data[ring->r];
        if(ring->r == DATA_BUF_LEN - 1)
        {
            ring->r = 0;
        }
        else
        {
            ring->r++;
        }
    }
    return true;
}

uint16_t data_len_available(ring_t *ring)
{
    if(ring->w >= ring->r)
    {
        return ring->w - ring->r;
    }
    if(ring->w <= ring->r)
    {
        return (DATA_BUF_LEN - ring->r) + ring->w; 
    }
    return 0;
}


/******************************* THE END *********************************/