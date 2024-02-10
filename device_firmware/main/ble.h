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

/******************************* DEFINES *********************************/

/******************************* TYPEDEFS ********************************/

/******************************* LOCAL FUNCTIONS *************************/

int serial_write_cb(uint16_t conn_handle, 
                    uint16_t attr_handle, 
                    struct ble_gatt_access_ctxt *ctxt, 
                    void *arg);

int serial_read_cb(uint16_t con_handle, 
                   uint16_t attr_handle, 
                   struct ble_gatt_access_ctxt *ctxt, 
                   void *arg);

void ble_app_on_sync(void);

int ble_gap_event(struct ble_gap_event *event, void *arg);

void ble_app_advertise(void);

void host_task(void *param);

/******************************* GLOBAL FUNCTIONS ************************/

bool init_ble(uint8_t *name, data_buffer_t *data_buffer);

/******************************* THE END *********************************/

#endif /* BLE_H_ */
