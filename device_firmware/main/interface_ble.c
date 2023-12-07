/*
 * interface_ble.c
 *
 * Created: 30-11-2023 
 * Author: Perry Petiet
 *
 * This module contains the functions to initaliaze and use the BLE
 * functionality on the ESP32 as a GATT device. It initalises 1 service
 * with a read and write characteristic to make it a "serial" device.
 * 
 */
/******************************* INCLUDES ********************************/

#include "interface_ble.h" 

/******************************* GLOBAL VARIABLES ************************/

static const char *TAG = "interface_ble";

/******************************* LOCAL FUNCTIONS *************************/

/******************************* GLOBAL FUNCTIONS ************************/

bool init_interface_ble()
{
    esp_err_t ret;
    

    // Initialize NVS. We need to do this in order for BLE to work. It 
    // saves things like calibration data :(
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "%s initialize controller failed", __func__);
        return false;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "%s enable controller failed", __func__);
        return false;
    }

    //esp_bluedroid_config_t bluedroid_cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    ret = esp_bluedroid_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "%s init bluetooth failed", __func__);
        return false;
    }
    ret = esp_bluedroid_enable();
    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "%s enable bluetooth failed", __func__);
        return false;
    }
    return true;
}

/******************************* THE END *********************************/