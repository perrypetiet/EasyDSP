/*
 * device_settings.c
 *
 * Created: 9-8-2023 
 * Author: Perry Petiet
 * 
 * The device_settings library contains the settings of the device. The
 * settings for the device are stored in a datatype. This datatype can be 
 *
 *  TODO: On initializing of the device settings, if the EEPROM is not 
 *  responding, we should load factory data to the local settings. Also, we
 *  should add a CRC checksum to see if data is valid. If not, load 
 *  factory to local and write local to EEPROM.
 * 
 */ 
/******************************* INCLUDES ********************************/

#include "device_settings.h"

/******************************* GLOBAL VARIABLES ************************/

device_settings_t *device_settings = NULL;

static const char *TAG = "Device_settings";

/******************************* LOCAL FUNCTIONS *************************/

/******************************* GLOBAL FUNCTIONS ************************/


uint8_t init_device_settings()
{
    bool eeprom_found = false;
    
    if(init_eeprom(NV_STORAGE_SCL_PIN,
                   NV_STORAGE_SDA_PIN,
                   NV_STORAGE_I2C_INTERFACE,
                   NV_STORAGE_I2C_ADDRESS))
    {
        eeprom_found = true;
    }
    
    device_settings = malloc(sizeof(device_settings_t));

    if(device_settings != NULL)
    {

        if(eeprom_found)
        {
            // Load from EEPROM.
        }
        else
        {
            // Load factory settings
        }

        ESP_LOGI(TAG, "Device settings init succes."); 

        return INIT_DEVICE_SETTINGS_SUCCESS;
    }
    
    deinit_eeprom();
    free(device_settings);
    device_settings = NULL;
    ESP_LOGW(TAG, "Device settings init failed.");
    return INIT_DEVICE_SETTINGS_FAILED;
}

uint8_t deinit_device_settings()
{
    if(device_settings != NULL)
    {
        free(device_settings);
        device_settings = NULL;
        return DEINIT_DEVICE_SETTINGS_SUCCESS;
    }
    return DEINIT_DEVICE_SETTINGS_FAILED;
}

device_settings_t* get_device_settings_address()
{
    return device_settings;
}

uint8_t device_settings_load_factory()
{
    if(device_settings != NULL)
    {
        // Set memory block to 0
        memset(device_settings, 0, sizeof(device_settings_t));
        // Manually set settings that shouldn't be 0
        strcpy(device_settings->device_name, "EasyDSP");
        device_settings->device_name_len = 7;

        // Input settings
        for(int i = 0; i < DEVICE_SETTINGS_INPUT_AMOUNT; i++)
        {
            for(int j = 0; j < DEVICE_SETTINGS_INPUT_EQ_AMOUNT; j++)
            {
                device_settings->inputs[i].eq[j].sigma_dsp_address = 
                    MOD_INPUT1_EQ_ALG0_STAGE0_B0_ADDR + 
                    (j * ADA_COEFFICIENT_AMOUNT) + 
                    (i * MOD_INPUT1_EQ_COUNT );

                device_settings->inputs[i].eq[j].freq         = 1000;
                device_settings->inputs[i].eq[j].q            = 1.41;
                device_settings->outputs[i].eq[j].filter_type = FILTER_TYPE_PEAK;
            }
        }

        // Output settings
        // TODO: add gain to factory settings.
        for(int i = 0; i < DEVICE_SETTINGS_OUTPUT_AMOUNT; i++)
        {
            device_settings->outputs[i].mux.index = MUX_SELECT_INPUT1;

            // Set the dsp address to the first mux address + output n
            device_settings->outputs[i].mux.sigma_dsp_address = 
                MOD_OUTPUT1_SELECT_MONOSWSLEW_ADDR + i;

            for(int j = 0; j < DEVICE_SETTINGS_OUTPUT_EQ_AMOUNT; j++)
            {
                device_settings->outputs[i].eq[j].sigma_dsp_address = 
                    MOD_OUTPUT1_EQ_ALG0_STAGE0_B0_ADDR + 
                    (j * ADA_COEFFICIENT_AMOUNT) + 
                    (i * MOD_OUTPUT1_EQ_COUNT);

                device_settings->outputs[i].eq[j].freq        = 1000;
                device_settings->outputs[i].eq[j].q           = 1.41;
                device_settings->outputs[i].eq[j].filter_type = FILTER_TYPE_PEAK;
            }
        }

        ESP_LOGI(TAG, "Loaded factory settings.");
        return 1;
    }
    return 0;
}

uint8_t device_settings_store_nv()
{
    bool write_success = true;
    if(device_settings != NULL)
    {
        // The EEPROM used can only write one page at once.
        // Let's see how many pages we have, we then program page for page.
        uint16_t page_amount = sizeof(device_settings_t) / EEPROM_PAGE_SIZE;
        uint8_t  remainder = sizeof(device_settings_t) % EEPROM_PAGE_SIZE;
        
        printf("Size of settings: %d\n", sizeof(device_settings_t));
        printf("Amount of pages: %d\n", page_amount);
        printf("Remaining bytes: %d\n", remainder);
        
        for(int i = 0; i < page_amount; i++)
        {
            if(!eeprom_write_page(NV_STORAGE_SETTINGS_ADDRESS + (i * EEPROM_PAGE_SIZE), 
                                  (uint8_t*)device_settings + (i * EEPROM_PAGE_SIZE), 
                                  EEPROM_PAGE_SIZE))
            {
                write_success = false;
            }
        }
        // Remaining bytes:
        if(!eeprom_write_page(NV_STORAGE_SETTINGS_ADDRESS + (page_amount * (EEPROM_PAGE_SIZE)),
                             (uint8_t*)device_settings + (page_amount * EEPROM_PAGE_SIZE),
                             remainder))
        {
            write_success = false;
        }

        if(!write_success)
        {
            ESP_LOGW(TAG, "Failed to write settings to NV storage!");
        }
        else 
        {
            ESP_LOGI(TAG, "Wrote settings to NV storage!");
        }
    }   
    return (uint8_t)write_success;
}

uint8_t device_settings_load_nv()
{
    if(device_settings != NULL)
    {
        // Do a sequential read for all settings starting on device_settings pointer.
        if(eeprom_sequential_read(NV_STORAGE_SETTINGS_ADDRESS,
                                  (uint8_t*)device_settings,
                                  sizeof(device_settings_t)))
        {
            ESP_LOGI(TAG, "Loaded device settings from NV storage.");
            return NV_RW_SUCCESS;
        }
    }
    ESP_LOGW(TAG, "Failed to load device settings from NV storage");
    return NV_RW_FAILED;
}

/******************************* THE END *********************************/