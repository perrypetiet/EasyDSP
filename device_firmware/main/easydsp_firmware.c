#include "dsp_control.h"
#include "device_settings.h"
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "Main";

void settings_task(void* arg)
{
    init_device_settings();

    device_settings_load_nv();

    // device_settings_load_factory();
    // device_settings_store_nv();

    device_settings_t * settings = get_device_settings_address();

    printf("Input1 eq1 address: %d\n", settings->inputs[0].eq[0].sigma_dsp_address);
    printf("Input1 eq2 address: %d\n", settings->inputs[0].eq[1].sigma_dsp_address);
    printf("Input1 eq3 address: %d\n", settings->inputs[0].eq[2].sigma_dsp_address);
    printf("Input1 eq4 address: %d\n", settings->inputs[0].eq[3].sigma_dsp_address);
    printf("Input1 eq5 address: %d\n", settings->inputs[0].eq[4].sigma_dsp_address);

    printf("Input2 eq1 address: %d\n", settings->inputs[1].eq[0].sigma_dsp_address);
    printf("Input2 eq2 address: %d\n", settings->inputs[1].eq[1].sigma_dsp_address);
    printf("Input2 eq3 address: %d\n", settings->inputs[1].eq[2].sigma_dsp_address);
    printf("Input2 eq4 address: %d\n", settings->inputs[1].eq[3].sigma_dsp_address);
    printf("Input2 eq5 address: %d\n", settings->inputs[1].eq[4].sigma_dsp_address);


    printf("Output1 mux addr: %d\n", settings->outputs[0].mux.sigma_dsp_address);
    printf("Output2 mux addr: %d\n", settings->outputs[1].mux.sigma_dsp_address);
    printf("Output3 mux addr: %d\n", settings->outputs[2].mux.sigma_dsp_address);
    printf("Output4 mux addr: %d\n", settings->outputs[3].mux.sigma_dsp_address);

    printf("Output1 eq1 address: %d\n", settings->outputs[0].eq[0].sigma_dsp_address);
    printf("Output1 eq2 address: %d\n", settings->outputs[0].eq[1].sigma_dsp_address);
    printf("Output1 eq3 address: %d\n", settings->outputs[0].eq[2].sigma_dsp_address);
    printf("Output1 eq4 address: %d\n", settings->outputs[0].eq[3].sigma_dsp_address);
    printf("Output1 eq5 address: %d\n", settings->outputs[0].eq[4].sigma_dsp_address);

    printf("Output2 eq1 address: %d\n", settings->outputs[1].eq[0].sigma_dsp_address);
    printf("Output2 eq2 address: %d\n", settings->outputs[1].eq[1].sigma_dsp_address);
    printf("Output2 eq3 address: %d\n", settings->outputs[1].eq[2].sigma_dsp_address);
    printf("Output2 eq4 address: %d\n", settings->outputs[1].eq[3].sigma_dsp_address);
    printf("Output2 eq5 address: %d\n", settings->outputs[1].eq[4].sigma_dsp_address);

    printf("Output3 eq1 address: %d\n", settings->outputs[2].eq[0].sigma_dsp_address);
    printf("Output3 eq2 address: %d\n", settings->outputs[2].eq[1].sigma_dsp_address);
    printf("Output3 eq3 address: %d\n", settings->outputs[2].eq[2].sigma_dsp_address);
    printf("Output3 eq4 address: %d\n", settings->outputs[2].eq[3].sigma_dsp_address);
    printf("Output3 eq5 address: %d\n", settings->outputs[2].eq[4].sigma_dsp_address);

    printf("Output4 eq1 address: %d\n", settings->outputs[3].eq[0].sigma_dsp_address);
    printf("Output4 eq2 address: %d\n", settings->outputs[3].eq[1].sigma_dsp_address);
    printf("Output4 eq3 address: %d\n", settings->outputs[3].eq[2].sigma_dsp_address);
    printf("Output4 eq4 address: %d\n", settings->outputs[3].eq[3].sigma_dsp_address);
    printf("Output4 eq5 address: %d\n", settings->outputs[3].eq[4].sigma_dsp_address);


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
    test_eq.sigma_dsp_address = MOD_INPUT1_EQ_ALG0_STAGE0_B0_ADDR;

    mux_t mux;
    mux.index = MUX_SELECT_INPUT1;
    mux.sigma_dsp_address = MOD_OUTPUT1_SELECT_MONOSWSLEW_ADDR;

    dsp_control_eq_secondorder(&test_eq);
    dsp_control_mux(&mux);

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

