#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

void task_one(void* arg)
{
    for(;;)
    {
        printf("task one running\n");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void task_two(void* arg)
{
    for(;;)
    {
        printf("task two running\n");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreatePinnedToCore(task_one, "task_one", 4096, NULL, tskIDLE_PRIORITY, NULL, 0);
    xTaskCreatePinnedToCore(task_two, "task_two", 4096, NULL, tskIDLE_PRIORITY, NULL, 1);
}

