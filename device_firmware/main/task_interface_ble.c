#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "event.h"
#include "esp_system.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

/* Function: interface_ble 
 *
 * This function is run as a thread by FreeRTOS. 
 * This task handles initialization of the BLE GATT server for
 * characteristics for a read/write setup of BLE. It will
 * also handle receiving and writing data and putting received
 * data into a buffer. Once a message has been found, it will
 * send it to the command interface task using an event.
 *
 */