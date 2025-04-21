/**
 * @file            manikin_software_conf.h
 * @brief           Platform typedef configuration for Manikin Libraries V3 library
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

#ifndef MANIKIN_CONF_H
#define MANIKIN_CONF_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>

#define MANIKIN_SOFTWARE_MAX_TIMEOUT   1u
#define MANIKIN_FLASH_MAX_RETRIES      3u
#define MANIKIN_SOFTWARE_MIN_SPI_SPEED (10u * 10e6)
#define MANIKIN_SOFTWARE_MAX_SPI_SPEED (50u * 10e6)

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