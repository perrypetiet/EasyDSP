/*
 * interface_ble.c
 *
 * Created: 30-11-2023 
 * Author: Perry Petiet
 *
 * This module contains the functions to initaliaze and use the BLE
 * functionality on the ESP32 as a GATT device. It initalises 1 service
 * with a read and write characteristic to make it a "serial" device.
 * 
 * Custom BLE services and characteristics for each EQ may be implemented
 * after this prototype. 
 * 
 */
/******************************* INCLUDES ********************************/

#include "ble.h"
  
/******************************* GLOBAL VARIABLES ************************/

uint8_t     channelIndex = 0;
uint8_t     eqIndex      = 0;
bool        isOutput     = false;

equalizer_t currentEq;
mux_t       currentMux;

communication_t*     toSettings = NULL;
dsp_event_t          event;
dsp_event_response_t event_response;

void (*ble_event_handler)(dsp_event_t) = NULL;
static const char *TAG = "BLE";
uint8_t ble_addr_type;

int32_t fToIntBuf;

// Describes services and characteristics
static const struct ble_gatt_svc_def gatt_svcs[] = {
    /* INDEXES */
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(0x0001),
     .characteristics = (struct ble_gatt_chr_def[])
     {
        /* CHANNEL INDEX*/
        {.uuid = BLE_UUID16_DECLARE(0x0002),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = chan_index_action,
        },
        /* IS OUTPUT*/
        {.uuid = BLE_UUID16_DECLARE(0x0003),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = is_output_action,
        },
        /* EQ INDEX*/
        {.uuid = BLE_UUID16_DECLARE(0x0004),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = eq_index_action,
        },
        {0}    
     }
    },
    /* EQUALIZER SERVICE */
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(0x0005),
     .characteristics = (struct ble_gatt_chr_def[])
     {
        /* Q */
        {.uuid = BLE_UUID16_DECLARE(0x0006),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = q_action,
        },
        /* S */
        {.uuid = BLE_UUID16_DECLARE(0x0007),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = s_action,
        },
        /* BANDWITH */
        {.uuid = BLE_UUID16_DECLARE(0x0008),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = bandwith_action,
        },
        /* BOOST */
        {.uuid = BLE_UUID16_DECLARE(0x0009),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = boost_action,
        },
        /* FREQ */
        {.uuid = BLE_UUID16_DECLARE(0x000A),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = freq_action,
        },
        /* GAIN */
        {.uuid = BLE_UUID16_DECLARE(0x000B),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = gain_action,
        },
        /* FILTER TYPE */
        {.uuid = BLE_UUID16_DECLARE(0x000C),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = filter_type_action,
        },
        /* PHASE */
        {.uuid = BLE_UUID16_DECLARE(0x000D),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = phase_action,
        },
        /* STATE */
        {.uuid = BLE_UUID16_DECLARE(0x000E),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = state_action,
        },
        {0}    
     }
    },
    /* MUX SERVICE */
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(0x000F),
     .characteristics = (struct ble_gatt_chr_def[])
     {
        /* MUX VALUE*/ 
        // Only for output channels.
        // On inputs this value doesn't matter and cant be set.
        {.uuid = BLE_UUID16_DECLARE(0x0010),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = mux_action,
        },
        {0}    
     }
    },
    {0}
};


/******************************* LOCAL FUNCTIONS *************************/

bool send_current_eq()
{
    if(toSettings != NULL)
    {
        event.chan_num   = channelIndex;
        event.eq_num     = eqIndex;
        event.output     = isOutput;
        event.eq         = currentEq;

        event.event_type = DSP_SET_EQ;

        if(send_event(toSettings, 
                      &event, 
                      &event_response, 
                      EVENT_STD_TIMEOUT_TICKS))
        {
            if(event_response.response_event_type == EVENT_RESPONSE_OK)
            {
                ESP_LOGI(TAG, "Response to send eq ok.");
                return true;
            }
        }
    }
    return false;
}

bool send_current_mux()
{
    if(toSettings != NULL)
    {
        event.chan_num   = channelIndex;
        event.output     = isOutput;
        event.mux        = currentMux;

        event.event_type = DSP_SET_MUX;

        if(send_event(toSettings, 
                      &event, 
                      &event_response, 
                      EVENT_STD_TIMEOUT_TICKS))
        {
            if(event_response.response_event_type == EVENT_RESPONSE_OK)
            {
                ESP_LOGI(TAG, "Response to send mux ok.");
                return true;
            }
        }
    }
    return false;
}

