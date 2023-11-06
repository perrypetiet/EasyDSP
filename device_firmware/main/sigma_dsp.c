/*
 * sigma_dsp.c
 *
 * Created: 30-10-2023 
 * Author: Perry Petiet
 * 
 * This module is used to communicate with the ADAU1701 DSP chip. It 
 * loads the initial DSP program from the sigma studio software using
 * the exported parameters. The parameters can be found in 
 * sigma_studio_export.h.
 * 
 * 
 */ 
/******************************* INCLUDES ********************************/

#include "sigma_dsp.h"

/******************************* GLOBAL VARIABLES ************************/

sigma_dsp_t *sigma_dsp = NULL;

static const char *TAG = "Sigma dsp";

/******************************* LOCAL FUNCTIONS *************************/

esp_err_t i2c_master_init(uint8_t i2c_scl_gpio, 
                          uint8_t i2c_sda_gpio, 
                          uint8_t i2c_port_num,
                          bool    internal_pullup)
{
    uint8_t i2c_master_port = i2c_port_num;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = i2c_sda_gpio,
        .scl_io_num = i2c_scl_gpio,
        .sda_pullup_en = internal_pullup,
        .scl_pullup_en = internal_pullup,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, 
                              conf.mode, 
                              I2C_MASTER_RX_BUF_DISABLE, 
                              I2C_MASTER_TX_BUF_DISABLE, 
                              0);
}

uint8_t ack_poll(void)
{
    long start = xTaskGetTickCount();

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 
                          (sigma_dsp->sigma_dsp_address << 1) | WRITE_BIT, 
                          ACK_CHECK_EN);
    i2c_master_stop(cmd); 

    while(1)
    {
        esp_err_t ret = i2c_master_cmd_begin(sigma_dsp->i2c_port_num, 
                                             cmd, 
                                             I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
        if(ret == ESP_OK)
        {
            return ACK_POLL_SUCCESS;
        }
        if((xTaskGetTickCount() - start) > (ACK_POLL_TIMEOUT / portTICK_PERIOD_MS))
        {
            return ACK_POLL_FAILED;
        }
    }
    return ACK_POLL_FAILED;
}


/******************************* GLOBAL FUNCTIONS ************************/

uint8_t init_sigma_dsp(uint8_t i2c_scl_gpio,
                       uint8_t i2c_sda_gpio,
                       uint8_t i2c_port_num,
                       uint8_t sigma_dsp_address,
                       gpio_num_t reset_pin)
{
    sigma_dsp = malloc(sizeof(sigma_dsp_t));

    if(sigma_dsp != NULL)
    {
        sigma_dsp->i2c_scl_gpio      = i2c_scl_gpio;
        sigma_dsp->i2c_sda_gpio      = i2c_sda_gpio;
        sigma_dsp->i2c_port_num      = i2c_port_num;
        sigma_dsp->sigma_dsp_address = sigma_dsp_address;
        sigma_dsp->reset_pin         = reset_pin;

        if(i2c_master_init(i2c_scl_gpio,
                           i2c_sda_gpio,
                           i2c_port_num,
                           false) == ESP_OK)
        {
            if(gpio_set_direction(reset_pin, GPIO_MODE_OUTPUT) == ESP_OK)
            {
                if(set_reset_pin(1))
                {
                    if(ack_poll())
                    {
                        ESP_LOGI(TAG, "Sigma DSP init success!");
                        return SIGMA_DSP_INIT_SUCCESS;
                    }
                }
            }
        }
    }
    free(sigma_dsp);
    sigma_dsp = NULL;
    ESP_LOGW(TAG, "Sigma DSP init failed!");
    return SIGMA_DSP_INIT_FAILED;
}

uint8_t deinit_sigma_dsp()
{
    if(sigma_dsp != NULL)
    {
        free(sigma_dsp);
        sigma_dsp = NULL;
        ESP_LOGI(TAG, "Sigma dsp deinit success!");
    }
    ESP_LOGW(TAG, "sigma dsp deinit failed!");
    return SIGMA_DSP_DEINIT_FAILED;
}

bool set_reset_pin(uint8_t level)
{
    if(sigma_dsp != NULL)
    {
        if(gpio_set_level(sigma_dsp->reset_pin, level) == ESP_OK)
        {
            return true;
        }
    }
    return false;
}


/******************************* THE END *********************************/