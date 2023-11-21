/*
 * dsp_control.h
 *
 * Created: 8-11-2023 
 * Author: Perry Petiet
 * 
 * This module is used to control the DSP on a high level.
 * It initializes the sigma_dsp module and contains functions to
 * calculate the correct parameter data for eq's and muxes. the
 * functions take own defined datatypes and send the data to program
 * the sigma dsp to the sigma_dsp module which then uses the safeload
 * write functionality of the dsp chip.
 * 
 */ 
#ifndef DSP_CONTROL_H_
#define DSP_CONTROL_H_

/******************************* INCLUDES ********************************/

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "sigma_dsp.h"
#include "device_settings.h"
#include "driver/gpio.h"
#include "esp_log.h"

/******************************* DEFINES *********************************/

#define ADA_I2C_SCL_PIN  12
#define ADA_I2C_SDA_PIN  13
#define ADA_I2C_PORT_NUM 0
#define ADA_I2C_ADDRESS  (0x68 >> 1) & 0xFE
#define ADA_GPIO_RESET   GPIO_NUM_16

#define ADA_SAMPLE_FREQ  48000

#define ADA_PARAM_REG_SIZE     4
#define ADA_COEFFICIENT_AMOUNT 5

/******************************* TYPEDEFS ********************************/

// typedef struct
// {
//     uint16_t addresses[DEVICE_SETTINGS_INPUT_EQ_AMOUNT]
// } input_eq_addresses_t;

// typedef struct
// {  
//     uint16_t addresses[DEVICE_SETTINGS_OUTPUT_EQ_AMOUNT]
// } output_eq_addresses_t;


// typedef struct
// {
//     input_eq_addresses_t inputs_eq_addresses[DEVICE_SETTINGS_INPUT_AMOUNT];

//     output_eq _addresses_t outputs_eq_addresses[DEVICE_SETTINGS_OUTPUT_AMOUNT];
// } module_addresses_t;

/******************************* LOCAL FUNCTIONS *************************/

/******************************* GLOBAL FUNCTIONS ************************/

bool init_dsp_control(void);

bool deinit_dsp_control(void);

bool dsp_control_mux(mux_t *mux);

bool dsp_control_eq_secondorder(equalizer_t *eq);

//TODO: add gain adjustment support

/******************************* THE END *********************************/
#endif /* DSP_CONTROL_H_ */