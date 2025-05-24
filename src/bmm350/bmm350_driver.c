/**
 * @file            bmm350.c
 * @brief           Driver implementation for a BMM350 3-axis magnetometer
 *
 * @par
 * Copyright 2025 (C) RobotPatient Simulators
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is part of the Manikin Software Libraries V3 project
 *
 * Author:          Victor Hogeweij
 */

#include "common/manikin_bit_manipulation.h"
#include "bmm350_driver.h"
#include "external/bmm350_defs.h"
#include <bmm350.h>
#include "i2c/i2c.h"
#include "bmm350_defs.h"
#include "manikin_platform.h"
#include "error_handler/error_handler.h"

#include <string.h>

#define HASH_BMM350 0x10C9F1D2u

static struct bmm350_dev dev;

/**
 * @brief Internal function to check the parameters entered into function
 * @param sensor_ctx The sensor settings ptr which should contain i2c bus details
 * @return - MANIKIN_STATUS_OK if all parameters are valid
 *         - MANIKIN_STATUS_ERR_NULL_PARAM if invalid
 */
static manikin_status_t
bmm350_check_params (const manikin_sensor_ctx_t *sensor_ctx)
{
    MANIKIN_ASSERT(HASH_BMM350, (sensor_ctx != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_BMM350, (sensor_ctx->i2c != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}

/*!
 * I2C read function map to COINES platform
 */
BMM350_INTF_RET_TYPE
bmm350_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    manikin_sensor_ctx_t *sensor_ctx = (manikin_sensor_ctx_t *)intf_ptr;

    size_t status
        = MANIKIN_I2C_HAL_WRITE_BYTES(sensor_ctx->i2c, sensor_ctx->i2c_addr, &reg_addr, 1);
    size_t res
        = MANIKIN_I2C_HAL_READ_BYTES(sensor_ctx->i2c, sensor_ctx->i2c_addr, reg_data, length);

    return res == length && status == 1 ? 0 : -1;
}
uint8_t write_buf[50];
/*!
 * I2C write function map to COINES platform
 */
BMM350_INTF_RET_TYPE
bmm350_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    manikin_sensor_ctx_t *sensor_ctx = (manikin_sensor_ctx_t *)intf_ptr;
    write_buf[0]                     = reg_addr;
    memcpy(write_buf + 1, reg_data, length);
    size_t res = MANIKIN_I2C_HAL_WRITE_BYTES(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, write_buf, (uint16_t)length + 1);
    return res == length + 1 ? 0 : -1;
}

void
bmm350_delay (uint32_t period, void *intf_ptr)
{
    (void)intf_ptr;
    MANIKIN_DELAY_US(period);
}

manikin_status_t
bmm350_init_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = bmm350_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BMM350, (status == MANIKIN_STATUS_OK), status);
    uint8_t  int_status, int_ctrl = 0;
    uint8_t  loop = 10;
    uint32_t secs, nano_secs = 0;

    // NOTE: needs_reinit is internal variable, which might be uninitialized when entered as param.
    sensor_ctx->needs_reinit = 0;
    dev.intf_ptr             = (void *)sensor_ctx;
    dev.read                 = bmm350_i2c_read;
    dev.write                = bmm350_i2c_write;
    dev.delay_us             = bmm350_delay;
    int8_t rslt              = bmm350_init(&dev);
    rslt                     = bmm350_configure_interrupt(
        BMM350_PULSED, BMM350_ACTIVE_HIGH, BMM350_INTR_PUSH_PULL, BMM350_UNMAP_FROM_PIN, &dev);
    if (rslt != BMM350_OK)
    {
        return MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL;
    }
    rslt = bmm350_enable_interrupt(BMM350_ENABLE_INTERRUPT, &dev);
    if (rslt != BMM350_OK)
    {
        return MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL;
    }
    rslt = bmm350_get_regs(BMM350_REG_INT_CTRL, &int_ctrl, 1, &dev);
    if (rslt != BMM350_OK)
    {
        return MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL;
    }

    // printf("Expected : 0x2E : Interrupt control : 0x%X\n", set_int_ctrl);
    // printf("Read : 0x2E : Interrupt control : 0x%X\n", int_ctrl);

    if (int_ctrl & BMM350_DRDY_DATA_REG_EN_MSK)
    {
        // printf("Data ready enabled\n");
    }

    /* Set ODR and performance */
    rslt = bmm350_set_odr_performance(BMM350_DATA_RATE_100HZ, BMM350_AVERAGING_4, &dev);
    if (rslt != BMM350_OK)
    {
        return MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL;
    }
    struct bmm350_raw_mag_data raw_data;
    /* Enable all axis */
    rslt = bmm350_enable_axes(BMM350_X_EN, BMM350_Y_EN, BMM350_Z_EN, &dev);
    if (rslt == BMM350_OK)
    {
        rslt = bmm350_set_powermode(BMM350_NORMAL_MODE, &dev);
        while (loop > 0)
        {
            int_status = 0;

            /* Get data ready interrupt status */
            rslt = bmm350_get_regs(BMM350_REG_INT_STATUS, &int_status, 1, &dev);
            if (rslt != BMM350_OK)
            {
                return MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL;
            }
            /* Check if data ready interrupt occurred */
            if (int_status & BMM350_DRDY_DATA_REG_MSK)
            {
                /* Get uncompensated mag data */
                rslt = bmm350_read_uncomp_mag_temp_data(&raw_data, &dev);
                if (rslt != BMM350_OK)
                {
                    return MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL;
                }
                rslt = bmm350_read_sensortime(&secs, &nano_secs, &dev);
                if (rslt != BMM350_OK)
                {
                    return MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL;
                }
                loop--;
            }
        }
    }
    return rslt == 0 ? MANIKIN_STATUS_OK : MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL;
}

