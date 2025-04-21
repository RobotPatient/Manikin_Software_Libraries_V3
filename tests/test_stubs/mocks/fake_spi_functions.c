/**
 * @file             fake_spi_functions.c
 * @brief            Test stubs used in software tests for testing spi module
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

#include "fake_spi_functions.h"
DEFINE_FFF_GLOBALS;

DEFINE_FAKE_VALUE_FUNC2(int, spi_hal_init, manikin_spi_inst_t, uint32_t);

DEFINE_FAKE_VALUE_FUNC3(size_t, spi_hal_write_bytes, manikin_spi_inst_t, const uint8_t *, size_t);

DEFINE_FAKE_VALUE_FUNC3(size_t, spi_hal_read_bytes, manikin_spi_inst_t, uint8_t *, size_t);

DEFINE_FAKE_VALUE_FUNC1(int, spi_hal_deinit, manikin_spi_inst_t);

DEFINE_FAKE_VALUE_FUNC2(int, spi_hal_set_cs, manikin_spi_cs_t, const uint8_t);