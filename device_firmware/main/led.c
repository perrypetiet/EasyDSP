/*
 * led.c
 *
 * Created: 17-08-2024 
 * Author: Perry Petiet
 *
 * Led controller for EasyDSP
 * 
 */ 
/******************************* INCLUDES ********************************/

#include "led.h"

/******************************* GLOBAL VARIABLES ************************/

static const char *TAG = "led";

const ledc_timer_config_t ledc_timer = 
{
    .speed_mode      = LEDC_MODE,
    .duty_resolution = LEDC_TIMER_10_BIT,
    .timer_num       = LEDC_TIMER,
    .freq_hz         = LED_FREQUENCY,
    .clk_cfg         = LEDC_AUTO_CLK
};

const ledc_channel_config_t ledc_channel = 
{
    .gpio_num   = LED_PIN_NUM,
    .speed_mode = LEDC_MODE,
    .channel    = LEDC_CHANNEL,
    .timer_sel  = LEDC_TIMER,
    .intr_type  = LEDC_INTR_FADE_END,
    .duty       = 0,
    .hpoint     = 0
};

TaskHandle_t      led_task;
SemaphoreHandle_t led_sem;

/******************************* LOCAL FUNCTIONS *************************/

static IRAM_ATTR bool cb_ledc_fade_end_event(const ledc_cb_param_t *param, 
                                             void *user_arg)
{
    BaseType_t taskAwoken = pdFALSE;

    if (param->event == LEDC_FADE_END_EVT) {
        SemaphoreHandle_t sem = (SemaphoreHandle_t) user_arg;
        xSemaphoreGiveFromISR(sem, &taskAwoken);
    }

    return (taskAwoken == pdTRUE);
}

bool led_fade(uint8_t duty_cycle)
{
    //Convert percentage to timer value
    uint16_t tim_val = ((float)duty_cycle / 100.0) * LEDC_MAX_DUTY;

    if(ledc_set_fade_with_time(LEDC_MODE, 
                               LEDC_CHANNEL, 
                               tim_val, 
                               LED_FADE_TIME) == ESP_OK &&
       ledc_fade_start(LEDC_MODE, 
                       LEDC_CHANNEL, 
                       LEDC_FADE_NO_WAIT)     == ESP_OK)
    {
        return true;
    }
    return false;
}

bool led_set_duty(uint8_t duty_cycle)
{
    if(duty_cycle <= LED_MAX_DUTY)
    {
        //Convert percentage to timer value
        uint16_t tim_val = ((float)duty_cycle / 100.0) * LEDC_MAX_DUTY;

        if(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, tim_val) == ESP_OK &&
           ledc_update_duty(LEDC_MODE, LEDC_CHANNEL)       == ESP_OK)
        {
            return true;
        }
    }
    return false;
}

void task_led_fade(void* pvParameters)
{
    // false = low to high
    // true  = high to low
    bool state = false;
    SemaphoreHandle_t semaphore = (SemaphoreHandle_t) pvParameters;
    // Infinite loop, waits for event fade interrupt
    led_fade(LED_FADE_MIN);
    for(;;)
    {
        xSemaphoreTake(semaphore, portMAX_DELAY);
        if(state == false)
        {
            led_fade(LED_FADE_MIN);
        }
        else if(state == true)
        {
            led_fade(LED_FADE_MAX);
        }
        state = !state;
    }
    vTaskDelete(NULL);
}

/******************************* GLOBAL FUNCTIONS ************************/


bool led_init()
{
    ledc_cbs_t callbacks = {
        .fade_cb = cb_ledc_fade_end_event
    };

    led_sem = xSemaphoreCreateBinary();

    if(ledc_timer_config(&ledc_timer)     == ESP_OK &&
       ledc_channel_config(&ledc_channel) == ESP_OK &&
       ledc_fade_func_install(0)          == ESP_OK &&
       ledc_cb_register(LEDC_MODE, 
                        LEDC_CHANNEL, 
                        &callbacks, 
                        (void*)led_sem)   == ESP_OK)
    {
        return true;
    }
    return false;
}

void led_fade_start()
{
    xTaskCreatePinnedToCore(task_led_fade, 
                            "LED_fader", 
                            4096, 
                            (void*) led_sem,                                 2, 
                            &led_task, 
                            tskNO_AFFINITY);
}

void led_fade_stop()
{
    vTaskDelete(led_task);
}

void led_static()
{
    led_set_duty(LED_STATIC);
}


/******************************* THE END *********************************/