/**
 * @file             fake_watchdog_functions.h
 * @brief            Test stubs used in software tests for testing watchdog module
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

#ifndef FAKE_WATCHDOG_FUNCTIONS_H
#define FAKE_WATCHDOG_FUNCTIONS_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stddef.h>
#include "fff.h"
#include "manikin_software_conf.h"

    DECLARE_FAKE_VALUE_FUNC2(int, watchdog_hal_init, manikin_watchdog_inst_t, uint32_t)
    DECLARE_FAKE_VALUE_FUNC1(int, watchdog_hal_kick, manikin_watchdog_inst_t)
    DECLARE_FAKE_VALUE_FUNC1(int, watchdog_hal_deinit, manikin_watchdog_inst_t)

#ifdef __cplusplus
}
#endif
#endif /* FAKE_WATCHDOG_FUNCTIONS_H */
