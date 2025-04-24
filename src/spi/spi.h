/**
 * @file            spi.h
 * @brief           A platform-agnostic spi wrapper module
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

#ifndef SPI_H
#define SPI_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "common/manikin_types.h"

    /**
     * @brief Initializes the SPI peripheral with default settings for the Manikin System
     * @param spi_inst The direct (ptr) handle to the spi peripheral used for the device
     * @return MANIKIN_STATUS_OK on success, MANIKIN_STATUS_ERROR_PERIPHERAL_FAIL on error
     */
    manikin_status_t manikin_spi_init(manikin_spi_inst_t spi_inst, uint32_t speed);

    /**
     * @brief End the transaction by pulling the cs_pin low
     * @param cs_pin The CS pin of the device
     * @return MANIKIN_STATUS_OK on success, MANIKIN_STATUS_ERROR_PERIPHERAL_FAIL on error
     */
    manikin_status_t manikin_spi_start_transaction(manikin_spi_cs_t cs_pin);

    /**
     * @brief Write multiple bytes to a spi-device
     * @param spi_inst The direct (ptr) handle to the spi peripheral used for the device
     * @param data     A pointer to the buffer to write the data from
     * @param len      The number of bytes to write to spi-device
     * @return The number of bytes written to spi-device
     */
    size_t manikin_spi_write(manikin_spi_inst_t spi_inst, uint8_t *data, size_t len);

    /**
     * @brief Reads multiple bytes from a spi-device
     * @param spi_inst The direct (ptr) handle to the spi peripheral used for the device
     * @param data     A pointer to the buffer to save the data to
     * @param len      The number of bytes to read from spi-device
     * @return The number of bytes written to spi-device
     */
    size_t manikin_spi_read(manikin_spi_inst_t spi_inst, uint8_t *data, size_t len);

    /**
     * @brief End the transaction by pulling the cs_pin high
     * @param cs_pin The CS pin of the device
     * @return MANIKIN_STATUS_OK on success, MANIKIN_STATUS_ERROR_PERIPHERAL_FAIL on error
     */
    manikin_status_t manikin_spi_end_transaction(manikin_spi_cs_t cs_pin);

    /**
     * @brief Deinitializes the SPI peripheral of the Manikin System
     * @param spi_inst The direct (ptr) handle to the spi peripheral used for the device
     * @return MANIKIN_STATUS_OK on success, MANIKIN_STATUS_ERROR_PERIPHERAL_FAIL on error
     */
    manikin_status_t manikin_spi_deinit(manikin_spi_inst_t spi_inst);
#ifdef __cplusplus
}
#endif
#endif // SPI_H
