#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "event.h"
#include "ble.h"

 void task_interfaces(void* pvParameters)
{
    bool ret = init_ble();

    // Infinite loop, waits for event from command interface.
    for(;;)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}