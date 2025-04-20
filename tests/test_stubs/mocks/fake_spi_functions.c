#include "fake_spi_functions.h"
DEFINE_FFF_GLOBALS;

DEFINE_FAKE_VALUE_FUNC2(int, spi_hal_init, manikin_spi_inst_t, uint32_t);

DEFINE_FAKE_VALUE_FUNC3(size_t, spi_hal_write_bytes, manikin_spi_inst_t, const uint8_t *, size_t);

DEFINE_FAKE_VALUE_FUNC3(size_t, spi_hal_read_bytes, manikin_spi_inst_t, uint8_t *, size_t);

DEFINE_FAKE_VALUE_FUNC1(int, spi_hal_deinit, manikin_spi_inst_t);

DEFINE_FAKE_VALUE_FUNC2(int, spi_hal_set_cs, manikin_spi_cs_t, const uint8_t);