#include "w25qxx128.h"

#include "manikin_platform.h"
#include "error_handler/error_handler.h"
#include "spi/spi.h"
#include "private/w25qxx128_regs.h"

#define W25QXX_HASH 0xAE5DF8E6

manikin_status_t
w25qxx_check_id (manikin_spi_memory_ctx_t *mem_ctx)
{
    const uint8_t check_id = 0x9F;
    uint8_t       read_buf[3];
    manikin_spi_start_transaction(mem_ctx->spi_cs);
    manikin_spi_write(mem_ctx->spi, &check_id, 1);
    manikin_spi_read(mem_ctx->spi, read_buf, 3);
    manikin_spi_end_transaction(mem_ctx->spi_cs);
    MANIKIN_NON_CRIT_ASSERT(W25QXX_HASH,
                            (read_buf[0] == W25QXX_RESP_MANUFACTURER_ID
                             && read_buf[1] == W25QXX_RESP_MANUFACTURER_MEM_TYPE
                             && read_buf[2] == W25QXX_RESP_MANUFACTURER_MEM_SIZE),
                            MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
w25qxx_init (manikin_spi_memory_ctx_t *mem_ctx)
{
    manikin_status_t status = w25qxx_check_id(mem_ctx);
    MANIKIN_ASSERT(W25QXX_HASH, (status == MANIKIN_STATUS_OK), status);
    const uint8_t reset_data[2] = { 0x66, 0x99 };
    manikin_spi_start_transaction(mem_ctx->spi_cs);
    manikin_spi_write(mem_ctx->spi, reset_data, 2);
    manikin_spi_end_transaction(mem_ctx->spi_cs);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
w25qxx_status (manikin_spi_memory_ctx_t *mem_ctx)
{
    return MANIKIN_STATUS_OK;
}

manikin_memory_result_t
w25qxx_ioctl (manikin_spi_memory_ctx_t *mem_ctx, uint8_t cmd, void *buff)
{
    return MANIKIN_MEMORY_RESULT_OK;
}

manikin_memory_result_t
w25qxx_write (manikin_spi_memory_ctx_t *mem_ctx, const uint8_t *data, uint64_t lba, size_t len)
{
    return MANIKIN_MEMORY_RESULT_OK;
}

manikin_memory_result_t
w25qxx_read (manikin_spi_memory_ctx_t *mem_ctx, uint8_t *data, uint64_t lba, size_t len)
{
    return MANIKIN_MEMORY_RESULT_OK;
}

manikin_status_t
w25qxx_deinit (manikin_spi_memory_ctx_t *mem_ctx)
{
    return MANIKIN_STATUS_OK;
}