bool update_current_eq()
{
    if(toSettings != NULL)
    {
        event.chan_num   = channelIndex;
        event.eq_num     = eqIndex;
        event.output     = isOutput;

        event.event_type = DSP_GET_EQ;

        if(send_event(toSettings, 
                      &event, 
                      &event_response, 
                      EVENT_STD_TIMEOUT_TICKS))
        {
            if(event_response.response_event_type == EVENT_RESPONSE_OK)
            {
                currentEq = event_response.response_eq;
                ESP_LOGI(TAG, "Response to get eq ok.");
                return true;
            }
        }
    }
    return false;
}

bool update_current_mux()
{
    if(toSettings != NULL)
    {
        event.chan_num   = channelIndex;
        event.output     = isOutput;

        event.event_type = DSP_GET_MUX;

        if(send_event(toSettings, 
                      &event, 
                      &event_response, 
                      EVENT_STD_TIMEOUT_TICKS))
        {
            if(event_response.response_event_type == EVENT_RESPONSE_OK)
            {
                currentMux = event_response.response_mux;
                ESP_LOGI(TAG, "Response to get mux ok.");
                return true;
            }
        }
    }
    return false;
}

/******************************* CHARACTERSTIC CALLBACKS *****************/

int chan_index_action(uint16_t conn_handle, 
                     uint16_t attr_handle, 
                     struct ble_gatt_access_ctxt *ctxt, 
                     void *arg)
{
    if(toSettings != NULL)
    {
        switch(ctxt->op)
        {  
            // Get channel index.
            case BLE_GATT_ACCESS_OP_READ_CHR:
                os_mbuf_append(ctxt->om, &channelIndex, sizeof(uint8_t));  
                break;

            // Set channel index.
            case BLE_GATT_ACCESS_OP_WRITE_CHR:
                uint8_t previous_index = channelIndex;
                channelIndex = *(ctxt->om->om_data);
                
                if(isOutput)
                {
                    if(!update_current_eq() ||
                    !update_current_mux()  )
                    {
                        channelIndex = previous_index;
                    }
                }
                else
                {
                    currentMux.index = 0;
                    if(!update_current_eq())
                    {
                       channelIndex = previous_index; 
                    }
                }
                break;
        }
    }
    return 0;
}

int is_output_action(uint16_t conn_handle, 
                     uint16_t attr_handle, 
                     struct ble_gatt_access_ctxt *ctxt, 
                     void *arg)
{
    if(toSettings != NULL)
    {
        switch(ctxt->op)
        {
            // get is output
            case BLE_GATT_ACCESS_OP_READ_CHR:
                os_mbuf_append(ctxt->om, &isOutput, sizeof(uint8_t));  
                break;

            // Set is output
            case BLE_GATT_ACCESS_OP_WRITE_CHR:
                bool prev = isOutput;
                isOutput = *(ctxt->om->om_data);
                if(!update_current_eq() ||
                   !update_current_mux())
                {
                    isOutput = prev;
                }
                break;
        }
    }
    return 0;
}

int eq_index_action(uint16_t conn_handle, 
                    uint16_t attr_handle, 
                    struct ble_gatt_access_ctxt *ctxt, 
                    void *arg)
{
    if(toSettings != NULL)
    {
        switch(ctxt->op)
        {
            // Get eq index.
            case BLE_GATT_ACCESS_OP_READ_CHR:
                os_mbuf_append(ctxt->om, &eqIndex, sizeof(uint8_t));  
                break;

            // Set eq index.
            case BLE_GATT_ACCESS_OP_WRITE_CHR:
                uint8_t prev = eqIndex;
                eqIndex = *(ctxt->om->om_data);
                if(!update_current_eq())
                {
                    eqIndex = prev;
                }
                break;
        }
    }
    return 0;
}

int q_action(uint16_t con_handle, 
             uint16_t attr_handle, 
             struct ble_gatt_access_ctxt *ctxt, 
             void *arg)
{
    if(toSettings != NULL)
    {
        switch(ctxt->op)
        {
            // Get q.
            case BLE_GATT_ACCESS_OP_READ_CHR:
                fToIntBuf = (int32_t)(currentEq.q * 100);
                os_mbuf_append(ctxt->om, &fToIntBuf, sizeof(int32_t));  
                break;

            // Set q.
            case BLE_GATT_ACCESS_OP_WRITE_CHR:
                float old = currentEq.q;
                currentEq.q = (float)(*(ctxt->om->om_data) / 100);
                if(!send_current_eq())
                {
                    currentEq.q = old;
                }
                break;
        }
    }
    return 0;
}

