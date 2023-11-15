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
// TODO: Change adding to i2c command. Can be done without loop.
/******************************* INCLUDES ********************************/

#include "sigma_dsp.h"
#include "sigma_dsp_program_data.h"

/******************************* GLOBAL VARIABLES ************************/

sigma_dsp_t *sigma_dsp = NULL;

static const char *TAG = "Sigma_dsp";

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
            i2c_cmd_link_delete(cmd);
            return ACK_POLL_SUCCESS;
        }
        if((xTaskGetTickCount() - start) > (ACK_POLL_TIMEOUT / portTICK_PERIOD_MS))
        {
            i2c_cmd_link_delete(cmd);
            return ACK_POLL_FAILED;
        }
    }
    i2c_cmd_link_delete(cmd);
    return ACK_POLL_FAILED;
}

bool load_program(void)
{
    if(sigma_dsp != NULL)
    {
        // Load dsp_core_register_R0_data
        if(!(sigma_dsp_write_burst(CORE_REGISTER_R0_ADDR,
                                   CORE_REGISTER_R0_SIZE,
                                   (uint8_t*)dsp_core_register_R0_data) 
                                   == SIGMA_DSP_WRITE_SUCCESS))
        {
            return false;
        }
        // Load the program data:
        if(!(sigma_dsp_write_burst(PROGRAM_ADDR,
                                   PROGRAM_SIZE,
                                   (uint8_t*)dsp_program_data) 
                                   == SIGMA_DSP_WRITE_SUCCESS))
        {
            return false;
        }
        // Load the parameter data:
        if(!(sigma_dsp_write_burst(PARAMETER_ADDR,
                                   PARAMETER_SIZE,
                                   (uint8_t*)dsp_parameter_data) 
                                   == SIGMA_DSP_WRITE_SUCCESS))
        {
            return false;
        }
        // Load dsp_hardware_conf_data
        if(!(sigma_dsp_write_burst(HARDWARE_CONF_ADDR,
                                   HARDWARE_CONF_SIZE,
                                   (uint8_t*)dsp_hardware_conf_data) 
                                   == SIGMA_DSP_WRITE_SUCCESS))
        {
            return false;
        }
        // Load dsp_core_register_R4_data
        if(!(sigma_dsp_write_burst(CORE_REGISTER_R4_ADDR,
                                   CORE_REGISTER_R4_SIZE,
                                   (uint8_t*)dsp_core_register_R4_data) 
                                   == SIGMA_DSP_WRITE_SUCCESS))
        {
            return false;
        }
    }
    return true;
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
                    if(load_program())
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

uint8_t sigma_dsp_write_burst(uint16_t reg_address, 
                              uint16_t len, 
                              uint8_t *data)
{
    if(sigma_dsp != NULL)
    {
        if(ack_poll())
        {
            uint8_t address_low  = reg_address;
            uint8_t address_high = reg_address >> 8;

            i2c_cmd_handle_t cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);
            // Control byte -->
            i2c_master_write_byte(cmd, 
                                (sigma_dsp->sigma_dsp_address << 1) | WRITE_BIT, 
                                ACK_CHECK_EN);
            i2c_master_write_byte(cmd, address_high, ACK_CHECK_EN);
            i2c_master_write_byte(cmd, address_low,  ACK_CHECK_EN);

            for(int i = 0; i < len; i++)
            {
                i2c_master_write_byte(cmd, data[i], ACK_CHECK_EN);
            }
            i2c_master_stop(cmd);

            esp_err_t ret = i2c_master_cmd_begin(sigma_dsp->i2c_port_num, 
                                                cmd, 
                                                I2C_TIMEOUT_MS / portTICK_PERIOD_MS);

            i2c_cmd_link_delete(cmd);

            if(ret == ESP_OK)
            {
                return SIGMA_DSP_WRITE_SUCCESS;
            }
        }
    }
    ESP_LOGW(TAG, "Burst write failed!");
    return SIGMA_DSP_WRITE_FAILED;
}

/******************************* THE END *********************************/