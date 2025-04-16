#ifndef MANIKIN_I2C_FUNCTIONS_H
#define MANIKIN_I2C_FUNCTIONS_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stddef.h>
#include "manikin_software_conf.h"

int i2c_hal_init(manikin_i2c_inst_t i2c_inst, uint32_t baud);

uint8_t i2c_hal_device_acknowledge(manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr);

uint32_t i2c_hal_error_flag_check(manikin_i2c_inst_t i2c_inst);

size_t i2c_hal_read_bytes(manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr, uint8_t* data, size_t len);

size_t i2c_hal_write_bytes(manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr, uint8_t* data, size_t len);

int i2c_hal_deinit(manikin_i2c_inst_t i2c_inst);

#ifdef __cplusplus
}
#endif

#endif /* MANIKIN_I2C_FUNCTIONS_H */