int s_action(uint16_t con_handle, 
             uint16_t attr_handle, 
             struct ble_gatt_access_ctxt *ctxt, 
             void *arg)
{
    if(toSettings != NULL)
    {
        switch(ctxt->op)
        {
            // Get s.
            case BLE_GATT_ACCESS_OP_READ_CHR:
                fToIntBuf = (int32_t)(currentEq.s * 100);
                os_mbuf_append(ctxt->om, &fToIntBuf, sizeof(int32_t));  
                break;

            // Set s.
            case BLE_GATT_ACCESS_OP_WRITE_CHR:
                float old = currentEq.s;
                currentEq.s = (float)(*(ctxt->om->om_data) / 100);
                if(!send_current_eq())
                {
                    currentEq.s = old;
                }
                break;
        }
    }
    return 0;
}

int bandwith_action(uint16_t con_handle, 
             uint16_t attr_handle, 
             struct ble_gatt_access_ctxt *ctxt, 
             void *arg)
{
    if(toSettings != NULL)
    {
        switch(ctxt->op)
        {
            // Get bandwith.
            case BLE_GATT_ACCESS_OP_READ_CHR:
                fToIntBuf = (int32_t)(currentEq.bandwidth * 100);
                os_mbuf_append(ctxt->om, &fToIntBuf, sizeof(int32_t));  
                break;

            // Set bandwith.
            case BLE_GATT_ACCESS_OP_WRITE_CHR:
                float old = currentEq.bandwidth;
                currentEq.bandwidth = (float)(*(ctxt->om->om_data) / 100);
                if(!send_current_eq())
                {
                    currentEq.bandwidth = old;
                }
                break;
        }
    }
    return 0;
}

int boost_action(uint16_t con_handle, 
             uint16_t attr_handle, 
             struct ble_gatt_access_ctxt *ctxt, 
             void *arg)
{
    if(toSettings != NULL)
    {
        switch(ctxt->op)
        {
            // Get boost.
            case BLE_GATT_ACCESS_OP_READ_CHR:
                fToIntBuf = (int32_t)(currentEq.boost * 100);
                os_mbuf_append(ctxt->om, &fToIntBuf, sizeof(int32_t));  
                break;

            // Set boost.
            case BLE_GATT_ACCESS_OP_WRITE_CHR:
                float old = currentEq.boost;
                currentEq.boost = (float)(*(ctxt->om->om_data) / 100);
                if(!send_current_eq())
                {
                    currentEq.boost = old;
                }
                break;
        }
    }
    return 0;
}

int freq_action(uint16_t con_handle, 
             uint16_t attr_handle, 
             struct ble_gatt_access_ctxt *ctxt, 
             void *arg)
{
    if(toSettings != NULL)
    {
        switch(ctxt->op)
        {
            // Get boost.
            case BLE_GATT_ACCESS_OP_READ_CHR:
                ESP_LOGW(TAG, "%f", currentEq.freq);
                fToIntBuf = (int32_t)(currentEq.freq * 100);
                ESP_LOGW(TAG, "%ld", fToIntBuf);
                os_mbuf_append(ctxt->om, &fToIntBuf, sizeof(int32_t));  
                break;

            // Set boost.
            case BLE_GATT_ACCESS_OP_WRITE_CHR:
                float old = currentEq.freq;
                currentEq.freq = (float)(*(ctxt->om->om_data) / 100);
                if(!send_current_eq())
                {
                    currentEq.freq = old;
                }
                break;
        }
    }
    return 0;
}

int gain_action(uint16_t con_handle, 
             uint16_t attr_handle, 
             struct ble_gatt_access_ctxt *ctxt, 
             void *arg)
{
    if(toSettings != NULL)
    {
        switch(ctxt->op)
        {
            // Get gain.
            case BLE_GATT_ACCESS_OP_READ_CHR:
                fToIntBuf = (int32_t)(currentEq.gain * 100);
                os_mbuf_append(ctxt->om, &fToIntBuf, sizeof(int32_t));  
                break;

            // Set gain.
            case BLE_GATT_ACCESS_OP_WRITE_CHR:
                float old = currentEq.gain;
                currentEq.gain = (float)(*(ctxt->om->om_data) / 100);
                if(!send_current_eq())
                {
                    currentEq.gain = old;
                }
                break;
        }
    }
    return 0;
}

int filter_type_action(uint16_t con_handle, 
             uint16_t attr_handle, 
             struct ble_gatt_access_ctxt *ctxt, 
             void *arg)
{
    if(toSettings != NULL)
    {
        switch(ctxt->op)
        {
            // Get filter type.
            case BLE_GATT_ACCESS_OP_READ_CHR:
                os_mbuf_append(ctxt->om, &currentEq.filter_type, sizeof(uint8_t));  
                break;

            // Set filter type.
            case BLE_GATT_ACCESS_OP_WRITE_CHR:
                float old = currentEq.filter_type;
                currentEq.filter_type = *ctxt->om->om_data;
                if(!send_current_eq())
                {
                    currentEq.filter_type = old;
                }
                break;
        }
    }
    return 0;
}

