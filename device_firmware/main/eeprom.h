/*
 * eeprom.h
 *
 * Created: 26-10-2023 
 * Author: Perry Petiet
 * 
 * Library to write and read data to and from a 24LC128 EEPROM.
 * 
 */ 
#ifndef EEPROM_H_
#define EEPROM_H_

/******************************* INCLUDES ********************************/

#include <stdio.h>
#include <stdbool.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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

#define EEPROM_WRITE_CYCLE_MS       10
#define EEPROM_PAGE_SIZE            64
#define EEPROM_WRITE_SUCCESS        1
#define EEPROM_WRITE_FAILED         0
#define EEPROM_READ_SUCCESS         1
#define EEPROM_READ_FAILED          0 

#define ACK_POLL_TIMEOUT            1000
#define ACK_POLL_SUCCESS            1
#define ACK_POLL_FAILED             0

#define EEPROM_INIT_SUCCESS         1
#define EEPROM_INIT_FAILED          0
#define EEPROM_DEINIT_SUCCESS       1
#define EEPROM_DEINIT_FAILED        0

/******************************* TYPEDEFS ********************************/

typedef struct
{
    uint8_t i2c_scl_gpio;
    uint8_t i2c_sda_gpio;
    uint8_t i2c_port_num;

    uint8_t eeprom_address;
} eeprom_data;

/******************************* LOCAL FUNCTIONS *************************/

esp_err_t i2c_master_init(uint8_t i2c_scl_gpio,
                          uint8_t i2c_sda_gpio,
                          uint8_t i2c_port_num,
                          bool    internal_pullup);

uint8_t ack_poll(void);

void write_cycle_task_hold(void);

/******************************* GLOBAL FUNCTIONS ************************/

uint8_t init_eeprom(uint8_t i2c_scl_gpio,
                    uint8_t i2c_sda_gpio,
                    uint8_t i2c_port_num,
                    uint8_t eeprom_address);

uint8_t deinit_eeprom();

uint8_t eeprom_read_random_byte(uint16_t data_address, 
                                uint8_t  *rx_data);

uint8_t eeprom_current_address_read(uint8_t *rx_data);

uint8_t eeprom_sequential_read(uint16_t data_address,
                               uint8_t *rx_data,
                               uint16_t len);

uint8_t eeprom_write_random_byte(uint16_t data_address, uint8_t tx_data);

uint8_t eeprom_write_page(uint16_t page_address, 
                          uint8_t  *tx_data, 
                          uint8_t  len);

/******************************* THE END *********************************/

#endif /* EEPROM_H_ */