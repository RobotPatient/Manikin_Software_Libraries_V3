/**
 * @file            vl53l4cd.c
 * @brief           Driver implementation for STM VL53L4CD ranging ToF sensor
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
 * Author:          Johan Korten
 */

#include "vl53l4cd.h"

#include "common/manikin_bit_manipulation.h"
#include "i2c/i2c.h"
#include "private/vl53l4cd_regs.h"
#include "error_handler/error_handler.h"

#define HASH_VL53L4CD 0xE2B0299Eu

/* Initialize the sensor with these register settings */
static const manikin_sensor_reg_t vl53l4cd_init_regs[] = {
    /* System configuration */
    { VL53L4CD_VHV_CONFIG_TIMEOUT_MACROP_LOOP_BOUND, 0x09, MANIKIN_SENSOR_REG_TYPE_WRITE },

    /* Sensor timing configurations */
    { VL53L4CD_RANGE_CONFIG_A, 0x09, MANIKIN_SENSOR_REG_TYPE_WRITE },
    { VL53L4CD_RANGE_CONFIG_B, 0x09, MANIKIN_SENSOR_REG_TYPE_WRITE },

    /* Set timing budget to 50ms (default) */
    { VL53L4CD_INTERMEASUREMENT_MS, 0x32, MANIKIN_SENSOR_REG_TYPE_WRITE },

    /* Configure sigma threshold (default is 15 mm) */
    { VL53L4CD_RANGE_CONFIG_SIGMA_THRESH, 0x0F, MANIKIN_SENSOR_REG_TYPE_WRITE },

    /* Clear any pending interrupts */
    { VL53L4CD_SYSTEM_INTERRUPT_CLEAR, VL53L4CD_CLEAR_INTERRUPT, MANIKIN_SENSOR_REG_TYPE_WRITE },
};

/**
 * @brief Internal function to check the parameters entered into function
 * @param sensor_ctx The sensor settings ptr which should contain i2c bus details
 * @return - MANIKIN_STATUS_OK if all parameters are valid
 *         - MANIKIN_STATUS_ERR_NULL_PARAM if invalid
 */
static manikin_status_t
vl53l4cd_check_params (manikin_sensor_ctx_t *sensor_ctx)
{
    MANIKIN_ASSERT(HASH_VL53L4CD, (sensor_ctx != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_VL53L4CD, (sensor_ctx->i2c != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
vl53l4cd_init_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = vl53l4cd_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), status);

    sensor_ctx->needs_reinit = 0;
    uint8_t  data[2]         = { 0 };
    uint16_t model_id        = 0;

    /* Check sensor fresh-out-of-reset bit, similar to VL6180X approach */
    status = manikin_i2c_read_reg(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL53L4CD_SYSTEM_FRESH_OUT_OF_RESET, &data[0]);
    MANIKIN_ASSERT(HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), status);

    /* Check if sensor is responding by reading the model ID */
    status = manikin_i2c_read_reg16(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL53L4CD_IDENTIFICATION_MODEL_ID, data);
    MANIKIN_ASSERT(HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), status);

    /* Check model ID is correct (0xEACC) */
    model_id = (uint16_t)((data[0] << 8) | data[1]);
    MANIKIN_NON_CRIT_ASSERT(
        HASH_VL53L4CD, (model_id == VL53L4CD_MODEL_ID_VALUE), MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL);

    /* Write configuration registers */
    for (size_t i = 0; i < sizeof(vl53l4cd_init_regs) / sizeof(manikin_sensor_reg_t); i++)
    {
        status = manikin_i2c_write_reg(sensor_ctx->i2c,
                                       sensor_ctx->i2c_addr,
                                       vl53l4cd_init_regs[i].reg,
                                       GET_LOWER_8_BITS_OF_SHORT(vl53l4cd_init_regs[i].val));
        MANIKIN_ASSERT(
            HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL);
    }

    /* Start ranging in continuous mode */
    status = manikin_i2c_write_reg(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL53L4CD_SYSTEM_START, VL53L4CD_RANGING_START);
    MANIKIN_ASSERT(
        HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL);

    return MANIKIN_STATUS_OK;
}

manikin_status_t
vl53l4cd_read_sensor (manikin_sensor_ctx_t *sensor_ctx, uint8_t *read_buf)
{
    manikin_status_t status = vl53l4cd_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), status);

    /* Reinitialize if needed */
    if (sensor_ctx->needs_reinit == 1)
    {
        status = vl53l4cd_init_sensor(sensor_ctx);
        MANIKIN_ASSERT(HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), status);
    }

    MANIKIN_ASSERT(HASH_VL53L4CD, (read_buf != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);

    /* Check for data ready */
    uint8_t data_ready = 0;
    status             = manikin_i2c_read_reg(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL53L4CD_SYSTEM_INTERRUPT_CLEAR, &data_ready);
    MANIKIN_ASSERT(HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), status);

    /* Check if data is ready (bit 0) */
    if (!(data_ready & 0x01))
    {
        return MANIKIN_STATUS_ERR_READ_FAIL;
    }

    /* Read distance measurement (16-bit value) */
    uint8_t distance_data[2] = { 0 };
    status                   = manikin_i2c_read_reg16(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL53L4CD_RESULT_FINAL_RANGE_MM, distance_data);
    MANIKIN_ASSERT(HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), status);

    /* Construct 16-bit distance value */
    uint16_t distance = (uint16_t)((distance_data[0] << 8) | distance_data[1]);

    /* To match VL6180X behavior, clip the distance to single byte (0-255mm) */
    if (distance > 255)
    {
        read_buf[0] = 255;
    }
    else
    {
        read_buf[0] = (uint8_t)distance;
    }

    /* Clear the interrupt for next measurement */
    status = manikin_i2c_write_reg(sensor_ctx->i2c,
                                   sensor_ctx->i2c_addr,
                                   VL53L4CD_SYSTEM_INTERRUPT_CLEAR,
                                   VL53L4CD_CLEAR_INTERRUPT);
    MANIKIN_ASSERT(HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), status);

    return MANIKIN_STATUS_OK;
}

manikin_status_t
vl53l4cd_deinit_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = vl53l4cd_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), status);

    /* Stop ranging */
    status = manikin_i2c_write_reg(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL53L4CD_SYSTEM_START, VL53L4CD_RANGING_STOP);
    MANIKIN_ASSERT(
        HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL);

    return MANIKIN_STATUS_OK;
}

manikin_status_t
vl54l4cd_parse_raw_data (const uint8_t *raw_data, vl53l4cd_sample_data_t *data)
{
    MANIKIN_ASSERT(HASH_VL53L4CD, (raw_data != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_VL53L4CD, (data != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    data->distance_mm = raw_data[0];
    return MANIKIN_STATUS_OK;
}