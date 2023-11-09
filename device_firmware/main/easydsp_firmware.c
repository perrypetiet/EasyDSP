#include "dsp_control.h"
#include "device_settings.h"
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sigma_dsp_module_data.h"

static const char *TAG = "Main";

void settings_task(void* arg)
{
    init_device_settings();
    device_settings_load_nv();
  
    device_settings_t * settings = get_device_settings_address();
    printf("Name len: %d\n", settings->device_name_len);
    printf("Name %s\n", settings->device_name);
    // printf("Input 2 eq5 type: %d\n", settings->input2.eq5.filter_type);

    for(;;)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void dsp_task(void* arg)
{
    init_dsp_control();

    equalizer_t test_eq;

    test_eq.filter_type = FILTER_TYPE_LOWPASS;
    test_eq.phase       = PHASE_NON_INVERTED;
    test_eq.q           = 0.7;
    test_eq.freq        = 100;
    test_eq.boost       = 10;
    test_eq.gain        = 0;
    test_eq.state       = STATE_OFF;

    mux_t mux;
    mux.index = MUX_SELECT_INPUT1;

    dsp_control_eq_secondorder(MOD_INPUT1_EQ_ALG0_STAGE0_B0_ADDR, &test_eq);
    dsp_control_mux(MOD_OUTPUT1_SELECT_MONOSWSLEW_ADDR, &mux);

    for(;;)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void memory_watcher(void* arg)
{
    for(;;)
    {
        printf("RAM left %d\n", (int)esp_get_free_heap_size());
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);   
}


void app_main(void)
{
    xTaskCreatePinnedToCore(memory_watcher, 
                            "Memory", 
                            4096, 
                            NULL, 
                            2, 
                            NULL, 
                            tskNO_AFFINITY);

    xTaskCreatePinnedToCore(dsp_task, 
                            "DSP_handler", 
                            4096, 
                            NULL, 
                            2, 
                            NULL, 
                            tskNO_AFFINITY);

    xTaskCreatePinnedToCore(settings_task, 
                            "Settings_handler", 
                            4096, 
                            NULL, 
                            2, 
                            NULL, 
                            tskNO_AFFINITY);
}

