#include "manikin_i2c_functions.h"
#include "stm32f4xx_ll_i2c.h"
#include "stm32f4xx_hal.h"
static I2C_HandleTypeDef handle;
int i2c_hal_init(manikin_i2c_inst_t i2c_inst, uint32_t baud) {
    handle.Instance = i2c_inst;
    handle.Init.ClockSpeed = baud;
    handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
    handle.Init.OwnAddress1 = 0;
    handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    handle.Init.OwnAddress2 = 0;
    handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    return HAL_I2C_Init(&handle) != HAL_OK;
}

uint8_t i2c_hal_device_acknowledge(manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr) {
    return 0;
}

uint32_t i2c_hal_error_flag_check(manikin_i2c_inst_t i2c_inst) {
    return 0;
}

size_t i2c_hal_read_bytes(manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr, uint8_t* data, size_t len) {
    handle.Instance = i2c_inst;
    HAL_I2C_Master_Receive(&handle, i2c_addr, data, len, 10);
    return len;
}

size_t i2c_hal_write_bytes(manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr, uint8_t* data, size_t len) {
    handle.Instance = i2c_inst;
    HAL_I2C_Master_Transmit(&handle, i2c_addr, data, len, 10);
    return len;
}

int i2c_hal_deinit(manikin_i2c_inst_t i2c_inst) {
    handle.Instance = i2c_inst;
    return HAL_I2C_DeInit(&handle) != HAL_OK;
}