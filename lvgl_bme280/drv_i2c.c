#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "fpioa.h"
#include "gpiohs.h"
#include "i2c.h"
#include "utils.h"
#include "sysctl.h"
#include "drv_i2c.h"

//#define I2C_DEBUG

#ifdef I2C_DEBUG
#define debug(fmt, ...) printf("%s: " fmt "\n", "I2C", ## __VA_ARGS__)
#else
#define debug(fmt, ...)
#endif

void i2c_mux_init(void)
{
    fpioa_set_function(scl_pin, scl_func);
    fpioa_set_function(sda_pin, sda_func);
}

void i2c_master_init(void)
{
    i2c_init(i2c_number, slave_address, slave_width, slave_clock);
}


// read a register of the MSA300 via i2c
uint8_t i2c_read(i2c_device_number_t i2c_num, uint8_t reg)
{

    uint8_t reg_val;
    int ret;
    ret = i2c_recv_data(i2c_num, &reg, 1, &reg_val, 1);
    
    if (ret != 0) {
        debug("Problem reading register 0x%x\n", reg);
        return -1;
    }
    
    return reg_val;
}


// set a register of the MSA300 via i2c
uint8_t i2c_write(i2c_device_number_t i2c_num, uint8_t reg, uint8_t val)
{
    uint8_t send_buf[2];
    int ret;

    send_buf[0] = reg;
    send_buf[1] = val;
    ret = i2c_send_data(i2c_num, send_buf, 2);
    if (ret != 0) {
        debug("Problem writing register 0x%x\n", reg);
        return -1;
    }
    return 0;
}
