/*
 * interface_ble.h
 *
 * Created: 30-11-2023 
 * Author: Perry Petiet
 *
 * This module contains the functions to initaliaze and use the BLE
 * functionality on the ESP32 as a GATT device. It initalises 1 service
 * with a read and write characteristic to make it a "serial" device.
 * 
 */ 
#ifndef BLE_H
#define BLE_H

/******************************* INCLUDES ********************************/

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "esp_log.h"
#include "esp_err.h"
#include "event.h"
#include "nvs_flash.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "host/ble_uuid.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "buffer.h"
#include "device_settings.h"
#include "led.h"

/******************************* DEFINES *********************************/

#define CHAN_TOTAL DEVICE_SETTINGS_INPUT_AMOUNT + DEVICE_SETTINGS_OUTPUT_AMOUNT

/******************************* TYPEDEFS ********************************/

/******************************* LOCAL FUNCTIONS *************************/

void ble_app_on_sync(void);

int ble_gap_event(struct ble_gap_event *event, void *arg);

void ble_app_advertise(void);

void host_task(void *param);

bool update_current_eq(void);
bool update_current_mux(void);

/******************************* CHARACTERSTIC CALLBACKS *****************/

int chan_index_action(uint16_t con_handle, 
                      uint16_t attr_handle, 
                      struct ble_gatt_access_ctxt *ctxt, 
                      void *arg);

int is_output_action(uint16_t con_handle, 
                     uint16_t attr_handle, 
                     struct ble_gatt_access_ctxt *ctxt, 
                     void *arg);                     

int eq_index_action(uint16_t con_handle, 
                    uint16_t attr_handle, 
                    struct ble_gatt_access_ctxt *ctxt, 
                    void *arg);

int q_action(uint16_t con_handle, 
             uint16_t attr_handle, 
             struct ble_gatt_access_ctxt *ctxt, 
             void *arg);   

int s_action(uint16_t con_handle, 
             uint16_t attr_handle, 
             struct ble_gatt_access_ctxt *ctxt, 
             void *arg);

int bandwith_action(uint16_t con_handle, 
                    uint16_t attr_handle, 
                    struct ble_gatt_access_ctxt *ctxt, 
                    void *arg);   
int boost_action(uint16_t con_handle, 
                 uint16_t attr_handle, 
                 struct ble_gatt_access_ctxt *ctxt, 
                 void *arg);

int freq_action(uint16_t con_handle, 
                uint16_t attr_handle, 
                struct ble_gatt_access_ctxt *ctxt, 
                void *arg);

int gain_action(uint16_t con_handle, 
                uint16_t attr_handle, 
                struct ble_gatt_access_ctxt *ctxt, 
                void *arg);   

int filter_type_action(uint16_t con_handle, 
                       uint16_t attr_handle, 
                       struct ble_gatt_access_ctxt *ctxt, 
                       void *arg);

int phase_action(uint16_t con_handle, 
                 uint16_t attr_handle, 
                 struct ble_gatt_access_ctxt *ctxt, 
                 void *arg);

int state_action(uint16_t con_handle, 
                 uint16_t attr_handle, 
                 struct ble_gatt_access_ctxt *ctxt, 
                 void *arg);

int mux_action(uint16_t con_handle, 
                 uint16_t attr_handle, 
                 struct ble_gatt_access_ctxt *ctxt, 
                 void *arg);   

/******************************* GLOBAL FUNCTIONS ************************/

bool init_ble(uint8_t* name, communication_t* communication_data);

void set_event_handler(void (*event_handler)(dsp_event_t));
void remove_event_handler(void);

/******************************* THE END *********************************/

#endif /* BLE_H_ */
