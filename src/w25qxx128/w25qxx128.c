#include "w25qxx128.h"

#include "manikin_platform.h"
#include "error_handler/error_handler.h"
#include "spi/spi.h"
#include "private/w25qxx128_regs.h"

#define W25QXX_HASH 0xAE5DF8E6
#define W25Q_NO_TIMEOUT_REACHED(TIME_START, TIMEOUT) \
    (((MANIKIN_HAL_GET_TICK() - TIME_START) & 0xFFFFFFFF) < (TIMEOUT))

/* Defined globally as the underlying HAL methods probably relies on DMA. */
/* Meaning these variables should be placed in .DATA section*/
uint8_t w25qxx_reg_write_buf[16];
uint8_t w25qxx_reg_read_buf[16];

manikin_status_t
w25qxx_check_id (manikin_spi_memory_ctx_t *mem_ctx)
{
    MANIKIN_ASSERT(W25QXX_HASH, mem_ctx != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);
    w25qxx_reg_write_buf[0] = W25QXX_REG_JEDEC;
    manikin_spi_start_transaction(mem_ctx->spi_cs);
    manikin_spi_write(mem_ctx->spi, w25qxx_reg_write_buf, 1);
    manikin_spi_read(mem_ctx->spi, w25qxx_reg_read_buf, W25QXX_REG_JEDEC_SIZE);
    manikin_spi_end_transaction(mem_ctx->spi_cs);
    MANIKIN_NON_CRIT_ASSERT(W25QXX_HASH,
                            (w25qxx_reg_read_buf[0] == W25QXX_RESP_MANUFACTURER_ID
                             && w25qxx_reg_read_buf[1] == W25QXX_RESP_MANUFACTURER_MEM_TYPE
                             && w25qxx_reg_read_buf[2] == W25QXX_RESP_MANUFACTURER_MEM_SIZE),
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
    w25qxx_reg_write_buf[0] = W25QXX_REG_STATUS; // Read status register

    manikin_status_t status = MANIKIN_STATUS_OK;

    do
    {
        status                     = manikin_spi_start_transaction(mem_ctx->spi_cs);
        const size_t bytes_written = manikin_spi_write(mem_ctx->spi, w25qxx_reg_write_buf, 1);
        const size_t bytes_read    = manikin_spi_read(mem_ctx->spi, w25qxx_reg_read_buf, 1);
        status |= manikin_spi_end_transaction(mem_ctx->spi_cs);
        if (status != MANIKIN_STATUS_OK || bytes_written != 1 || bytes_read != 1)
        {
            status = MANIKIN_STATUS_ERR_READ_FAIL;
            break;
        }
    } while (w25qxx_reg_read_buf[0] & W25QXX_REG_STATUS_BUSY
             && W25Q_NO_TIMEOUT_REACHED(
                 time_start, MANIKIN_SOFTWARE_MAX_TIMEOUT)); // Wait while BUSY bit is set
    manikin_spi_end_transaction(mem_ctx->spi_cs);
    return status;
}

manikin_memory_result_t
w25qxx_write (manikin_spi_memory_ctx_t *mem_ctx, uint8_t *data, uint32_t addr, size_t len)
{
    MANIKIN_ASSERT(W25QXX_HASH,
                   mem_ctx != NULL && data != NULL && len != SIZE_MAX && len != 0,
                   MANIKIN_MEMORY_RESULT_PARERR);
    MANIKIN_ASSERT(
        W25QXX_HASH, mem_ctx->fault_cnt < MANIKIN_FLASH_MAX_RETRIES, MANIKIN_MEMORY_RESULT_NOTRDY);

    addr                = addr * 256;
    size_t   remaining  = len;
    uint8_t *src        = data;
    size_t   time_start = MANIKIN_HAL_GET_TICK();
    /* Worst case, flash runs at 10 MHz, with a very inefficient hal, it would take 1 cycle to
     * transmit 1024 bytes */
    size_t timeout_time = ((len / 1024) + 1) * MANIKIN_SOFTWARE_MAX_TIMEOUT;
    while (remaining > 0 && W25Q_NO_TIMEOUT_REACHED(time_start, timeout_time))
    {
        // Calculate how much we can write in this page
        uint32_t page_offset = addr % 256;
        uint32_t page_space  = 256 - page_offset;
        size_t   chunk       = (remaining < page_space) ? remaining : page_space;

        // --- Write Enable ---
        w25qxx_reg_write_buf[0] = W25QXX_REG_WREN;
        manikin_spi_start_transaction(mem_ctx->spi_cs);
        manikin_spi_write(mem_ctx->spi, w25qxx_reg_write_buf, 1);
        manikin_spi_end_transaction(mem_ctx->spi_cs);

        // --- Page Program ---
        w25qxx_reg_write_buf[0] = W25QXX_REG_PAGE_PROGRAM;
        w25qxx_reg_write_buf[1] = (addr >> 16) & 0xFF;
        w25qxx_reg_write_buf[2] = (addr >> 8) & 0xFF;
        w25qxx_reg_write_buf[3] = addr & 0xFF;

        manikin_status_t status = manikin_spi_start_transaction(mem_ctx->spi_cs);
        size_t           bytes_written
            = manikin_spi_write(mem_ctx->spi, w25qxx_reg_write_buf, W25QXX_REG_PAGE_PROGRAM_SIZE);
        if (bytes_written != W25QXX_REG_PAGE_PROGRAM_SIZE)
        {
            manikin_spi_end_transaction(mem_ctx->spi_cs);
            mem_ctx->fault_cnt++;
            return MANIKIN_MEMORY_RESULT_ERROR;
        }

        bytes_written = manikin_spi_write(mem_ctx->spi, src, chunk);
        status |= manikin_spi_end_transaction(mem_ctx->spi_cs);
        if (status != MANIKIN_STATUS_OK || bytes_written != chunk)
        {
            manikin_spi_end_transaction(mem_ctx->spi_cs);
            mem_ctx->fault_cnt++;
            return MANIKIN_MEMORY_RESULT_ERROR;
        }

        // --- Wait for write to complete ---
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
                   mem_ctx != NULL && data != NULL && len != SIZE_MAX && len != 0,
                   MANIKIN_MEMORY_RESULT_PARERR);
    MANIKIN_ASSERT(
        W25QXX_HASH, mem_ctx->fault_cnt < MANIKIN_FLASH_MAX_RETRIES, MANIKIN_MEMORY_RESULT_NOTRDY);
    manikin_status_t status = w25qxx_check_id(mem_ctx);
    MANIKIN_ASSERT(W25QXX_HASH, status == MANIKIN_STATUS_OK, status);
    addr             = addr * 256; // 256 = page size
    size_t remaining = len;

    status |= manikin_spi_start_transaction(mem_ctx->spi_cs);
    if (status != MANIKIN_STATUS_OK)
    {
        manikin_spi_end_transaction(mem_ctx->spi_cs);
        mem_ctx->fault_cnt++;
        return MANIKIN_MEMORY_RESULT_ERROR;
    }
    w25qxx_reg_write_buf[0] = W25QXX_REG_READ; // Read command
    w25qxx_reg_write_buf[1] = (addr >> 16) & 0xFF;
    w25qxx_reg_write_buf[2] = (addr >> 8) & 0xFF;
    w25qxx_reg_write_buf[3] = addr & 0xFF;

    size_t bytes_written
        = manikin_spi_write(mem_ctx->spi, w25qxx_reg_write_buf, W25QXX_REG_READ_SIZE);
    if (bytes_written != W25QXX_REG_READ_SIZE)
    {
        mem_ctx->fault_cnt++;
        manikin_spi_end_transaction(mem_ctx->spi_cs);
        return MANIKIN_MEMORY_RESULT_ERROR;
    }
    size_t time_start = MANIKIN_HAL_GET_TICK();
    /* Worst case, flash runs at 10 MHz, with a very inefficient hal, it would take 1 cycle to
     * receive 1024 bytes */
    size_t timeout_time = ((len / 1024) + 1) * MANIKIN_SOFTWARE_MAX_TIMEOUT;
    while (remaining > 0 && W25Q_NO_TIMEOUT_REACHED(time_start, timeout_time))
    {
        size_t chunk = remaining > 256 ? 256 : remaining;

        size_t bytes_read = manikin_spi_read(mem_ctx->spi, data, chunk);
        if (bytes_read != chunk)
        {
            manikin_spi_end_transaction(mem_ctx->spi_cs);
            mem_ctx->fault_cnt++;
            return MANIKIN_MEMORY_RESULT_ERROR;
        }
        data += chunk;
        addr += chunk;
        remaining -= chunk;
    }
    manikin_spi_end_transaction(mem_ctx->spi_cs);
    return MANIKIN_MEMORY_RESULT_OK;
}

manikin_memory_result_t
w25qxx_erase_sector (manikin_spi_memory_ctx_t *mem_ctx, uint32_t sector_number)
{
    MANIKIN_ASSERT(
        W25QXX_HASH, mem_ctx != NULL && sector_number != SIZE_MAX, MANIKIN_MEMORY_RESULT_PARERR);
    MANIKIN_ASSERT(
        W25QXX_HASH, mem_ctx->fault_cnt < MANIKIN_FLASH_MAX_RETRIES, MANIKIN_MEMORY_RESULT_NOTRDY);
    uint32_t addr = sector_number * W25QXX_SECTOR_SIZE; // sector size = 4KB

    // Enable write (0x06)
    w25qxx_reg_write_buf[0]        = W25QXX_REG_WREN;
    manikin_status_t status        = manikin_spi_start_transaction(mem_ctx->spi_cs);
    size_t           bytes_written = manikin_spi_write(mem_ctx->spi, w25qxx_reg_write_buf, 1);
    status |= manikin_spi_end_transaction(mem_ctx->spi_cs);
    if (status != MANIKIN_STATUS_OK && bytes_written != 1)
    {
        manikin_spi_end_transaction(mem_ctx->spi_cs);
        mem_ctx->fault_cnt++;
        return MANIKIN_MEMORY_RESULT_ERROR;
    }

    // Sector erase (0x20) + 3-byte address
    w25qxx_reg_write_buf[0] = W25QXX_REG_SECTOR_ERASE;
    w25qxx_reg_write_buf[1] = (addr >> 16) & 0xFF;
    w25qxx_reg_write_buf[2] = (addr >> 8) & 0xFF;
    w25qxx_reg_write_buf[3] = addr & 0xFF;

    status = manikin_spi_start_transaction(mem_ctx->spi_cs);
    bytes_written
        = manikin_spi_write(mem_ctx->spi, w25qxx_reg_write_buf, W25QXX_REG_SECTOR_ERASE_SIZE);
    status |= manikin_spi_end_transaction(mem_ctx->spi_cs);
    if (status != MANIKIN_STATUS_OK && bytes_written != W25QXX_REG_SECTOR_ERASE_SIZE)
    {
        manikin_spi_end_transaction(mem_ctx->spi_cs);
        mem_ctx->fault_cnt++;
        return MANIKIN_MEMORY_RESULT_ERROR;
    }

    return MANIKIN_STATUS_OK;
}

manikin_status_t
w25qxx_deinit (manikin_spi_memory_ctx_t *mem_ctx)
{
    return MANIKIN_STATUS_OK;
}
