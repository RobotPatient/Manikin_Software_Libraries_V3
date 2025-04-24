/**
 * @file            i2c.c
 * @brief           Implementation of platform-agnostic i2c wrapper implementation
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

#include "i2c.h"
#include "manikin_platform.h"
#include "error_handler/error_handler.h"
#include "common/manikin_bit_manipulation.h"

#define HASH_I2C 0xA0DF31CAu

/**
 * @brief Internal function to check the baud parameter
 * @param baud The baud-rate to check
 * @return - MANIKIN_STATUS_OK if all parameters are valid
 *         - MANIKIN_STATUS_ERR_INVALID_I2C_BAUD if invalid
 */
static manikin_status_t
manikin_i2c_validate_baud (const manikin_i2c_speed_t baud)
{
    manikin_status_t status;
    switch (baud)
    {
        case MANIKIN_I2C_SPEED_100KHz:
        case MANIKIN_I2C_SPEED_400KHz:
        case MANIKIN_I2C_SPEED_1MHz:
            status = MANIKIN_STATUS_OK;
            break;

        default:
            status = MANIKIN_STATUS_ERR_INVALID_I2C_BAUD;
            break;
    }
    return status;
}

manikin_status_t
manikin_i2c_init (manikin_i2c_inst_t i2c_inst, const manikin_i2c_speed_t i2c_baud)
{

    MANIKIN_ASSERT(HASH_I2C, (i2c_inst != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    manikin_status_t status = manikin_i2c_validate_baud(i2c_baud);
    MANIKIN_NON_CRIT_ASSERT(HASH_I2C, (status == MANIKIN_STATUS_OK), status);

    status = MANIKIN_I2C_HAL_INIT(i2c_inst, i2c_baud);
    MANIKIN_NON_CRIT_ASSERT(HASH_I2C, (status == MANIKIN_STATUS_OK), status);

    return MANIKIN_STATUS_OK;
}

uint8_t
manikin_i2c_check_device_address (manikin_i2c_inst_t i2c_inst, const uint8_t i2c_addr)
{
    MANIKIN_ASSERT(HASH_I2C, (i2c_inst != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_I2C_HAL_DEVICE_ACKNOWLEDGE(i2c_inst, i2c_addr << 1);
}

manikin_status_t
manikin_i2c_write_reg (manikin_i2c_inst_t i2c_inst,
                       const uint8_t      i2c_addr,
                       const uint16_t     reg,
                       const uint8_t      data)
{

    MANIKIN_ASSERT(HASH_I2C, (i2c_inst != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    uint8_t bytes[3];
    bytes[0] = GET_UPPER_8_BITS_OF_SHORT(reg);
    bytes[1] = GET_LOWER_8_BITS_OF_SHORT(reg);
    bytes[2] = data;
    if (MANIKIN_I2C_HAL_WRITE_BYTES(i2c_inst, i2c_addr << 1, bytes, sizeof(bytes)) != sizeof(bytes))
    {
        return MANIKIN_STATUS_ERR_WRITE_FAIL;
    }
    return MANIKIN_STATUS_OK;
}

manikin_status_t
manikin_i2c_write_reg16 (manikin_i2c_inst_t i2c_inst,
                         const uint8_t      i2c_addr,
                         const uint16_t     reg,
                         const uint16_t     data)
{
    MANIKIN_ASSERT(HASH_I2C, (i2c_inst != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    uint8_t bytes[4];
    bytes[0] = GET_UPPER_8_BITS_OF_SHORT(reg);
    bytes[1] = GET_LOWER_8_BITS_OF_SHORT(reg);
    bytes[2] = GET_UPPER_8_BITS_OF_SHORT(data);
    bytes[3] = GET_LOWER_8_BITS_OF_SHORT(data);

    if (MANIKIN_I2C_HAL_WRITE_BYTES(i2c_inst, i2c_addr << 1, bytes, sizeof(bytes)) != sizeof(bytes))
    {
        return MANIKIN_STATUS_ERR_WRITE_FAIL;
    }
    return MANIKIN_STATUS_OK;
}

manikin_status_t
manikin_i2c_read_reg (manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr, uint16_t reg, uint8_t *data)
{
    MANIKIN_ASSERT(HASH_I2C, (i2c_inst != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    uint8_t bytes[2];
    bytes[0] = GET_UPPER_8_BITS_OF_SHORT(reg);
    bytes[1] = GET_LOWER_8_BITS_OF_SHORT(reg);
    if (MANIKIN_I2C_HAL_WRITE_BYTES(i2c_inst, i2c_addr << 1, bytes, sizeof(bytes)) != sizeof(bytes))
    {
        return MANIKIN_STATUS_ERR_READ_FAIL;
    }
    if (MANIKIN_I2C_HAL_READ_BYTES(i2c_inst, i2c_addr << 1, data, 1) != 1)
    {
        return MANIKIN_STATUS_ERR_READ_FAIL;
    }
    return MANIKIN_STATUS_OK;
}

manikin_status_t
manikin_i2c_read_reg16 (manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr, uint16_t reg, uint8_t *data)
{
    MANIKIN_ASSERT(HASH_I2C, (i2c_inst != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    uint8_t bytes[2];
    bytes[0] = GET_UPPER_8_BITS_OF_SHORT(reg);
    bytes[1] = GET_LOWER_8_BITS_OF_SHORT(reg);
    if (MANIKIN_I2C_HAL_WRITE_BYTES(i2c_inst, i2c_addr << 1, bytes, sizeof(bytes)) != sizeof(bytes))
    {
        return MANIKIN_STATUS_ERR_READ_FAIL;
    }
    if (MANIKIN_I2C_HAL_READ_BYTES(i2c_inst, i2c_addr << 1, data, 2) != 2)
    {
        return MANIKIN_STATUS_ERR_READ_FAIL;
    }
    return MANIKIN_STATUS_OK;
}

size_t
manikin_i2c_read_bytes (manikin_i2c_inst_t i2c_inst,
                        const uint8_t      i2c_addr,
                        uint8_t           *data,
                        const size_t       len)
{
    MANIKIN_ASSERT(HASH_I2C, (i2c_inst != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_I2C_HAL_READ_BYTES(i2c_inst, i2c_addr << 1, data, len);
}

size_t
manikin_i2c_write_bytes (manikin_i2c_inst_t i2c_inst,
                         const uint8_t      i2c_addr,
                         const uint8_t     *data,
                         const size_t       len)
{
    MANIKIN_ASSERT(HASH_I2C, (i2c_inst != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_I2C_HAL_WRITE_BYTES(i2c_inst, i2c_addr << 1, data, len);
}

manikin_status_t
manikin_i2c_deinit (manikin_i2c_inst_t i2c_inst)
{
    MANIKIN_ASSERT(HASH_I2C, (i2c_inst != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_I2C_HAL_DEINIT(i2c_inst);
}
