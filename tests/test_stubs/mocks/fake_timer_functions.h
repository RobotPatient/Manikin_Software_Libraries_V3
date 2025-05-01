/**
 * @file             fake_timer_functions.h
 * @brief            Test stubs used in software tests for testing timer module
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

#ifndef FAKE_TIMER_FUNCTIONS_H
#define FAKE_TIMER_FUNCTIONS_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stddef.h>
#include "fff.h"
#include "manikin_software_conf.h"

    DECLARE_FAKE_VALUE_FUNC2(uint32_t, timer_hal_init, manikin_timer_inst_t, uint32_t)
    DECLARE_FAKE_VALUE_FUNC1(uint32_t, timer_hal_start, manikin_timer_inst_t)
    DECLARE_FAKE_VALUE_FUNC1(uint32_t, timer_hal_stop, manikin_timer_inst_t)
    DECLARE_FAKE_VALUE_FUNC1(uint32_t, timer_hal_deinit, manikin_timer_inst_t)
    DECLARE_FAKE_VALUE_FUNC0(size_t, timer_hal_get_tick)
#ifdef __cplusplus
}
#endif
#endif /* FAKE_TIMER_FUNCTIONS_H */
