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

/*
Tasks initialises the supported interfaces 
*/
static const char *TAG = "Interfaces task";

void task_interfaces(void* pvParameters)
{
  communication_t* toSettings = (communication_t*)pvParameters;
  uint8_t device_name[] = "easydsp";

  init_ble(device_name, toSettings);

  for(;;) 
  {
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}