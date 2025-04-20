#ifndef W25QXX128_H
#define W25QXX128_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "common/manikin_types.h"

    manikin_status_t w25qxx_init(manikin_spi_memory_ctx_t *mem_ctx);

    manikin_status_t w25qxx_status(manikin_spi_memory_ctx_t *mem_ctx);

    manikin_memory_result_t w25qxx_ioctl(manikin_spi_memory_ctx_t *mem_ctx,
                                         uint8_t                   cmd,
                                         void                     *buff);

    manikin_memory_result_t w25qxx_write(manikin_spi_memory_ctx_t *mem_ctx,
                                         const uint8_t            *data,
                                         uint64_t                  lba,
                                         size_t                    len);
    manikin_memory_result_t w25qxx_read(manikin_spi_memory_ctx_t *mem_ctx,
                                        uint8_t                  *data,
                                        uint64_t                  lba,
                                        size_t                    len);

    manikin_status_t w25qxx_deinit(manikin_spi_memory_ctx_t *mem_ctx);

#ifdef __cplusplus
}
#endif
#endif /* W25QXX128_H */
