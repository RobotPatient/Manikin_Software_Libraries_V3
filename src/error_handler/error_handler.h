/**
 * @file            error_handler.h
 * @brief           Assert & Error handling module
 * modules
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

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>

/**
 * @brief Critical assert macro
 * @param FILE_HASH CRC32 hash of the filename
 * @param cond The condition to check
 * @param retval The return value which gets returned when check fails
 * @return The retval which was specified as parameter
 */
#define MANIKIN_ASSERT(FILE_HASH, cond, retval)               \
    do                                                        \
    {                                                         \
        if (!(cond))                                          \
        {                                                     \
            manikin_critical_assert((FILE_HASH), (__LINE__)); \
            return (retval);                                  \
        }                                                     \
    } while (0)

/**
 * @brief Non-critical assert macro
 * @param FILE_HASH CRC32 hash of the filename
 * @param cond The condition to check
 * @param retval The return value which gets returned when check fails
 * @return The retval which was specified as parameter
 */
#define MANIKIN_NON_CRIT_ASSERT(FILE_HASH, cond, retval)          \
    do                                                            \
    {                                                             \
        if (!(cond))                                              \
        {                                                         \
            manikin_non_critical_assert((FILE_HASH), (__LINE__)); \
            return (retval);                                      \
        }                                                         \
    } while (0)

    /**
     * @brief Critical assert handler
     * @param file_crc32 CRC32 hash of the filename
     * @param line The line on which assert was specified
     */
    void manikin_critical_assert(uint32_t file_crc32, int line);

    /**
     * @brief Non-critical assert handler
     * @param file_crc32 CRC32 hash of the filename
     * @param line The line on which assert was specified
     */
    void manikin_non_critical_assert(uint32_t file_crc32, int line);

#ifdef __cplusplus
}
#endif
#endif // ERROR_HANDLER_H
