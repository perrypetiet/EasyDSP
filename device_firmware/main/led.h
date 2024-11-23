/*
 * led.h
 *
 * Created: 17-08-2024 
 * Author: Perry Petiet
 *
 * Led controller for EasyDSP
 * 
 */ 
#ifndef LED_H_
#define LED_H_

/******************************* INCLUDES ********************************/

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

/******************************* DEFINES *********************************/

#define LED_PIN_NUM     GPIO_NUM_4
#define LED_FREQUENCY   4000
#define LED_MAX_DUTY    100
#define LED_FADE_TIME   1200
#define LED_FADE_MAX    100
#define LED_FADE_MIN    2
#define LED_STATIC      100

#define LEDC_MODE       LEDC_LOW_SPEED_MODE
#define LEDC_TIMER      LEDC_TIMER_2
#define LEDC_CHANNEL    LEDC_CHANNEL_0
#define LEDC_RESOLUTION LEDC_TIMER_10_BIT
#define LEDC_MAX_DUTY   1024



/******************************* TYPEDEFS ********************************/

/******************************* LOCAL FUNCTIONS *************************/

bool led_fade(uint8_t duty_cycle);

bool led_set_duty(uint8_t duty_cycle);

/******************************* GLOBAL FUNCTIONS ************************/

bool led_init(void);

void led_fade_start(void);

void led_fade_stop(void);

void led_static(void);

/******************************* THE END *********************************/

#endif /* LED_H_ */
