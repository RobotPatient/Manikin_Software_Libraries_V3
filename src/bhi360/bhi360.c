/**
 * @file            bhi360.c
 * @brief           Driver implementation for a BHI360 programmable IMU
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

#include <bhi360/bhi360.h>
#include "i2c/i2c.h"
#include "private/bhi360_regs.h"
#include "error_handler/error_handler.h"

#define HASH_BHI360 0xC3A556E3u

static const manikin_sensor_reg_t bhi360_init_regs[]
    = { { 0x00, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE } };

/**
 * @brief Internal function to check the parameters entered into function
 * @param sensor_ctx The sensor settings ptr which should contain i2c bus details
 * @return - MANIKIN_STATUS_OK if all parameters are valid
 *         - MANIKIN_STATUS_ERR_NULL_PARAM if invalid
 */
static manikin_status_t
bhi360_check_params (const manikin_sensor_ctx_t *sensor_ctx)
{
    MANIKIN_ASSERT(HASH_BHI360, (sensor_ctx != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_BHI360, (sensor_ctx->i2c != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
bhi360_init_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = bhi360_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BHI360, (status == MANIKIN_STATUS_OK), status);

    // NOTE: needs_reinit is internal variable, which might be uninitialized when entered as param.
    sensor_ctx->needs_reinit = 0;

    for (size_t i = 0; i < sizeof(bhi360_init_regs) / sizeof(manikin_sensor_reg_t); i++)
    {
        manikin_i2c_write_reg16(sensor_ctx->i2c,
                                sensor_ctx->i2c_addr,
                                bhi360_init_regs[i].reg,
                                bhi360_init_regs[i].val);
    }
    return MANIKIN_STATUS_OK;
}

manikin_status_t
bhi360_read_sensor (manikin_sensor_ctx_t *sensor_ctx, uint8_t *read_buf)
{
    (void)read_buf;
    manikin_status_t status = bhi360_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BHI360, (status == MANIKIN_STATUS_OK), status);

    if (sensor_ctx->needs_reinit)
    {
        bhi360_init_sensor(sensor_ctx);
    }
    return MANIKIN_STATUS_OK;
}

manikin_status_t
bhi360_deinit_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = bhi360_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BHI360, (status == MANIKIN_STATUS_OK), status);
    return MANIKIN_STATUS_OK;
}