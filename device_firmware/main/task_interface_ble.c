#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "event.h"

#include "interface_ble.h"


/* Function: interface_ble_task 
 *
 * This function is run as a thread by FreeRTOS. 
 * This task handles initialization of the BLE GATT server for
 * characteristics for a read/write setup of BLE. It will
 * also handle receiving and writing data and putting received
 * data into a buffer. Once a message has been found, it will
 * send it to the command interface task using an event.
 *
 */

 void task_interface_ble(void* pvParameters)
{
    bool ret = init_interface_ble();

    // Infinite loop, waits for event from command interface.
    for(;;)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}