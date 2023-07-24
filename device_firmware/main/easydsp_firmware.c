#include <stdio.h>
#include "driver/gpio.h"

void app_main(void)
{
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);

    while(true)
    {
        gpio_set_level(GPIO_NUM_2, 0);
    }
}
