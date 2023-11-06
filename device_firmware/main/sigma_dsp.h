/*
 * sigma_dsp.h
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
#ifndef SIGMA_DSP_H_
#define SIGMA_DSP_H_

/******************************* INCLUDES ********************************/

#include <stdio.h>
#include <stdbool.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

/******************************* DEFINES *********************************/

#define I2C_MASTER_FREQ_HZ          400000 /* I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0      /* Disable buffer as master   */
#define I2C_MASTER_RX_BUF_DISABLE   0      /* Disable buffer as master   */
#define I2C_TIMEOUT_MS              1000

#define ACK_CHECK_EN                0x1    /* Check ACK from slave       */
#define ACK_CHECK_DIS               0x0    /* Do not chekc ACK from slave*/
#define ACK_VAL                     0x0    /* I2C ack value              */
#define NACK_VAL                    0x1    /* I2C nack value             */

#define WRITE_BIT                   0      /* I2C master write value     */
#define READ_BIT                    1      /* I2C master read value      */

#define ACK_POLL_TIMEOUT            1000
#define ACK_POLL_SUCCESS            1
#define ACK_POLL_FAILED             0

#define SIGMA_DSP_INIT_SUCCESS   1
#define SIGMA_DSP_INIT_FAILED    0 
#define SIGMA_DSP_DEINIT_SUCCESS 1
#define SIGMA_DSP_DEINIT_FAILED  0 


/******************************* TYPEDEFS ********************************/

typedef struct
{
    uint8_t i2c_scl_gpio;
    uint8_t i2c_sda_gpio;
    uint8_t i2c_port_num;

    gpio_num_t reset_pin;
    uint8_t sigma_dsp_address;
} sigma_dsp_t;

/******************************* LOCAL FUNCTIONS *************************/

esp_err_t i2c_master_init(uint8_t i2c_scl_gpio,
                          uint8_t i2c_sda_gpio,
                          uint8_t i2c_port_num,
                          bool    internal_pullup);

/******************************* GLOBAL FUNCTIONS ************************/

uint8_t init_sigma_dsp(uint8_t i2c_scl_gpio,
                       uint8_t i2c_sda_gpio,
                       uint8_t i2c_port_num,
                       uint8_t sigma_dsp_address,
                       gpio_num_t reset_pin);

uint8_t deinit_sigma_dsp();

bool set_reset_pin(uint8_t level);
/******************************* THE END *********************************/

#endif /* SIGMA_DSP_H_ */