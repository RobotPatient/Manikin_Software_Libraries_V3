/**
 * @file            vl6180x.c
 * @brief           Driver implementation for STM VL6180x ranging ToF sensor
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

#include "vl6180x.h"

#include "common/manikin_bit_manipulation.h"
#include "i2c/i2c.h"
#include "private/vl6180x_regs.h"
#include "error_handler/error_handler.h"

#define HASH_VL6180X 0xE1B0199Eu

// NOTE: Some of these are undocumented private registers
static const manikin_sensor_reg_t vl6180x_init_regs[]
    = { { 0x0207, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x0208, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x0096, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x0097, 0xfd, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00e3, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00e4, 0x03, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00e5, 0x02, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00e6, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00e7, 0x03, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00f5, 0x02, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00d9, 0x05, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00db, 0xce, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00dc, 0x03, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00dd, 0xf8, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x009f, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00a3, 0x3c, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00b7, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00bb, 0x3c, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00b2, 0x09, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00ca, 0x09, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x0198, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x01b0, 0x17, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x01ad, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00ff, 0x05, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x0100, 0x05, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x0199, 0x05, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x01a6, 0x1b, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x01ac, 0x3e, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x01a7, 0x1f, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x0030, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_READOUT_AVERAGING_SAMPLE_PERIOD, 0x30, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSALS_ANALOGUE_GAIN, 0x46, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSRANGE_VHV_REPEAT_RATE, 0xff, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSALS_INTEGRATION_PERIOD, 0x63, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSRANGE_VHV_RECALIBRATE, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSRANGE_INTERMEASUREMENT_PERIOD, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSALS_INTERMEASUREMENT_PERIOD, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSRANGE_MAX_CONVERGENCE_TIME, 0x32, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSRANGE_RANGE_CHECK_ENABLES, (0x10 | 0x01), MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE, 0x7b, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSALS_INTEGRATION_PERIOD, 0x64, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_READOUT_AVERAGING_SAMPLE_PERIOD, 0x30, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSALS_ANALOGUE_GAIN, 0x40, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_FIRMWARE_RESULT_SCALER, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSRANGE_START, 0x03, MANIKIN_SENSOR_REG_TYPE_WRITE } };

/**
 * @brief Internal function to check the parameters entered into function
 * @param sensor_ctx The sensor settings ptr which should contain i2c bus details
 * @return - MANIKIN_STATUS_OK if all parameters are valid
 *         - MANIKIN_STATUS_ERR_NULL_PARAM if invalid
 */
static manikin_status_t
vl6180x_check_params (manikin_sensor_ctx_t *sensor_ctx)
{
    MANIKIN_ASSERT(HASH_VL6180X, (sensor_ctx != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_VL6180X, (sensor_ctx->i2c != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
vl6180x_init_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = vl6180x_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_VL6180X, (status == MANIKIN_STATUS_OK), status);
    sensor_ctx->needs_reinit = 0;
    uint8_t data             = 0;
    status                   = manikin_i2c_read_reg(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL6180X_REG_SYSTEM_FRESH_OUT_OF_RESET, &data);
    MANIKIN_ASSERT(HASH_VL6180X, (status == MANIKIN_STATUS_OK), status);
    MANIKIN_NON_CRIT_ASSERT(HASH_VL6180X, (data == 1), MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL);

    for (size_t i = 0; i < sizeof(vl6180x_init_regs) / sizeof(manikin_sensor_reg_t); i++)
    {
        // WARNING: Cast in line below.
        // NOTE: Mask ensures only lower byte is written; cast enforces 8-bit width
        manikin_i2c_write_reg(sensor_ctx->i2c,
                              sensor_ctx->i2c_addr,
                              vl6180x_init_regs[i].reg,
                              GET_LOWER_8_BITS_OF_SHORT(vl6180x_init_regs[i].val));
    }
    return MANIKIN_STATUS_OK;
}

manikin_status_t
vl6180x_read_sensor (manikin_sensor_ctx_t *sensor_ctx, uint8_t *read_buf)
{
    manikin_status_t status = vl6180x_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_VL6180X, (status == MANIKIN_STATUS_OK), status);
    if (sensor_ctx->needs_reinit == 1)
    {
        vl6180x_init_sensor(sensor_ctx);
    }
    MANIKIN_ASSERT(HASH_VL6180X, (read_buf != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    status = manikin_i2c_read_reg(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL6180X_REG_RESULT_RANGE_VAL, read_buf);
    MANIKIN_ASSERT(HASH_VL6180X, (status == MANIKIN_STATUS_OK), status);
    MANIKIN_ASSERT(HASH_VL6180X, (read_buf[0] != 0), MANIKIN_STATUS_ERR_READ_FAIL);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
vl6180x_deinit_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = vl6180x_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_VL6180X, (status == MANIKIN_STATUS_OK), status);
    status = manikin_i2c_write_reg(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL6180X_REG_SYSRANGE_START, 0x00);
    MANIKIN_ASSERT(
        HASH_VL6180X, (status == MANIKIN_STATUS_OK), MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
vl6180x_parse_raw_data (const uint8_t *raw_data, vl6180x_sample_data_t *data)
{
    MANIKIN_ASSERT(HASH_VL6180X, (raw_data != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_VL6180X, (data != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    data->distance_mm = raw_data[0];
    return MANIKIN_STATUS_OK;
}