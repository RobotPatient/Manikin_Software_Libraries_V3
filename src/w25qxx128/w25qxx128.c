#include "w25qxx128.h"

#include "manikin_platform.h"
#include "error_handler/error_handler.h"
#include "spi/spi.h"
#include "private/w25qxx128_regs.h"

#define W25QXX_HASH 0xAE5DF8E6

/* Defined globally as the underlying HAL methods probably rely on DMA. */
/* Meaning these have to be placed in .DATA section*/
uint8_t w25qxx_reg_write_buf[2];
uint8_t w25qxx_reg_read_buf[128];

manikin_status_t
w25qxx_check_id (manikin_spi_memory_ctx_t *mem_ctx)
{
    w25qxx_reg_write_buf[0] = W25QXX_REG_JEDEC;
    manikin_spi_start_transaction(mem_ctx->spi_cs);
    manikin_spi_write(mem_ctx->spi, w25qxx_reg_write_buf, 1);
    manikin_spi_read(mem_ctx->spi, w25qxx_reg_read_buf, W25QXX_REG_JEDEC_SIZE);
    manikin_spi_end_transaction(mem_ctx->spi_cs);
    MANIKIN_NON_CRIT_ASSERT(W25QXX_HASH,
                            (w25qxx_reg_read_buf[0] == W25QXX_RESP_MANUFACTURER_ID
                             && w25qxx_reg_read_buf[1] == W25QXX_RESP_MANUFACTURER_MEM_TYPE
                             && w25qxx_reg_read_buf[2] == W25QXX_RESP_MANUFACTURER_MEM_SIZE),
                            MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
w25qxx_init (manikin_spi_memory_ctx_t *mem_ctx)
{
    manikin_status_t status = w25qxx_check_id(mem_ctx);
    MANIKIN_ASSERT(W25QXX_HASH, (status == MANIKIN_STATUS_OK), status);
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
    return w25qxx_check_id(mem_ctx);
}

static void
w25qxx_wait_while_busy (manikin_spi_memory_ctx_t *mem_ctx)
{
    uint8_t cmd = W25QXX_REG_STATUS; // Read status register
    uint8_t status;

    do
    {
        manikin_spi_start_transaction(mem_ctx->spi_cs);
        manikin_spi_write(mem_ctx->spi, &cmd, 1);
        manikin_spi_read(mem_ctx->spi, &status, 1);
        manikin_spi_end_transaction(mem_ctx->spi_cs);
    } while (status & W25QXX_REG_STATUS_BUSY); // Wait while BUSY bit is set
}

manikin_memory_result_t
w25qxx_write (manikin_spi_memory_ctx_t *mem_ctx, uint8_t *data, uint32_t lba, size_t len)
{
    if (!mem_ctx || !data || len == 0)
    {
        return MANIKIN_MEMORY_RESULT_PARERR;
    }

    uint32_t addr      = lba * 256;
    size_t   remaining = len;
    uint8_t *src       = data;

    while (remaining > 0)
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

        manikin_spi_start_transaction(mem_ctx->spi_cs);
        manikin_spi_write(mem_ctx->spi, w25qxx_reg_write_buf, W25QXX_REG_PAGE_PROGRAM_SIZE);
        manikin_spi_write(mem_ctx->spi, src, chunk);
        manikin_spi_end_transaction(mem_ctx->spi_cs);

        // --- Wait for write to complete ---
        w25qxx_wait_while_busy(mem_ctx);

        addr += chunk;
        src += chunk;
        remaining -= chunk;
    }

    return MANIKIN_MEMORY_RESULT_OK;
}

manikin_memory_result_t
w25qxx_read (manikin_spi_memory_ctx_t *mem_ctx, uint8_t *data, uint32_t lba, size_t len)
{

    manikin_status_t status = w25qxx_check_id(mem_ctx);
    MANIKIN_ASSERT(W25QXX_HASH, status == MANIKIN_STATUS_OK, status);
    uint32_t addr      = lba * 256; // 256 = page size
    size_t   remaining = len;

    manikin_spi_start_transaction(mem_ctx->spi_cs);

    w25qxx_reg_write_buf[0] = W25QXX_REG_READ; // Read command
    w25qxx_reg_write_buf[1] = (addr >> 16) & 0xFF;
    w25qxx_reg_write_buf[2] = (addr >> 8) & 0xFF;
    w25qxx_reg_write_buf[3] = addr & 0xFF;

    manikin_spi_write(mem_ctx->spi, w25qxx_reg_write_buf, W25QXX_REG_READ_SIZE);
    while (remaining > 0)
    {
        size_t chunk = remaining > 256 ? 256 : remaining;

        manikin_spi_read(mem_ctx->spi, data, chunk);

        data += chunk;
        addr += chunk;
        remaining -= chunk;
    }
    manikin_spi_end_transaction(mem_ctx->spi_cs);
    return MANIKIN_MEMORY_RESULT_OK;
}

manikin_status_t
w25qxx_erase_sector (manikin_spi_memory_ctx_t *mem_ctx, uint32_t sector_number)
{
    uint32_t addr = sector_number * W25QXX_SECTOR_SIZE; // sector size = 4KB

    // Enable write (0x06)
    w25qxx_reg_write_buf[0] = W25QXX_REG_WREN;
    manikin_spi_start_transaction(mem_ctx->spi_cs);
    manikin_spi_write(mem_ctx->spi, w25qxx_reg_write_buf, 1);
    manikin_spi_end_transaction(mem_ctx->spi_cs);

    // Sector erase (0x20) + 3-byte address
    w25qxx_reg_write_buf[0] = W25QXX_REG_SECTOR_ERASE;
    w25qxx_reg_write_buf[1] = (addr >> 16) & 0xFF;
    w25qxx_reg_write_buf[2] = (addr >> 8) & 0xFF;
    w25qxx_reg_write_buf[3] = addr & 0xFF;

    manikin_spi_start_transaction(mem_ctx->spi_cs);
    manikin_spi_write(mem_ctx->spi, w25qxx_reg_write_buf, W25QXX_REG_SECTOR_ERASE_SIZE);
    manikin_spi_end_transaction(mem_ctx->spi_cs);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
w25qxx_deinit (manikin_spi_memory_ctx_t *mem_ctx)
{
    return MANIKIN_STATUS_OK;
}
