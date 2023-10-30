/*
 * device_settings.c
 *
 * Created: 9-8-2023 
 * Author: Perry Petiet
 * 
 * The device_settings library contains the settings of the device. The
 * settings for the device are stored in a datatype. This datatype can be 
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
        ESP_LOGI(TAG, "Device settings init succes.");

        device_settings->input1.eq1.q = 0.0;
        device_settings->input1.eq1.filter_type = FILTER_TYPE_PEAK; 

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
        device_settings->input1.eq1.freq  = 1000;
        device_settings->input1.eq2.freq  = 1000;
        device_settings->input1.eq3.freq  = 1000;
        device_settings->input1.eq4.freq  = 1000;
        device_settings->input1.eq5.freq  = 1000;

        device_settings->input2.eq1.freq  = 1000;
        device_settings->input2.eq2.freq  = 1000;
        device_settings->input2.eq3.freq  = 1000;
        device_settings->input2.eq4.freq  = 1000;
        device_settings->input2.eq5.freq  = 1000;

        device_settings->output1.eq1.freq = 1000;
        device_settings->output1.eq2.freq = 1000;
        device_settings->output1.eq3.freq = 1000;
        device_settings->output1.eq4.freq = 1000;
        device_settings->output1.eq5.freq = 1000;

        device_settings->output2.eq1.freq = 1000;
        device_settings->output2.eq2.freq = 1000;
        device_settings->output2.eq3.freq = 1000;
        device_settings->output2.eq4.freq = 1000;
        device_settings->output2.eq5.freq = 1000;

        device_settings->output3.eq1.freq = 1000;
        device_settings->output3.eq2.freq = 1000;
        device_settings->output3.eq3.freq = 1000;
        device_settings->output3.eq4.freq = 1000;
        device_settings->output3.eq5.freq = 1000;

        device_settings->output4.eq1.freq = 1000;
        device_settings->output4.eq2.freq = 1000;
        device_settings->output4.eq3.freq = 1000;
        device_settings->output4.eq4.freq = 1000;
        device_settings->output4.eq5.freq = 1000;
    }

    return 0;
}

uint8_t device_settings_store_nv()
{
    bool write_success = true;
    if(device_settings != NULL)
    {
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