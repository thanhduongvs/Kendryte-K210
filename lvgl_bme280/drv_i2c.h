#ifndef _DRV_I2C_H
#define _DRV_I2C_H

#include <stdint.h>
#include <i2c.h>

#define i2c_number          I2C_DEVICE_1
#define scl_func            FUNC_I2C1_SCLK
#define sda_func            FUNC_I2C1_SDA

//#define slave_address       (0x76 << 1)
#define slave_address       0x76
#define slave_width         0x07
#define slave_clock         10000 //Hz

#define scl_pin             25
#define sda_pin             24

typedef enum {
	i2c_error   = 0,
	i2c_success = !i2c_error
} i2c_result;

void i2c_mux_init(void);
void i2c_master_init(void);

uint8_t i2c_read(i2c_device_number_t i2c_num, uint8_t reg);
uint8_t i2c_write(i2c_device_number_t i2c_num, uint8_t reg, uint8_t val);

#endif /* _DRV_I2C_H */