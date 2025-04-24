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

#include <bmm350/bmm350.h>
#include "i2c/i2c.h"
#include "private/bmm350_regs.h"
#include "error_handler/error_handler.h"

#define HASH_BMM350 0x10C9F1D2u

static const manikin_sensor_reg_t init_regs[] = {
    /* Set strong drive on all pins  */
    { BMM350_REG_PAD_CTRL, BMM350_PAD_DRIVE_STRONGEST, MANIKIN_SENSOR_REG_TYPE_WRITE },
    /* Clear the sampling settings register first  */
    { BMM350_REG_PMU_CMD_AGGR_SET, BMM350_AGGR_CLR, MANIKIN_SENSOR_REG_TYPE_WRITE },
    /* Set the sampling rate to 200Hz with 4 AVG samples */
    { BMM350_REG_PMU_CMD_AGGR_SET,
      (BMM350_AVG_SHIFT(BMM350_AVG_4) | BMM350_ODR_200HZ),
      MANIKIN_SENSOR_REG_TYPE_WRITE },
    /* Update ODR and AVG parameters */
    { BMM350_REG_PMU_CMD, BMM350_PMU_CMD_UPD_OAE, MANIKIN_SENSOR_REG_TYPE_WRITE },
    /* Enable x, y, z axis */
    { BMM350_REG_PMU_CMD_AXIS_EN,
      (BMM350_AXIS_EN_X | BMM350_AXIS_EN_Y | BMM350_AXIS_EN_Z),
      MANIKIN_SENSOR_REG_TYPE_WRITE },
    /* Enable normal mode */
    { BMM350_REG_PMU_CMD, (BMM350_PMU_CMD_NM), MANIKIN_SENSOR_REG_TYPE_WRITE },
};

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

manikin_status_t
bmm350_init_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = bmm350_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BMM350, (status == MANIKIN_STATUS_OK), status);

    // NOTE: needs_reinit is internal variable, which might be uninitialized when entered as param.
    sensor_ctx->needs_reinit = 0;

    for (size_t i = 0; i < sizeof(init_regs) / sizeof(manikin_sensor_reg_t); i++)
    {
        manikin_i2c_write_reg(sensor_ctx->i2c,
                              sensor_ctx->i2c_addr,
                              init_regs[i].reg,
                              GET_LOWER_8_BITS_OF_SHORT(init_regs[i].val));
    }
    return MANIKIN_STATUS_OK;
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

    status = manikin_i2c_write_reg(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, BMM350_REG_MAG_X_LSB, BMM350_REG_SEL);
    MANIKIN_ASSERT(HASH_BMM350, (status == MANIKIN_STATUS_OK), status);

    size_t bytes_read = manikin_i2c_read_bytes(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, read_buf, BMM350_READ_BUFFER_LENGTH);
    MANIKIN_ASSERT(
        HASH_BMM350, (bytes_read == BMM350_READ_BUFFER_LENGTH), MANIKIN_STATUS_ERR_READ_FAIL);

    return MANIKIN_STATUS_OK;
}

manikin_status_t
bmm350_deinit_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = bmm350_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BMM350, (status == MANIKIN_STATUS_OK), status);
    status = manikin_i2c_write_reg(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, BMM350_REG_PMU_CMD, BMM350_PMU_CMD_SUS);
    MANIKIN_ASSERT(
        HASH_BMM350, (status == MANIKIN_STATUS_OK), MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL);
    return MANIKIN_STATUS_OK;
}