int phase_action(uint16_t con_handle, 
             uint16_t attr_handle, 
             struct ble_gatt_access_ctxt *ctxt, 
             void *arg)
{
    if(toSettings != NULL)
    {
        switch(ctxt->op)
        {
            // Get phase.
            case BLE_GATT_ACCESS_OP_READ_CHR:
                os_mbuf_append(ctxt->om, &currentEq.phase, sizeof(uint8_t));  
                break;

            // Set phase.
            case BLE_GATT_ACCESS_OP_WRITE_CHR:
                float old = currentEq.phase;
                currentEq.phase = *ctxt->om->om_data;
                if(!send_current_eq())
                {
                    currentEq.phase = old;
                }
                break;
        }
    }
    return 0;
}

int state_action(uint16_t con_handle, 
             uint16_t attr_handle, 
             struct ble_gatt_access_ctxt *ctxt, 
             void *arg)
{
    if(toSettings != NULL)
    {
        switch(ctxt->op)
        {
            // Get state.
            case BLE_GATT_ACCESS_OP_READ_CHR:
                os_mbuf_append(ctxt->om, &currentEq.state, sizeof(uint8_t));  
                break;

            // Set state.
            case BLE_GATT_ACCESS_OP_WRITE_CHR:
                float old = currentEq.state;
                currentEq.state = *ctxt->om->om_data;
                if(!send_current_eq())
                {
                    currentEq.state = old;
                }
                break;
        }
    }
    return 0;
}

int mux_action(uint16_t con_handle, 
             uint16_t attr_handle, 
             struct ble_gatt_access_ctxt *ctxt, 
             void *arg)
{
    if(toSettings != NULL)
    {
        switch(ctxt->op)
        {
            // Get mux.
            case BLE_GATT_ACCESS_OP_READ_CHR:
                os_mbuf_append(ctxt->om, &currentMux.index, sizeof(uint8_t));  
                break;

            // Set mux.
            case BLE_GATT_ACCESS_OP_WRITE_CHR:
                float old = currentMux.index;
                currentMux.index = *ctxt->om->om_data;
                if(!send_current_mux())
                {
                    currentMux.index = old;
                }
                break;
        }
    }
    return 0;
}

// On sync callback function
void ble_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type);
    ble_app_advertise();                     
}

int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
    // Advertise if connected
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI(TAG, "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
        if (event->connect.status != 0)
        {
            ble_app_advertise();
        }
        else
        {
            led_fade_stop();
            led_static();
        }
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "Client disconnected.");
        led_fade_start();
        ble_app_advertise();
        break;    
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(TAG, "BLE GAP EVENT");
        ble_app_advertise();
        break;
    default:
        break;
    }
    return 0;
}

void ble_app_advertise(void)
{
    // GAP - device name definition
    struct ble_hs_adv_fields fields;
    const char *device_name;
    memset(&fields, 0, sizeof(fields));
    device_name = ble_svc_gap_device_name(); 
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    fields.tx_pwr_lvl = -128; // auto power level is -128
    ble_gap_adv_set_fields(&fields);

    // GAP - device connectivity definition
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND; 
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    ble_gap_adv_start(ble_addr_type, 
                      NULL, 
                      BLE_HS_FOREVER, 
                      &adv_params, 
                      ble_gap_event, 
                      NULL);
}

// The infinite RTOS task. This is the taks that runs the actual BLE stack
void host_task(void *param)
{
    nimble_port_run(); 
}

/******************************* GLOBAL FUNCTIONS ************************/

bool init_ble(uint8_t* name, communication_t* communication_data)
{
    //https://github.com/SIMS-IOT-Devices/FreeRTOS-ESP-IDF-BLE-Server/blob/main/proj3.c
    /* Initialize NVS — it is used to store PHY calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || 
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_LOGI(TAG, "Staring BLE...");

    esp_nimble_hci_init();
    nimble_port_init();
    ble_svc_gap_device_name_set((char*)name); 
    ble_svc_gap_init();                        
    ble_svc_gatt_init();

    ble_gatts_count_cfg(gatt_svcs); 
    ble_gatts_add_svcs(gatt_svcs);
    ble_hs_cfg.sync_cb = ble_app_on_sync;
    nimble_port_freertos_init(host_task);

    // Initialize led
    if(led_init() && (communication_data != NULL))
    {
        toSettings = communication_data;

        update_current_eq();
        update_current_mux();

        led_fade_start();
        return true;
    }
    return false;
}

void set_event_handler(void (*event_handler)(dsp_event_t))
{   
    ble_event_handler = event_handler; 
}

void remove_event_handler()
{
    ble_event_handler = NULL;
}

/******************************* THE END *********************************/