#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

void eight_second_task(void* arg)
{
    printf("Eight second task finished!");
    vTaskDelete(NULL);
}

void ten_second_task(void* arg)
{
    vTaskDelay(pdMS_TO_TICKS(1000));
    printf("Ten second task finished!");
    vTaskDelay(pdMS_TO_TICKS(1000));
    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreate(eight_second_task, "eight second task", 4096, NULL, tskIDLE_PRIORITY, NULL);
}

