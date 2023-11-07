/*
 * eeprom.c
 *
 * Created: 26-10-2023 
 * Author: Perry Petiet
 * 
 * Library to write and read data to and from a 24LC128 EEPROM.
 * 
 */ 
/******************************* INCLUDES ********************************/

#include "eeprom.h"

/******************************* GLOBAL VARIABLES ************************/

static const char *TAG = "eeprom_control";

eeprom_data *eeprom = NULL;

/******************************* LOCAL FUNCTIONS *************************/

esp_err_t eeprom_i2c_master_init(uint8_t i2c_scl_gpio, 
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


uint8_t eeprom_ack_poll(void)
{
    long start = xTaskGetTickCount();

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 
                          (eeprom->eeprom_address << 1) | WRITE_BIT, 
                          ACK_CHECK_EN);
    i2c_master_stop(cmd); 

    while(1)
    {
        esp_err_t ret = i2c_master_cmd_begin(eeprom->i2c_port_num, 
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

// depricated
void write_cycle_task_hold(void)
{
    vTaskDelay(EEPROM_WRITE_CYCLE_MS / portTICK_PERIOD_MS);
}

/******************************* GLOBAL FUNCTIONS ************************/

uint8_t init_eeprom(uint8_t i2c_scl_gpio, 
                    uint8_t i2c_sda_gpio, 
                    uint8_t i2c_port_num,
                    uint8_t eeprom_address)
{
    eeprom = malloc(sizeof(eeprom_data));

    if(eeprom != NULL)
    {
        eeprom->i2c_scl_gpio   = i2c_scl_gpio;
        eeprom->i2c_sda_gpio   = i2c_sda_gpio;
        eeprom->i2c_port_num   = i2c_port_num;
        eeprom->eeprom_address = eeprom_address;

        esp_err_t err_code = eeprom_i2c_master_init(i2c_scl_gpio,
                                                    i2c_sda_gpio,
                                                    i2c_port_num,
                                                    false);

        if(err_code == ESP_OK)
        {
            // Let's poll the i2c line to see if we get a response.
            if(eeprom_ack_poll())
            {
                ESP_LOGI(TAG, "EEPROM init succes!");
                return EEPROM_INIT_SUCCESS;
            }
        }
    }
    free(eeprom);
    eeprom = NULL;
    ESP_LOGW(TAG, "EEPROM init failed!");
    return EEPROM_INIT_FAILED;
}

uint8_t deinit_eeprom()
{
    if(eeprom != NULL)
    {
        i2c_driver_delete(eeprom->i2c_port_num);
        free(eeprom);
        eeprom = NULL;
        return EEPROM_DEINIT_SUCCESS;
    }
    return EEPROM_DEINIT_FAILED;
}

uint8_t eeprom_read_random_byte(uint16_t data_address, 
                                uint8_t  *rx_data)
{
    if(eeprom != NULL)
    {   
        if(eeprom_ack_poll())
        {
            uint8_t address_low  = data_address;
            uint8_t address_high = data_address >> 8;

            i2c_cmd_handle_t cmd = i2c_cmd_link_create();

            i2c_master_start(cmd);
            // Control byte ->
            i2c_master_write_byte(cmd, 
                                (eeprom->eeprom_address << 1) | WRITE_BIT, 
                                ACK_CHECK_EN);
            i2c_master_write_byte(cmd, address_high, ACK_CHECK_EN);
            i2c_master_write_byte(cmd, address_low,  ACK_CHECK_EN);
            i2c_master_start(cmd);
            // Control byte ->
            i2c_master_write_byte(cmd, 
                                (eeprom->eeprom_address << 1) | READ_BIT, 
                                ACK_CHECK_EN);
            i2c_master_read_byte(cmd, rx_data, NACK_VAL);
            i2c_master_stop(cmd); 

            esp_err_t ret = i2c_master_cmd_begin(eeprom->i2c_port_num, 
                                                cmd, 
                                                I2C_TIMEOUT_MS / portTICK_PERIOD_MS);

            i2c_cmd_link_delete(cmd);

            if(ret == ESP_OK)
            {
                return EEPROM_READ_SUCCESS;
            }
        }
    }
    ESP_LOGW(TAG, "EEPROM read failed.");
    return EEPROM_READ_FAILED;
}

uint8_t eeprom_current_address_read(uint8_t  *rx_data)
{
    if(eeprom != NULL)
    {
        if(eeprom_ack_poll())
        {
            i2c_cmd_handle_t cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);
            // Control byte ->
            i2c_master_write_byte(cmd, 
                                (eeprom->eeprom_address << 1) | READ_BIT, 
                                ACK_CHECK_EN);

            i2c_master_read_byte(cmd, rx_data, NACK_VAL);
            i2c_master_stop(cmd); 

            esp_err_t ret = i2c_master_cmd_begin(eeprom->i2c_port_num, 
                                                cmd, 
                                                I2C_TIMEOUT_MS / portTICK_PERIOD_MS);

            i2c_cmd_link_delete(cmd);

            if(ret == ESP_OK)
            {
                return EEPROM_READ_SUCCESS;
            }
        }   
    }
    ESP_LOGW(TAG, "EEPROM read failed.");
    return EEPROM_READ_FAILED;
}

uint8_t eeprom_sequential_read(uint16_t data_address,
                               uint8_t  *rx_data,
                               uint16_t len)
{
    if(eeprom != NULL)
    {
        if(eeprom_ack_poll())
        {
            if(len > 1)
            {
                uint8_t address_low  = data_address;
                uint8_t address_high = data_address >> 8;

                i2c_cmd_handle_t cmd = i2c_cmd_link_create();

                i2c_master_start(cmd);
                // Control byte ->
                i2c_master_write_byte(cmd, 
                                    (eeprom->eeprom_address << 1) | WRITE_BIT, 
                                    ACK_CHECK_EN);
                i2c_master_write_byte(cmd, address_high, ACK_CHECK_EN);
                i2c_master_write_byte(cmd, address_low,  ACK_CHECK_EN);
                i2c_master_start(cmd);
                // Control byte ->
                i2c_master_write_byte(cmd, 
                                    (eeprom->eeprom_address << 1) | READ_BIT, 
                                    ACK_CHECK_EN);

                // We now sequentially read every byte from EEPROM
                // Every byte is ended with an ACK. The last byte is ended with
                // NACK and stop bit.
                for(int i = 0; i < len - 1; i++)
                {
                    i2c_master_read_byte(cmd, rx_data + i, ACK_VAL);
                }
                i2c_master_read_byte(cmd, rx_data + (len - 1), NACK_VAL);
                i2c_master_stop(cmd);
                esp_err_t ret = i2c_master_cmd_begin(eeprom->i2c_port_num, 
                                                cmd, 
                                                I2C_TIMEOUT_MS / portTICK_PERIOD_MS);

                i2c_cmd_link_delete(cmd);

                if(ret == ESP_OK)
                {
                    return EEPROM_READ_SUCCESS;
                } 
            }
        }
    }
    ESP_LOGW(TAG, "EEPROM read failed.");
    return EEPROM_READ_FAILED;
}

uint8_t eeprom_write_random_byte(uint16_t data_address, 
                                 uint8_t  tx_data)
{
    if(eeprom != NULL)
    {
        if(eeprom_ack_poll())
        {
            uint8_t address_low  = data_address;
            uint8_t address_high = data_address >> 8;

            i2c_cmd_handle_t cmd = i2c_cmd_link_create();

            i2c_master_start(cmd);
            // Control byte ->
            i2c_master_write_byte(cmd, 
                                (eeprom->eeprom_address << 1) | WRITE_BIT, 
                                ACK_CHECK_EN);
            i2c_master_write_byte(cmd, address_high, ACK_CHECK_EN);
            i2c_master_write_byte(cmd, address_low,  ACK_CHECK_EN);
            i2c_master_write_byte(cmd, tx_data, ACK_CHECK_EN);
            i2c_master_stop(cmd);

            esp_err_t ret = i2c_master_cmd_begin(eeprom->i2c_port_num, 
                                                cmd, 
                                                I2C_TIMEOUT_MS / portTICK_PERIOD_MS);

            i2c_cmd_link_delete(cmd);

            if(ret == ESP_OK)
            {   
                return EEPROM_WRITE_SUCCESS;
            } 
        }
    }
    return EEPROM_WRITE_FAILED;
}

uint8_t eeprom_write_page(uint16_t page_address, 
                          uint8_t  *tx_data, 
                          uint8_t  len)
{
    if(eeprom != NULL)
    {
        if(eeprom_ack_poll())
        {
            if((len <= EEPROM_PAGE_SIZE) && (len > 1)) 
            {
                //Check if given address is a page start address.
                if((page_address % EEPROM_PAGE_SIZE)  == 0)
                {
                    uint8_t address_low  = page_address;
                    uint8_t address_high = page_address >> 8;

                    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

                    i2c_master_start(cmd);
                    // Control byte ->
                    i2c_master_write_byte(cmd, 
                                        (eeprom->eeprom_address << 1) | WRITE_BIT, 
                                        ACK_CHECK_EN);
                    i2c_master_write_byte(cmd, address_high, ACK_CHECK_EN);
                    i2c_master_write_byte(cmd, address_low,  ACK_CHECK_EN);
                    for(int i = 0; i < len; i++)
                    {
                        i2c_master_write_byte(cmd, tx_data[i], ACK_CHECK_EN);
                    }
                    i2c_master_stop(cmd);

                    esp_err_t ret = i2c_master_cmd_begin(eeprom->i2c_port_num, 
                                                cmd, 
                                                I2C_TIMEOUT_MS / portTICK_PERIOD_MS);

                    i2c_cmd_link_delete(cmd);

                    if(ret == ESP_OK)
                    {   
                        return EEPROM_WRITE_SUCCESS;
                    }        
                }
            }
        }
    }
    return EEPROM_WRITE_FAILED;
}

/******************************* THE END *********************************/