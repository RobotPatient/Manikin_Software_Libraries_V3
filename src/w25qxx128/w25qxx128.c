/**
 * @file            w25qxx128.c
 * @brief           Driver implementation for the Winbond W25QXX 128 Mbit SPI NOR flash chip
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

#include "w25qxx128.h"

#include "manikin_platform.h"
#include "common/manikin_bit_manipulation.h"
#include "error_handler/error_handler.h"
#include "spi/spi.h"
#include "private/w25qxx128_regs.h"

#define W25QXX_HASH 0xAE5DF8E6u
#define W25Q_NO_TIMEOUT_REACHED(TIME_START, TIMEOUT) \
    (((MANIKIN_HAL_GET_TICK() - TIME_START) & 0xFFFFFFFF) < (TIMEOUT))

// NOTE: Defined globally as the underlying HAL methods probably relies on DMA.
// Meaning these variables should be placed in .DATA section and persist after function call.
static uint8_t w25qxx_reg_write_buf[16];
static uint8_t w25qxx_reg_read_buf[16];

static manikin_status_t
w25qxx_check_id (manikin_spi_memory_ctx_t *mem_ctx)
{
    MANIKIN_ASSERT(W25QXX_HASH, mem_ctx != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);
    w25qxx_reg_write_buf[0] = W25QXX_REG_JEDEC;
    manikin_spi_start_transaction(mem_ctx->spi_cs);
    manikin_spi_write(mem_ctx->spi, w25qxx_reg_write_buf, W25QXX_REG_JEDEC_WR_SIZE);
    manikin_spi_read(mem_ctx->spi, w25qxx_reg_read_buf, W25QXX_REG_JEDEC_SIZE);
    manikin_spi_end_transaction(mem_ctx->spi_cs);
    MANIKIN_NON_CRIT_ASSERT(W25QXX_HASH,
                            (w25qxx_reg_read_buf[W25QXX_MAN_ID_IDX] == W25QXX_RESP_MANUFACTURER_ID
                             && w25qxx_reg_read_buf[W25QXX_MEM_TYPE_IDX] == W25QXX_RESP_MEM_TYPE
                             && w25qxx_reg_read_buf[W25QXX_MEM_SIZE_IDX] == W25QXX_RESP_MEM_SIZE),
                            MANIKIN_STATUS_ERR_READ_FAIL);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
w25qxx_init (manikin_spi_memory_ctx_t *mem_ctx)
{
    manikin_status_t status = w25qxx_check_id(mem_ctx);
    MANIKIN_ASSERT(W25QXX_HASH, (status == MANIKIN_STATUS_OK), status);
    mem_ctx->fault_cnt      = 0;
    w25qxx_reg_write_buf[0] = W25QXX_REG_RESET_MSB;
    w25qxx_reg_write_buf[1] = W25QXX_REG_RESET_LSB;
    manikin_spi_start_transaction(mem_ctx->spi_cs);
    manikin_spi_write(mem_ctx->spi, w25qxx_reg_write_buf, W25QXX_REG_RESET_SIZE);
    manikin_spi_end_transaction(mem_ctx->spi_cs);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
w25qxx_status (manikin_spi_memory_ctx_t *mem_ctx)
{
    MANIKIN_ASSERT(W25QXX_HASH, mem_ctx != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);
    return w25qxx_check_id(mem_ctx);
}

manikin_status_t
w25qxx_wait_while_busy (manikin_spi_memory_ctx_t *mem_ctx)
{
    size_t time_start       = MANIKIN_HAL_GET_TICK();
    w25qxx_reg_write_buf[0] = W25QXX_REG_STATUS;

    manikin_status_t status = MANIKIN_STATUS_OK;

    do
    {
        status = manikin_spi_start_transaction(mem_ctx->spi_cs);
        const size_t bytes_written
            = manikin_spi_write(mem_ctx->spi, w25qxx_reg_write_buf, W25QXX_REG_STATUS_SIZE);
        const size_t bytes_read
            = manikin_spi_read(mem_ctx->spi, w25qxx_reg_read_buf, W25QXX_REG_STATUS_SIZE);
        status |= manikin_spi_end_transaction(mem_ctx->spi_cs);
        if (status != MANIKIN_STATUS_OK || bytes_written != W25QXX_REG_STATUS_SIZE
            || bytes_read != W25QXX_REG_STATUS_SIZE)
        {
            status = MANIKIN_STATUS_ERR_READ_FAIL;
            break;
        }
    } while (w25qxx_reg_read_buf[0] & W25QXX_REG_STATUS_BUSY
             && W25Q_NO_TIMEOUT_REACHED(time_start, MANIKIN_SOFTWARE_MAX_TIMEOUT));

    manikin_spi_end_transaction(mem_ctx->spi_cs);
    return status;
}

static manikin_status_t
w25qxx_set_address (manikin_spi_memory_ctx_t *mem_ctx, const uint8_t opcode, const uint32_t addr)
{
    w25qxx_reg_write_buf[0] = opcode;
    w25qxx_reg_write_buf[1] = GET_LAST_8_BITS_OF_24B(addr);
    w25qxx_reg_write_buf[2] = GET_UPPER_8_BITS_OF_24B(addr);
    w25qxx_reg_write_buf[3] = GET_LOWER_8_BITS_OF_24B(addr);

    manikin_status_t status = manikin_spi_start_transaction(mem_ctx->spi_cs);
    size_t           bytes_written
        = manikin_spi_write(mem_ctx->spi, w25qxx_reg_write_buf, W25QXX_REG_PAGE_PROGRAM_SIZE);

    MANIKIN_NON_CRIT_ASSERT(
        W25QXX_HASH,
        ((status == MANIKIN_STATUS_OK) && (bytes_written == W25QXX_REG_PAGE_PROGRAM_SIZE)),
        MANIKIN_STATUS_ERR_WRITE_FAIL);

    return MANIKIN_STATUS_OK;
}

manikin_memory_result_t
w25qxx_write (manikin_spi_memory_ctx_t *mem_ctx, uint8_t *data, uint32_t addr, size_t len)
{
    MANIKIN_ASSERT(W25QXX_HASH,
                   mem_ctx != NULL && data != NULL && len < W25QXX_FLASH_SIZE && len != 0,
                   MANIKIN_MEMORY_RESULT_PARERR);
    const uint8_t fault_cnt_not_exceeded = mem_ctx->fault_cnt < MANIKIN_FLASH_MAX_RETRIES;
    MANIKIN_ASSERT(W25QXX_HASH, fault_cnt_not_exceeded, MANIKIN_MEMORY_RESULT_NOTRDY);

    // WARNING: Cast in line below
    // Size_t is assumed to be 32-bit, but on 64-bit platform it will be 64-bit.
    // This is not an issue, as the range is limited to 16 MB by assert above. The cast is safe!
    uint32_t remaining  = (uint32_t)(len);
    uint8_t *src        = data;
    size_t   time_start = MANIKIN_HAL_GET_TICK();

    // NOTE: Worst case, flash runs at 10 MHz, with a very inefficient hal:
    // it would take approximately 1 tick (SysTick) to transmit 1024 bytes
    size_t timeout_time = ((len / 1024) + 1) * MANIKIN_SOFTWARE_MAX_TIMEOUT;
    while (remaining > 0 && W25Q_NO_TIMEOUT_REACHED(time_start, timeout_time))
    {
        uint32_t page_offset = addr % W25QXX_PAGE_SIZE;
        uint32_t page_space  = W25QXX_PAGE_SIZE - page_offset;
        uint32_t chunk       = (remaining < page_space) ? remaining : page_space;

        w25qxx_reg_write_buf[0] = W25QXX_REG_WREN;
        manikin_spi_start_transaction(mem_ctx->spi_cs);
        manikin_spi_write(mem_ctx->spi, w25qxx_reg_write_buf, 1);
        manikin_spi_end_transaction(mem_ctx->spi_cs);

        manikin_status_t status = w25qxx_set_address(mem_ctx, W25QXX_REG_PAGE_PROGRAM, addr);

        if (status != MANIKIN_STATUS_OK)
        {
            manikin_spi_end_transaction(mem_ctx->spi_cs);
            mem_ctx->fault_cnt++;
            return MANIKIN_MEMORY_RESULT_ERROR;
        }

        size_t bytes_written = manikin_spi_write(mem_ctx->spi, src, chunk);
        status |= manikin_spi_end_transaction(mem_ctx->spi_cs);
        if (status != MANIKIN_STATUS_OK || bytes_written != chunk)
        {
            manikin_spi_end_transaction(mem_ctx->spi_cs);
            mem_ctx->fault_cnt++;
            return MANIKIN_MEMORY_RESULT_ERROR;
        }

        if (w25qxx_wait_while_busy(mem_ctx) != MANIKIN_STATUS_OK)
        {
            mem_ctx->fault_cnt++;
            return MANIKIN_MEMORY_RESULT_ERROR;
        }
        addr += chunk;
        src += chunk;
        remaining -= chunk;
    }

    return MANIKIN_MEMORY_RESULT_OK;
}

manikin_memory_result_t
w25qxx_read (manikin_spi_memory_ctx_t *mem_ctx, uint8_t *data, uint32_t addr, size_t len)
{
    MANIKIN_ASSERT(W25QXX_HASH,
                   mem_ctx != NULL && data != NULL && len < W25QXX_FLASH_SIZE && len != 0,
                   MANIKIN_MEMORY_RESULT_PARERR);

    const uint8_t fault_cnt_not_exceeded = mem_ctx->fault_cnt < MANIKIN_FLASH_MAX_RETRIES;
    MANIKIN_ASSERT(W25QXX_HASH, fault_cnt_not_exceeded, MANIKIN_MEMORY_RESULT_NOTRDY);

    manikin_status_t status = w25qxx_check_id(mem_ctx);
    MANIKIN_ASSERT(W25QXX_HASH, status == MANIKIN_STATUS_OK, MANIKIN_MEMORY_RESULT_NOTRDY);

    size_t remaining = len;

    status |= manikin_spi_start_transaction(mem_ctx->spi_cs);
    if (status != MANIKIN_STATUS_OK)
    {
        manikin_spi_end_transaction(mem_ctx->spi_cs);
        mem_ctx->fault_cnt++;
        return MANIKIN_MEMORY_RESULT_ERROR;
    }

    status |= w25qxx_set_address(mem_ctx, W25QXX_REG_READ, addr);
    if (status != MANIKIN_STATUS_OK)
    {
        mem_ctx->fault_cnt++;
        manikin_spi_end_transaction(mem_ctx->spi_cs);
        return MANIKIN_MEMORY_RESULT_ERROR;
    }

    size_t time_start = MANIKIN_HAL_GET_TICK();
    // Worst case, flash runs at 10 MHz, with a very inefficient hal, it would take 1 cycle to
    // receive 1024 bytes
    size_t timeout_time = ((len / 1024) + 1) * MANIKIN_SOFTWARE_MAX_TIMEOUT;

    while (remaining > 0 && W25Q_NO_TIMEOUT_REACHED(time_start, timeout_time))
    {
        size_t chunk = remaining > W25QXX_PAGE_SIZE ? W25QXX_PAGE_SIZE : remaining;

        size_t bytes_read = manikin_spi_read(mem_ctx->spi, data, chunk);
        if (bytes_read != chunk)
        {
            manikin_spi_end_transaction(mem_ctx->spi_cs);
            mem_ctx->fault_cnt++;
            return MANIKIN_MEMORY_RESULT_ERROR;
        }
        data += chunk;

        // WARNING: Cast in line below
        // NOTE: Size_t is assumed to be 32-bit, but on 64-bit platform it will be 64-bit wide.
        // This is not an issue, as the range is limited to 16 MB by assert above. The cast is safe!
        addr += (uint32_t)chunk;
        remaining -= chunk;
    }
    manikin_spi_end_transaction(mem_ctx->spi_cs);
    return MANIKIN_MEMORY_RESULT_OK;
}

manikin_memory_result_t
w25qxx_erase_sector (manikin_spi_memory_ctx_t *mem_ctx, uint32_t sector_number)
{
    MANIKIN_ASSERT(W25QXX_HASH,
                   mem_ctx != NULL && sector_number < W25QXX_SECTOR_SIZE,
                   MANIKIN_MEMORY_RESULT_PARERR);
    const uint8_t fault_cnt_not_exceeded = mem_ctx->fault_cnt < MANIKIN_FLASH_MAX_RETRIES;
    MANIKIN_ASSERT(W25QXX_HASH, fault_cnt_not_exceeded, MANIKIN_MEMORY_RESULT_NOTRDY);

    const uint32_t addr     = sector_number * W25QXX_SECTOR_SIZE;
    w25qxx_reg_write_buf[0] = W25QXX_REG_WREN;
    manikin_status_t status = manikin_spi_start_transaction(mem_ctx->spi_cs);
    const size_t     written
        = manikin_spi_write(mem_ctx->spi, w25qxx_reg_write_buf, W25QXX_REG_WREN_SIZE);
    status |= manikin_spi_end_transaction(mem_ctx->spi_cs);
    if (status != MANIKIN_STATUS_OK && written != W25QXX_REG_WREN_SIZE)
    {
        manikin_spi_end_transaction(mem_ctx->spi_cs);
        mem_ctx->fault_cnt++;
        return MANIKIN_MEMORY_RESULT_ERROR;
    }

    status |= manikin_spi_start_transaction(mem_ctx->spi_cs);
    status |= w25qxx_set_address(mem_ctx, W25QXX_REG_SECTOR_ERASE, addr);
    if ((status != MANIKIN_STATUS_OK) || (w25qxx_wait_while_busy(mem_ctx) != MANIKIN_STATUS_OK))
    {
        manikin_spi_end_transaction(mem_ctx->spi_cs);
        mem_ctx->fault_cnt++;
        return MANIKIN_MEMORY_RESULT_ERROR;
    }
    status |= manikin_spi_end_transaction(mem_ctx->spi_cs);
    if (status != MANIKIN_STATUS_OK)
    {
        manikin_spi_end_transaction(mem_ctx->spi_cs);
        mem_ctx->fault_cnt++;
        return MANIKIN_MEMORY_RESULT_ERROR;
    }
    return MANIKIN_MEMORY_RESULT_OK;
}

manikin_status_t
w25qxx_deinit (manikin_spi_memory_ctx_t *mem_ctx)
{
    // TODO: Maybe implement an deinit sequence?
    (void)mem_ctx;
    return MANIKIN_STATUS_OK;
}
