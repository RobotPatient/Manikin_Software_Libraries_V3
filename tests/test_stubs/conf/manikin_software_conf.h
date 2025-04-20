#ifndef MANIKIN_CONF_H
#define MANIKIN_CONF_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>

#define MANIKIN_SOFTWARE_MIN_SPI_SPEED (10e6)
#define MANIKIN_SOFTWARE_MAX_SPI_SPEED (50e6)

    typedef uint8_t *manikin_i2c_inst_t;

    typedef uint8_t *manikin_timer_inst_t;

    typedef uint8_t *manikin_watchdog_inst_t;

    typedef struct
    {
        uint8_t port;
        uint8_t pin;
    } manikin_spi_cs_t;

    typedef uint8_t *manikin_spi_inst_t;

#ifdef __cplusplus
}
#endif
#endif /* MANIKIN_CONF_H */