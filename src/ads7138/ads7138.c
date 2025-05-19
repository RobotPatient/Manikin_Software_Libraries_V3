/**
 * @file            ads7138.c
 * @brief           Driver implementation for a TI ADS7138 8-channel 16-bit ADC
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

#include <ads7138/ads7138.h>
#include "i2c/i2c.h"
#include "private/ads7138_regs.h"
#include "error_handler/error_handler.h"

#include <common/manikin_bit_manipulation.h>

#define HASH_ADS7138 0x157F34F4u

static const manikin_sensor_reg_t ads7138_init_regs[] = {
    // All channels are configured as analog input
    { ADS7138_REG(ADS7138_REG_PIN_CFG, ADS7138_OP_SET_BIT),
      ADS7138_REG_ALL_PINS_ANALOG_INP_BIT,
      MANIKIN_SENSOR_REG_TYPE_WRITE },
    // Set cal bit
    { ADS7138_REG(ADS7138_REG_GEN_CFG, ADS7138_OP_SET_BIT),
      ADS7138_REG_CAL_BIT,
      MANIKIN_SENSOR_REG_TYPE_WRITE },
    // Set all adc channels as inputs. enabled in scanning sequence.
    { ADS7138_REG(ADS7138_REG_AUTO_SEQ_CH_SEL, ADS7138_OP_SET_BIT),
      ADS7138_REG_AUTO_SEQ_ALL_CH_SEL,
      MANIKIN_SENSOR_REG_TYPE_WRITE },
    // Set Auto sequence mode on = 1. And 4th for sequence start.
    { ADS7138_REG(ADS7138_REG_SEQUENCE_CFG, ADS7138_OP_SET_BIT),
      ADS7138_REG_ENABLE_AUTO_SEQ,
      MANIKIN_SENSOR_REG_TYPE_WRITE }
};

/**
 * @brief Internal function to check the parameters entered into function
 * @param sensor_ctx The sensor settings ptr which should contain i2c bus details
 * @return - MANIKIN_STATUS_OK if all parameters are valid
 *         - MANIKIN_STATUS_ERR_NULL_PARAM if invalid
 */
static manikin_status_t
ads7138_check_params (const manikin_sensor_ctx_t *sensor_ctx)
{
    MANIKIN_ASSERT(HASH_ADS7138, (sensor_ctx != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_ADS7138, (sensor_ctx->i2c != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
ads7138_init_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = ads7138_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_ADS7138, (status == MANIKIN_STATUS_OK), status);

    // NOTE: needs_reinit is internal variable, which might be uninitialized when entered as param.
    sensor_ctx->needs_reinit = 0;

    uint8_t data;
    status = manikin_i2c_read_reg(sensor_ctx->i2c,
                                  sensor_ctx->i2c_addr,
                                  ADS7138_REG(ADS7138_REG_SYSTEM_STATUS, ADS7138_OP_SINGLE_READ),
                                  &data);
    MANIKIN_ASSERT(HASH_ADS7138, (status == MANIKIN_STATUS_OK), status);

    // NOTE: If sensor is properly connected, the status RSVD bit should be 1
    MANIKIN_NON_CRIT_ASSERT(HASH_ADS7138,
                            BIT_IS_SET(data, ADS7138_REG_SYSTEM_STATUS_RSVD_BIT),
                            MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL);

    for (size_t i = 0; i < sizeof(ads7138_init_regs) / sizeof(manikin_sensor_reg_t); i++)
    {
        manikin_i2c_write_reg(sensor_ctx->i2c,
                              sensor_ctx->i2c_addr,
                              ads7138_init_regs[i].reg,
                              GET_LOWER_8_BITS_OF_SHORT(ads7138_init_regs[i].val));
    }
    return MANIKIN_STATUS_OK;
}

manikin_status_t
ads7138_read_sensor (manikin_sensor_ctx_t *sensor_ctx, uint8_t *read_buf)
{
    MANIKIN_ASSERT(HASH_ADS7138, (read_buf != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    manikin_status_t status = ads7138_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_ADS7138, (status == MANIKIN_STATUS_OK), status);

    if (sensor_ctx->needs_reinit)
    {
        ads7138_init_sensor(sensor_ctx);
    }

    status = manikin_i2c_write_reg(sensor_ctx->i2c,
                                   sensor_ctx->i2c_addr,
                                   ADS7138_REG(ADS7138_REG_SEQUENCE_CFG, ADS7138_OP_SET_BIT),
                                   ADS7138_REG_SEQ_START_BIT);
    MANIKIN_ASSERT(HASH_ADS7138, (status == MANIKIN_STATUS_OK), status);

    uint8_t raw_buf[ADS7138_READ_BUF_SIZE];
    size_t  bytes_read = manikin_i2c_read_bytes(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, raw_buf, ADS7138_READ_BUF_SIZE);
    status = manikin_i2c_write_reg(sensor_ctx->i2c,
                                   sensor_ctx->i2c_addr,
                                   ADS7138_REG(ADS7138_REG_SEQUENCE_CFG, ADS7138_OP_CLEAR_BIT),
                                   ADS7138_REG_SEQ_START_BIT);
    MANIKIN_ASSERT(HASH_ADS7138, bytes_read == ADS7138_READ_BUF_SIZE, MANIKIN_STATUS_ERR_READ_FAIL);

    // Convert raw buffer into 12-bit values, store as 2-byte values into read_buf
    for (uint8_t i = 0; i < ADS7138_NUMBER_OF_CHANNELS; i++)
    {
        const uint16_t val  = TRUNCATE_TO_12B_VAL(raw_buf[2 * i], raw_buf[2 * i + 1]);
        read_buf[2 * i]     = GET_UPPER_8_BITS_OF_SHORT(val);
        read_buf[2 * i + 1] = GET_LOWER_8_BITS_OF_SHORT(val);
    }
    return status;
}

manikin_status_t
ads7138_deinit_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = ads7138_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_ADS7138, (status == MANIKIN_STATUS_OK), status);
    status = manikin_i2c_write_reg(sensor_ctx->i2c,
                                   sensor_ctx->i2c_addr,
                                   ADS7138_REG(ADS7138_REG_GEN_CFG, ADS7138_OP_SET_BIT),
                                   ADS7138_REG_SW_RESET_BIT);
    MANIKIN_ASSERT(
        HASH_ADS7138, (status == MANIKIN_STATUS_OK), MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
ads7138_parse_raw_data (const uint8_t *raw_data, ads7138_sample_data_t *data)
{
    MANIKIN_ASSERT(HASH_ADS7138, (raw_data != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_ADS7138, (data != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    data->ch1_mv = CONSTRUCT_SHORT_FROM_BYTES(raw_data[0], raw_data[1]);
    data->ch2_mv = CONSTRUCT_SHORT_FROM_BYTES(raw_data[2], raw_data[3]);
    data->ch3_mv = CONSTRUCT_SHORT_FROM_BYTES(raw_data[4], raw_data[5]);
    data->ch4_mv = CONSTRUCT_SHORT_FROM_BYTES(raw_data[6], raw_data[7]);
    data->ch5_mv = CONSTRUCT_SHORT_FROM_BYTES(raw_data[8], raw_data[9]);
    data->ch6_mv = CONSTRUCT_SHORT_FROM_BYTES(raw_data[10], raw_data[11]);
    data->ch7_mv = CONSTRUCT_SHORT_FROM_BYTES(raw_data[12], raw_data[13]);
    data->ch8_mv = CONSTRUCT_SHORT_FROM_BYTES(raw_data[14], raw_data[15]);
    return MANIKIN_STATUS_OK;
}