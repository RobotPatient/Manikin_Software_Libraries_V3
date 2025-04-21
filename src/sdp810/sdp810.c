/**
 * @file            sdp810.c
 * @brief          Driver implementation for the Sensirion SDP810 Differential pressure sensor
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

#include "sdp810.h"
#include "i2c/i2c.h"
#include "private/sdp810_regs.h"
#include "error_handler/error_handler.h"

#include <string.h>

#define HASH_SDP810 0x6BCF2C37u

static const manikin_sensor_reg_t SDP810_init_regs[]
    = { { SDP810_REG_CONT_MASS_FLOW, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE } };

static manikin_status_t
sdp810_check_params (manikin_sensor_ctx_t *sensor_ctx)
{
    MANIKIN_ASSERT(HASH_SDP810, (sensor_ctx != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_SDP810, (sensor_ctx->i2c != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
sdp810_init_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = sdp810_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_SDP810, (status == MANIKIN_STATUS_OK), status);
    sensor_ctx->needs_reinit = 0;
    for (size_t i = 0; i < sizeof(SDP810_init_regs) / sizeof(manikin_sensor_reg_t); i++)
    {
        // WARNING: Cast in line below.
        // NOTE: Mask ensures only lower byte is written; cast enforces 8-bit width
        manikin_i2c_write_reg(sensor_ctx->i2c,
                              sensor_ctx->i2c_addr,
                              SDP810_init_regs[i].reg,
                              (uint8_t)(SDP810_init_regs[i].val & 0xFF));
    }
    return MANIKIN_STATUS_OK;
}

manikin_status_t
sdp810_read_sensor (manikin_sensor_ctx_t *sensor_ctx, uint8_t *read_buf)
{
    MANIKIN_ASSERT(
        HASH_SDP810, sensor_ctx != NULL && read_buf != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);

    if (sensor_ctx->needs_reinit)
    {
        sdp810_init_sensor(sensor_ctx);
    }

    size_t bytes_read = manikin_i2c_read_bytes(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, read_buf, SDP810_READ_BUFFER_SIZE);

    MANIKIN_ASSERT(
        HASH_SDP810, bytes_read == SDP810_READ_BUFFER_SIZE, MANIKIN_STATUS_ERR_READ_FAIL);

    return MANIKIN_STATUS_OK;
}

manikin_status_t
sdp810_deinit_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = sdp810_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_SDP810, status == MANIKIN_STATUS_OK, MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}
