#ifndef FAKE_SPI_FUNCTIONS_H
#define FAKE_SPI_FUNCTIONS_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "manikin_software_conf.h"
#include "fff.h"

    DECLARE_FAKE_VALUE_FUNC2(int, spi_hal_init, manikin_spi_inst_t, uint32_t);

    DECLARE_FAKE_VALUE_FUNC3(
        size_t, spi_hal_write_bytes, manikin_spi_inst_t, const uint8_t *, size_t);

    DECLARE_FAKE_VALUE_FUNC3(size_t, spi_hal_read_bytes, manikin_spi_inst_t, uint8_t *, size_t);

    DECLARE_FAKE_VALUE_FUNC2(int, spi_hal_set_cs, manikin_spi_cs_t, const uint8_t);

    DECLARE_FAKE_VALUE_FUNC1(int, spi_hal_deinit, manikin_spi_inst_t);

#ifdef __cplusplus
}
#endif
#endif /* FAKE_SPI_FUNCTIONS_H */
