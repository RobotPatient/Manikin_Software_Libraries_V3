#ifndef MANIKIN_TYPES_H
#define MANIKIN_TYPES_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stddef.h>
#include "manikin_software_conf.h"

    typedef enum
    {
        MANIKIN_STATUS_OK = 0,
        MANIKIN_STATUS_ERR_NULL_PARAM,
        MANIKIN_STATUS_ERR_INVALID_I2C_BAUD,
        MANIKIN_STATUS_ERR_WRITE_FAIL,
        MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL,
        MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL,
        MANIKIN_STATUS_ERR_PERIPHERAL_INIT_FAIL,
        MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG
    } manikin_status_t;

    typedef enum
    {
        MANIKIN_I2C_SPEED_NONE   = 0,
        MANIKIN_I2C_SPEED_100KHz = 100000,
        MANIKIN_I2C_SPEED_400KHz = 400000,
        MANIKIN_I2C_SPEED_1MHz   = 1000000,
    } manikin_i2c_speed_t;

#ifdef __cplusplus
}
#endif
#endif