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
static const char *TAG = "ble";
uint8_t ble_addr_type;

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
         .access_cb = chan_index_action,
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
         .access_cb = chan_index_action,
        },
        /* S */
        {.uuid = BLE_UUID16_DECLARE(0x0007),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = eq_index_action,
        },
        /* BANDWITH */
        {.uuid = BLE_UUID16_DECLARE(0x0008),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = eq_index_action,
        },
        /* BOOST */
        {.uuid = BLE_UUID16_DECLARE(0x0009),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = eq_index_action,
        },
        /* FREQ */
        {.uuid = BLE_UUID16_DECLARE(0x000A),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = eq_index_action,
        },
        /* GAIN */
        {.uuid = BLE_UUID16_DECLARE(0x000B),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = eq_index_action,
        },
        /* FILTER TYPE */
        {.uuid = BLE_UUID16_DECLARE(0x000C),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = eq_index_action,
        },
        /* PHASE */
        {.uuid = BLE_UUID16_DECLARE(0x000D),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = eq_index_action,
        },
        /* STATE */
        {.uuid = BLE_UUID16_DECLARE(0x000E),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = eq_index_action,
        },
        {0}    
     }
    },
    /* MUX SERVICE */
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(0x000F),
     .characteristics = (struct ble_gatt_chr_def[])
     {
        /* MUX VALUE*/ // Only for output channels.
        // On inputs this value is just 0 and can't be set.
        {.uuid = BLE_UUID16_DECLARE(0x0010),
         .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,  
         .access_cb = chan_index_action,
        },
        {0}    
     }
    },
    {0}
};

/******************************* LOCAL FUNCTIONS *************************/

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
                channelIndex = *(ctxt->om->om_data);
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
                eqIndex = *(ctxt->om->om_data);
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
        // Gather eq data for selected channel (0) from settings task.
        event.event_type = DSP_GET_EQ;
        event.chan_num   = channelIndex;
        event.eq_num     = eqIndex;
        event.output     = isOutput;

        if(send_event(toSettings, 
                      &event, 
                      &event_response, 
                      EVENT_STD_TIMEOUT_TICKS))
        {
            if(event_response.response_event_type == EVENT_RESPONSE_OK)
            {
                currentEq = event_response.response_eq;
                ESP_LOGI(TAG, "freq: %f\n", event_response.response_eq.freq);
            }
        }

        event.event_type = DSP_GET_MUX;

        if(send_event(toSettings, 
                      &event, 
                      &event_response, 
                      EVENT_STD_TIMEOUT_TICKS))
        {
            if(event_response.response_event_type == EVENT_RESPONSE_OK)
            {
                currentMux = event_response.response_mux;
                ESP_LOGI(TAG, "mux index: %i\n", event_response.response_mux.index);
            }
        }

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