/*
 * sigma_adressing.h
 *
 * Created: 3-12-2024 
 * Author: Perry Petiet
 * 
 * This module is used to convert the header file created by the
 * DSP_parameter_generator.ps1 to a structure that contains the addresses
 * in a way directly used by the settings module to get the correct
 * sigma dsp addresses when sending data to the dsp_control module.
 * 
 */
#ifndef DSP_ADDRESSING_H_
#define DSP_ADDRESSING_H_
/******************************* INCLUDES ********************************/

#include "sigma_dsp_module_data.h"
#include "device_settings.h"

/******************************* DEFINES *********************************/

/******************************* TYPEDEFS ********************************/

typedef struct
{
    uint32_t eq_addresses[DEVICE_SETTINGS_INPUT_EQ_AMOUNT];

} input_addresses_t;

typedef struct 
{
    uint32_t eq_addresses[DEVICE_SETTINGS_OUTPUT_EQ_AMOUNT];
    uint32_t mux;

} output_adresses_t;

typedef struct
{
    input_addresses_t input_addresses[DEVICE_SETTINGS_INPUT_AMOUNT];
    output_adresses_t output_addresses[DEVICE_SETTINGS_OUTPUT_AMOUNT];

} sigma_dsp_addresses_t;

/******************************* GLOBAL FUNCTIONS ************************/

/******************************* LOCAL FUNCTIONS *************************/

/******************************* THE END *********************************/
#endif /* DSP_ADDRESSING_H_ */