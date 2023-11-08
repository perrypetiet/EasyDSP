/*
 * device_settings.h
 *
 * Created: 9-8-2023 
 * Author: Perry Petiet
 * 
 * The device_settings library contains the settings of the device. Using
 * simple functions we can set and get settings for the inputs, outputs and
 * routing block. In and output blocks contain eq and gain. The routing 
 * block settins contain to which output(s) and input is connected.
 * 
 */ 

#ifndef DEVICE_SETTINGS_H_
#define DEVICE_SETTINGS_H_

/******************************* INCLUDES ********************************/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "eeprom.h"

/******************************* DEFINES *********************************/

#define NV_STORAGE_SETTINGS_ADDRESS    0x40
#define NV_STORAGE_I2C_ADDRESS         0x50
#define NV_STORAGE_I2C_INTERFACE       1
#define NV_STORAGE_SCL_PIN             14
#define NV_STORAGE_SDA_PIN             15
#define NV_RW_SUCCESS                  1
#define NV_RW_FAILED                   0

#define INIT_DEVICE_SETTINGS_SUCCESS   1
#define INIT_DEVICE_SETTINGS_FAILED    0
#define DEINIT_DEVICE_SETTINGS_SUCCESS 1
#define DEINIT_DEVICE_SETTINGS_FAILED  0

#define DEVICE_SETTINGS_DEVICE_NAME_LEN  64  
#define DEVICE_SETTINGS_INPUT_AMOUNT     2
#define DEVICE_SETTINGS_OUTPUT_AMOUNT    4
#define DEVICE_SETTINGS_INPUT_EQ_AMOUNT  5
#define DEVICE_SETTINGS_OUTPUT_EQ_AMOUNT 5

#define FILTER_TYPE_PEAK      0
#define FILTER_TYPE_LOWSHELF  1
#define FILTER_TYPE_HIGHSHELF 2
#define FILTER_TYPE_LOWPASS   3
#define FILTER_TYPE_HIGHPASS  4
#define FILTER_TYPE_BANDPASS  5 
#define FILTER_TYPE_BANDSTOP  6

#define PHASE_NON_INVERTED 0
#define PHASE_INVERTED     1

#define STATE_OFF 0
#define STATE_ON  1

/******************************* TYPEDEFS ********************************/

typedef struct
{
    float q;
    float s;
    float bandwidth;
    float boost;
    float freq;
    float gain;

    uint8_t filter_type;
    uint8_t phase;
    uint8_t state;
} equalizer_t;

typedef struct
{
    uint8_t index;
} mux_t;

typedef struct
{
    equalizer_t eq[DEVICE_SETTINGS_INPUT_EQ_AMOUNT];
} input_t;

typedef struct
{
    equalizer_t eq[DEVICE_SETTINGS_OUTPUT_EQ_AMOUNT];
    mux_t       mux;
} output_t;

typedef struct
{
    uint8_t  device_name_len;
    char     device_name[DEVICE_SETTINGS_DEVICE_NAME_LEN];
    
    input_t  inputs[DEVICE_SETTINGS_INPUT_AMOUNT];
    output_t outputs[DEVICE_SETTINGS_OUTPUT_AMOUNT];
} device_settings_t;


/******************************* LOCAL FUNCTIONS *************************/

/******************************* GLOBAL FUNCTIONS ************************/

uint8_t init_device_settings();

uint8_t deinit_device_settings();

device_settings_t* get_device_settings_address();

uint8_t device_settings_load_factory();

uint8_t device_settings_store_nv();

uint8_t device_settings_load_nv();

/******************************* THE END *********************************/

#endif /* DEVICE_SETTINGS_H_ */