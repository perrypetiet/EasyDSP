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
#ifndef INTERFACE_BLE_H_
#define INTERFACE_BLE_H_

/******************************* INCLUDES ********************************/

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "esp_log.h"
#include "esp_err.h"
#include "event.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

/******************************* DEFINES *********************************/

/******************************* TYPEDEFS ********************************/

/******************************* LOCAL FUNCTIONS *************************/

/******************************* GLOBAL FUNCTIONS ************************/

bool init_ble();

/******************************* THE END *********************************/

#endif /* INTERFACE_BLE_H_ */