manikin_status_t
bmm350_read_sensor (manikin_sensor_ctx_t *sensor_ctx, uint8_t *read_buf)
{
    MANIKIN_ASSERT(HASH_BMM350, (read_buf != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    manikin_status_t status = bmm350_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BMM350, (status == MANIKIN_STATUS_OK), status);

    if (sensor_ctx->needs_reinit)
    {
        bmm350_init_sensor(sensor_ctx);
    }
    struct bmm350_mag_temp_data mag_temp_data;

    uint8_t int_status = 0;

    /* Get data ready interrupt status */
    int8_t rslt = bmm350_get_regs(BMM350_REG_INT_STATUS, &int_status, 1, &dev);

    /* Check if data ready interrupt occurred */
    if (int_status & BMM350_DRDY_DATA_REG_MSK)
    {
        rslt = bmm350_get_compensated_mag_xyz_temp_data(&mag_temp_data, &dev);
        memcpy(read_buf, &mag_temp_data, sizeof(mag_temp_data));
    }

    return rslt == 0 ? MANIKIN_STATUS_OK : MANIKIN_STATUS_ERR_READ_FAIL;
}

manikin_status_t
bmm350_deinit_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = bmm350_check_params(sensor_ctx);

    return status;
}

manikin_status_t
bmm350_parse_raw_data (const uint8_t *raw_data, bmm350_sample_data_t *data)
{
    MANIKIN_ASSERT(HASH_BMM350, (raw_data != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_BMM350, (data != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    struct bmm350_mag_temp_data raw_mag_data;
    memcpy(&raw_mag_data, raw_data, sizeof(struct bmm350_mag_temp_data));
    /**
     * Expected layout:
     * [0-2]   - magneto_x (LSB first)
     * [3-5]   - magneto_y
     * [6-8]   - magneto_z
     * [9-11]  - temperature
     * [12-14] - sensor_time
     */
    data->magneto_x_ut     = raw_mag_data.x;
    data->magneto_y_ut     = raw_mag_data.y;
    data->magneto_z_ut     = raw_mag_data.z;
    data->temperature_mdeg = raw_mag_data.temperature;
    data->sensor_time_us   = 0;
    return MANIKIN_STATUS_OK;
}