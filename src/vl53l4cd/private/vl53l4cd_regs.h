/**
 * @file            vl53l4cd_regs.h
 * @brief           Header file with registers for the STM VL53L4CD ranging ToF sensor
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
 * Author:          Johan Korten
 */

#ifndef VL53L4CD_REGS_H
#define VL53L4CD_REGS_H
#ifdef __cplusplus
extern "C"
{
#endif

/* Device identification */
#define VL53L4CD_IDENTIFICATION_MODEL_ID 0x010Fu

/* System registers */
#define VL53L4CD_FIRMWARE_SYSTEM_STATUS    0x00E5u
#define VL53L4CD_SYSTEM_INTERRUPT_CLEAR    0x0086u
#define VL53L4CD_SYSTEM_START              0x0087u
#define VL53L4CD_SYSTEM_FRESH_OUT_OF_RESET 0x0016u /* Similar to VL6180X */

/* Ranging configuration registers */
#define VL53L4CD_RANGE_CONFIG_A            0x005Eu
#define VL53L4CD_RANGE_CONFIG_B            0x0061u
#define VL53L4CD_RANGE_CONFIG_SIGMA_THRESH 0x0064u
#define VL53L4CD_INTERMEASUREMENT_MS       0x006Cu
#define VL53L4CD_THRESH_HIGH               0x0072u
#define VL53L4CD_THRESH_LOW                0x0074u

/* Result registers */
#define VL53L4CD_RESULT_RANGE_STATUS   0x0089u
#define VL53L4CD_RESULT_SPAD_NB        0x008Cu
#define VL53L4CD_RESULT_SIGNAL_RATE    0x008Eu
#define VL53L4CD_RESULT_AMBIENT_RATE   0x0090u
#define VL53L4CD_RESULT_SIGMA          0x0092u
#define VL53L4CD_RESULT_FINAL_RANGE_MM 0x0096u
#define VL53L4CD_RESULT__OSC_CALIBRATE_VAL      ((uint16_t)0x00DE)
/* VHV configuration registers */
#define VL53L4CD_VHV_CONFIG_TIMEOUT_MACROP_LOOP_BOUND 0x0008u

/* I2C and configuration registers */
#define VL53L4CD_I2C_SLAVE_DEVICE_ADDRESS 0x0001u
#define VL53L4CD_OSC_CALIBRATE_VAL        0x00DEu

/* Default I2C addresses */
#define VL53L4CD_DEFAULT_ADDR     0x52u /* 7-bit address */
#define VL53L4CD_DEFAULT_ADDR_ALT 0x29u /* Alternative address */

/* Start/stop values */
#define VL53L4CD_RANGING_START 0x40u
#define VL53L4CD_RANGING_STOP  0x00u

/* Interrupt clear value */
#define VL53L4CD_CLEAR_INTERRUPT 0x01u

/* Model ID value */
#define VL53L4CD_MODEL_ID_VALUE 0xEACCu

/* Status codes */
#define VL53L4CD_NO_ERROR 0u
#define VL53L4CD_ERROR    1u

#ifdef __cplusplus
}
#endif
#endif // VL53L4CD_REGS_H
