/**
 * @file            vl6180x_regs.h
 * @brief           Header file with registers for the STM VL6180x ranging ToF sensor
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

#ifndef VL6180X_REGS_H
#define VL6180X_REGS_H
#ifdef __cplusplus
extern "C"
{
#endif

#define VL6180X_REG_IDENTIFICATION_MODEL_ID              0x0000u
#define VL6180X_REG_IDENTIFICATION_MODEL_REV_MAJOR       0x0001u
#define VL6180X_REG_IDENTIFICATION_MODEL_REV_MINOR       0x0002u
#define VL6180X_REG_IDENTIFICATION_MODULE_REV_MAJOR      0x0003u
#define VL6180X_REG_IDENTIFICATION_MODULE_REV_MINOR      0x0004u
#define VL6180X_REG_IDENTIFICATION_DATE_HI               0x0006u
#define VL6180X_REG_IDENTIFICATION_DATE_LO               0x0007u
#define VL6180X_REG_IDENTIFICATION_TIME                  0x0008u
#define VL6180X_REG_SYSTEM_MODE_GPIO0                    0x0010u
#define VL6180X_REG_SYSTEM_MODE_GPIO1                    0x0011u
#define VL6180X_REG_SYSTEM_HISTORY_CTRL                  0x0012u
#define VL6180X_REG_SYSTEM_INTERRUPT_CONFIG_GPIO         0x0014u
#define VL6180X_REG_SYSTEM_INTERRUPT_CLEAR               0x0015u
#define VL6180X_REG_SYSTEM_FRESH_OUT_OF_RESET            0x0016u
#define VL6180X_REG_SYSTEM_GROUPED_PARAMETER_HOLD        0x0017u
#define VL6180X_REG_SYSRANGE_START                       0x0018u
#define VL6180X_REG_SYSRANGE_THRESH_HIGH                 0x0019u
#define VL6180X_REG_SYSRANGE_THRESH_LOW                  0x001Au
#define VL6180X_REG_SYSRANGE_INTERMEASUREMENT_PERIOD     0x001Bu
#define VL6180X_REG_SYSRANGE_MAX_CONVERGENCE_TIME        0x001Cu
#define VL6180X_REG_SYSRANGE_CROSSTALK_COMPENSATION_RATE 0x001Eu
#define VL6180X_REG_SYSRANGE_CROSSTALK_VALID_HEIGHT      0x0021u
#define VL6180X_REG_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE  0x0022u
#define VL6180X_REG_SYSRANGE_PART_TO_PART_RANGE_OFFSET   0x0024u
#define VL6180X_REG_SYSRANGE_RANGE_IGNORE_VALID_HEIGHT   0x0025u
#define VL6180X_REG_SYSRANGE_RANGE_IGNORE_THRESHOLD      0x0026u
#define VL6180X_REG_SYSRANGE_MAX_AMBIENT_LEVEL_MULT      0x002Cu
#define VL6180X_REG_SYSRANGE_RANGE_CHECK_ENABLES         0x002Du
#define VL6180X_REG_SYSRANGE_VHV_RECALIBRATE             0x002Eu
#define VL6180X_REG_SYSRANGE_VHV_REPEAT_RATE             0x0031u
#define VL6180X_REG_SYSALS_START                         0x0038u
#define VL6180X_REG_SYSALS_THRESH_HIGH                   0x003Au
#define VL6180X_REG_SYSALS_THRESH_LOW                    0x003Cu
#define VL6180X_REG_SYSALS_INTERMEASUREMENT_PERIOD       0x003Eu
#define VL6180X_REG_SYSALS_ANALOGUE_GAIN                 0x003Fu
#define VL6180X_REG_SYSALS_INTEGRATION_PERIOD            0x0040u
#define VL6180X_REG_RESULT_RANGE_STATUS                  0x004Du
#define VL6180X_REG_RESULT_ALS_STATUS                    0x004Eu
#define VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO         0x004Fu
#define VL6180X_REG_RESULT_ALS_VAL                       0x0050u
#define VL6180X_REG_RESULT_HISTORY_BUFFER                0x0052u // 0x052 to 0x060
#define VL6180X_REG_RESULT_RANGE_VAL                     0x0062u
#define VL6180X_REG_RESULT_RANGE_RAW                     0x0064u
#define VL6180X_REG_RESULT_RANGE_RETURN_RATE             0x0066u
#define VL6180X_REG_RESULT_RANGE_REFERENCE_RATE          0x0068u
#define VL6180X_REG_RESULT_RANGE_RETURN_SIGNAL_COUNT     0x006Cu
#define VL6180X_REG_RESULT_RANGE_REFERENCE_SIGNAL_COUNT  0x0070u
#define VL6180X_REG_RESULT_RANGE_RETURN_AMB_COUNT        0x0074u
#define VL6180X_REG_RESULT_RANGE_REFERENCE_AMB_COUNT     0x0078u
#define VL6180X_REG_RESULT_RANGE_RETURN_CONV_TIME        0x007Cu
#define VL6180X_REG_RESULT_RANGE_REFERENCE_CONV_TIME     0x0080u
#define VL6180X_REG_READOUT_AVERAGING_SAMPLE_PERIOD      0x010Au
#define VL6180X_REG_FIRMWARE_BOOTUP                      0x0119u
#define VL6180X_REG_FIRMWARE_RESULT_SCALER               0x0120u
#define VL6180X_REG_I2C_SLAVE_DEVICE_ADDRESS             0x0212u
#define VL6180X_REG_INTERLEAVED_MODE_ENABLE              0x02A3u

#ifdef __cplusplus
}
#endif
#endif // VL6180X_REGS_H