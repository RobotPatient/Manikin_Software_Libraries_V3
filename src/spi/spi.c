/**
 * @file            spi.c
 * @brief           Implementation of platform-agnostic spi wrapper implementation
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

#include "spi.h"
#include "manikin_platform.h"
#include "error_handler/error_handler.h"

#define SPI_HASH 0x71C911D7u

manikin_status_t
manikin_spi_init (manikin_spi_inst_t spi_inst, uint32_t speed)
{
    MANIKIN_ASSERT(SPI_HASH, spi_inst != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(SPI_HASH,
                   speed >= MANIKIN_SOFTWARE_MIN_SPI_SPEED
                       && speed <= MANIKIN_SOFTWARE_MAX_SPI_SPEED,
                   MANIKIN_STATUS_ERR_INVALID_SPI_BAUD);
    const manikin_status_t status = MANIKIN_SPI_HAL_INIT(spi_inst, speed);
    return status;
}

manikin_status_t
manikin_spi_start_transaction (manikin_spi_cs_t cs_pin)
{
    const manikin_status_t status = MANIKIN_SPI_HAL_SET_CS(cs_pin, 0);
    return status;
}

size_t
manikin_spi_write (manikin_spi_inst_t spi_inst, uint8_t *data, size_t len)
{
    MANIKIN_ASSERT(SPI_HASH, spi_inst != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(SPI_HASH, data != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(SPI_HASH, len > 0 && len < SIZE_MAX, MANIKIN_STATUS_ERR_INVALID_BUFFER_SIZE);
    const size_t bytes_written = MANIKIN_SPI_HAL_WRITE_BYTES(spi_inst, data, len);
    return bytes_written;
}

size_t
manikin_spi_read (manikin_spi_inst_t spi_inst, uint8_t *data, size_t len)
{
    MANIKIN_ASSERT(SPI_HASH, spi_inst != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(SPI_HASH, data != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(SPI_HASH, len > 0 && len < SIZE_MAX, MANIKIN_STATUS_ERR_INVALID_BUFFER_SIZE);
    const size_t bytes_read = MANIKIN_SPI_HAL_READ_BYTES(spi_inst, data, len);
    return bytes_read;
}

manikin_status_t
manikin_spi_end_transaction (manikin_spi_cs_t cs_pin)
{
    const manikin_status_t status = MANIKIN_SPI_HAL_SET_CS(cs_pin, 1);
    return status;
}

manikin_status_t
manikin_spi_deinit (manikin_spi_inst_t spi_inst)
{
    MANIKIN_ASSERT(SPI_HASH, spi_inst != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);
    const manikin_status_t status = MANIKIN_SPI_HAL_DEINIT(spi_inst);
    return status;
}
