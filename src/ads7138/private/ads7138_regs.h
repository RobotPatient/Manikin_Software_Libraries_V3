/**
 * @file            ads7138_regs.h
 * @brief           Header file with registers for the TI ADS7138 8-channel 16-bit ADC
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

#ifndef ADS7138_REGS_H
#define ADS7138_REGS_H
#ifdef __cplusplus
extern "C"
{
#endif
#define ADS7138_REG(reg, opcode)    (reg | opcode << 8u)
#define ADS7138_READ_BUF_SIZE       16u // 2 bytes per channel, 8 channels
#define ADS7138_NUMBER_OF_CHANNELS  8u
#define ADS7138_REG_SYSTEM_STATUS   0x00u
#define ADS7138_REG_GEN_CFG         0x01u
#define ADS7138_REG_PIN_CFG         0x05u
#define ADS7138_REG_SEQUENCE_CFG    0x10u
#define ADS7138_REG_AUTO_SEQ_CH_SEL 0x12u

#define ADS7138_REG_ALL_PINS_ANALOG_INP_BIT 0x00u
#define ADS7138_REG_CAL_BIT                 0x02u
#define ADS7138_REG_SW_RESET_BIT            0x01u
#define ADS7138_REG_AUTO_SEQ_ALL_CH_SEL     0xFFu
#define ADS7138_REG_SEQ_START_BIT           (1 << 4u)
#define ADS7138_REG_ENABLE_AUTO_SEQ         0x01u
#define ADS7138_REG_SYSTEM_STATUS_RSVD_BIT  (7u)

#define ADS7138_OP_SINGLE_READ      0x10u
#define ADS7138_OP_SINGLE_WRITE     0x10u
#define ADS7138_OP_SET_BIT          0x18u
#define ADS7138_OP_CLEAR_BIT        0x20u
#define ADS7138_OP_CONTINUOUS_READ  0x30u
#define ADS7138_OP_CONTINUOUS_WRITE 0x28u

#ifdef __cplusplus
}
#endif
#endif /* ADS7138_REGS_H */
