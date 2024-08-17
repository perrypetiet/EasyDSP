#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "event.h"
#include "ble.h"
#include "buffer.h"
#include "event.h"

void task_interfaces(void* pvParameters)
{

    // -- Block the task and wait for the device name from the
    //    settings task.

    uint8_t device_name[] = "easydsp";

    // Initialize the BLE, it will run as a task on its own.

    data_buffer_t *ble_buffer = buf_inst_create();
    init_ble(device_name, ble_buffer);
    

    // Infinite loop, waits for event from command interface.
    for(;;)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}