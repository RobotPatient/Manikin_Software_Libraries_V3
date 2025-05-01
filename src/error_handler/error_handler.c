/**
 * @file            error_handler.c
 * @brief           Source-file with error-handler implementation
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

#include "error_handler.h"
#include "manikin_platform.h"

void
manikin_critical_assert (const uint32_t file_crc32, const int line)
{
    MANIKIN_CRIT_ERROR_HANDLER(file_crc32, line);
}

void
manikin_non_critical_assert (const uint32_t file_crc32, const int line)
{
    MANIKIN_NON_CRIT_ERROR_HANDLER(file_crc32, line);
}
