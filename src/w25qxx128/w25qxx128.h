/**
 * @file            w25qxx128.h
 * @brief           Driver module for the Winbond W25QXX 128 Mbit SPI NOR flash chip
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

#ifndef W25QXX128_H
#define W25QXX128_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "common/manikin_types.h"

    /**
     * @brief Check for presence of W25QXX SPI flash and reset it if detected.
     * @param mem_ctx Ptr to a configuration struct containing:
     *                - Direct ptr to SPI peripheral
     *                - CS pin
     *                - SPI baud
     *                - Chip size in bytes
     * @return  - MANIKIN_STATUS_OK if chip is recognized and SPI is configured right,
     *          - MANIKIN_STATUS_ERR_READ_FAIL if chip is not recognized after read.
     */
    manikin_status_t w25qxx_init(manikin_spi_memory_ctx_t *mem_ctx);

    /**
     * @brief Check for presence of W25QXX SPI flash.
     * @param mem_ctx Ptr to a configuration struct containing:
     *                - Direct ptr to SPI peripheral
     *                - CS pin
     *                - SPI baud
     *                - Chip size in bytes
     * @return - MANIKIN_STATUS_OK if chip is recognized and SPI is configured right,
     *         - MANIKIN_STATUS_ERR_READ_FAIL if chip is not recognized after read.
     */
    manikin_status_t w25qxx_status(manikin_spi_memory_ctx_t *mem_ctx);

    /**
     * @brief Write arbitrary number of bytes to specific address on memory chip
     * @param mem_ctx Ptr to a configuration struct containing:
     *                - Direct ptr to SPI peripheral
     *                - CS pin
     *                - SPI baud
     *                - Chip size in bytes
     * @param data Ptr to the data buffer which contains data to be written to flash chip.
     * @param addr The memory address to write the data to on the flash chip.
     * @param len The number of bytes to write from the buffer.
     * @return - MANIKIN_MEMORY_RESULT_OK: If no error occurred during write,
     *         - MANIKIN_MEMORY_RESULT_WRPRT: If flash is write protected
     *         - MANIKIN_MEMORY_RESULT_NOTRDY: If flash is unresponsive, not able to read JEDEC id
     *         - MANIKIN_MEMORY_RESULT_PARERR: If any of the parameters are NULL or 0 length
     */
    manikin_memory_result_t w25qxx_write(manikin_spi_memory_ctx_t *mem_ctx,
                                         uint8_t                  *data,
                                         uint32_t                  addr,
                                         size_t                    len);

    /**
     * @brief Read arbitrary number of bytes from specific address on memory chip
     * @param mem_ctx Ptr to a configuration struct containing:
     *                - Direct ptr to SPI peripheral
     *                - CS pin
     *                - SPI baud
     *                - Chip size in bytes
     * @param data Ptr to the data buffer which contains data to be read from flash chip.
     * @param addr The memory address to store the bytes read from the flash chip.
     * @param len The number of bytes to read to the buffer.
     * @return - MANIKIN_MEMORY_RESULT_OK: If no error occurred during read,
     *         - MANIKIN_MEMORY_RESULT_NOTRDY: If flash is unresponsive, not able to read JEDEC id
     *         - MANIKIN_MEMORY_RESULT_PARERR: If any of the parameters are NULL or 0 length
     */
    manikin_memory_result_t w25qxx_read(manikin_spi_memory_ctx_t *mem_ctx,
                                        uint8_t                  *data,
                                        uint32_t                  addr,
                                        size_t                    len);

    /**
     * @brief Erase sector (4Kb) at specific sector address on memory chip
     * @param mem_ctx Ptr to a configuration struct containing:
     *                - Direct ptr to SPI peripheral
     *                - CS pin
     *                - SPI baud
     *                - Chip size in bytes
     * @param sector_number The specific sector to erase.
     * @return - MANIKIN_MEMORY_RESULT_OK: If no error occurred during read,
     *         - MANIKIN_MEMORY_RESULT_WRPRT: If flash is write protected
     *         - MANIKIN_MEMORY_RESULT_NOTRDY: If flash is unresponsive, not able to read JEDEC id
     *         - MANIKIN_MEMORY_RESULT_PARERR: If any of the parameters are NULL or 0 length
     */
    manikin_memory_result_t w25qxx_erase_sector(manikin_spi_memory_ctx_t *mem_ctx,
                                                uint32_t                  sector_number);

    /**
     * @brief Check for presence of W25QXX SPI flash and reset it if detected.
     * @param mem_ctx Ptr to a configuration struct containing:
     *                - Direct ptr to SPI peripheral
     *                - CS pin
     *                - SPI baud
     *                - Chip size in bytes
     * @return  - MANIKIN_STATUS_OK if chip is recognized and SPI is configured right,
     *          - MANIKIN_STATUS_ERR_READ_FAIL if chip is not recognized after read.
     */
    manikin_status_t w25qxx_deinit(manikin_spi_memory_ctx_t *mem_ctx);

#ifdef __cplusplus
}
#endif
#endif // W25QXX128_H
