/**
 * @file            sdp810_regs.h
 * @brief           Header file with registers for the Sensirion SDP810 Differential pressure sensor
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

#ifndef SDP810_REGS_H
#define SDP810_REGS_H
#ifdef __cplusplus
extern "C"
{
#endif

#define SDP810_REG_CONT_MASS_FLOW     (0x36u << 8u | 0x03u)
#define SDP810_READ_BUFFER_SIZE       9u
#define SDP810_READ_DIFF_PRESSURE_MSB 0u
#define SDP810_READ_DIFF_PRESSURE_LSB 1u
#define SDP810_READ_DIFF_PRESSURE_CRC 2u
#define SDP810_READ_TEMPERATURE_MSB   3u
#define SDP810_READ_TEMPERATURE_LSB   4u
#define SDP810_READ_TEMPERATURE_CRC   5u
#define SDP810_READ_SCALE_FACTOR_MSB  6u
#define SDP810_READ_SCALE_FACTOR_LSB  7u
#define SDP810_READ_SCALE_FACTOR_CRC  8u

#ifdef __cplusplus
}
#endif
#endif // SDP810_REGS_H
