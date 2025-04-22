/**
 * @file            bmm350_regs.h
 * @brief           Header file with registers for the Bosch BMM350 3-axis magnetometer
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

#ifndef BMM350_REGS_H
#define BMM350_REGS_H
#ifdef __cplusplus
extern "C"
{
#endif

#define BMM350_SOFT_RESET_DELAY       24000u
#define BMM350_MAGNETIC_RESET_DELAY   40000u
#define BMM350_START_UP_TIME_FROM_POR 3000u

#define BMM350_GOTO_SUSPEND_DELAY      6000u
#define BMM350_SUSPEND_TO_NORMAL_DELAY 38000u

#define BMM350_SUS_TO_FORCEDMODE_NO_AVG_DELAY 15000u
#define BMM350_SUS_TO_FORCEDMODE_AVG_2_DELAY  17000u
#define BMM350_SUS_TO_FORCEDMODE_AVG_4_DELAY  20000u
#define BMM350_SUS_TO_FORCEDMODE_AVG_8_DELAY  28000u

#define BMM350_SUS_TO_FORCEDMODE_FAST_NO_AVG_DELAY 4000u
#define BMM350_SUS_TO_FORCEDMODE_FAST_AVG_2_DELAY  5000u
#define BMM350_SUS_TO_FORCEDMODE_FAST_AVG_4_DELAY  9000u
#define BMM350_SUS_TO_FORCEDMODE_FAST_AVG_8_DELAY  16000u

#define BMM350_UPD_OAE_DELAY 1000u

#define BMM350_BR_DELAY  14000u
#define BMM350_FGR_DELAY 18000u

/************************ Length macros ************************/
#define BMM350_OTP_DATA_LENGTH    32u
#define BMM350_MAG_TEMP_DATA_LEN  12u

/************************ Averaging macros **********************/
#define BMM350_AVG_SHIFT(x) (x << 4u)
#define BMM350_AVG_NO_AVG   0x0u
#define BMM350_AVG_2        0x1u
#define BMM350_AVG_4        0x2u
#define BMM350_AVG_8        0x3u

/******************************* ODR **************************/
#define BMM350_ODR_400HZ    0x2u
#define BMM350_ODR_200HZ    0x3u
#define BMM350_ODR_100HZ    0x4u
#define BMM350_ODR_50HZ     0x5u
#define BMM350_ODR_25HZ     0x6u
#define BMM350_ODR_12_5HZ   0x7u
#define BMM350_ODR_6_25HZ   0x8u
#define BMM350_ODR_3_125HZ  0x9u
#define BMM350_ODR_1_5625HZ 0xAu

/********************* Power modes *************************/
#define BMM350_PMU_CMD_SUS      0x00u
#define BMM350_PMU_CMD_NM       0x01u
#define BMM350_PMU_CMD_UPD_OAE  0x02u
#define BMM350_PMU_CMD_FM       0x03u
#define BMM350_PMU_CMD_FM_FAST  0x04u
#define BMM350_PMU_CMD_FGR      0x05u
#define BMM350_PMU_CMD_FGR_FAST 0x06u
#define BMM350_PMU_CMD_BR       0x07u
#define BMM350_PMU_CMD_BR_FAST  0x08u
#define BMM350_PMU_CMD_NM_TC    0x09u

#define BMM350_PMU_STATUS_0 0x0u
#define BMM350_AGGR_CLR     0x0u

#define BMM350_DISABLE 0x0u
#define BMM350_ENABLE  0x1u

#define BMM350_CMD_NOP       0x0u
#define BMM350_CMD_SOFTRESET 0xB6u

#define BMM350_TARGET_PAGE_PAGE0 0x0u
#define BMM350_TARGET_PAGE_PAGE1 0x1u

#define BMM350_INT_MODE_LATCHED 0x1u
#define BMM350_INT_MODE_PULSED  0x0u

#define BMM350_INT_POL_ACTIVE_HIGH 0x1u
#define BMM350_INT_POL_ACTIVE_LOW  0x0u

#define BMM350_INT_OD_PUSHPULL  0x1u
#define BMM350_INT_OD_OPENDRAIN 0x0u

#define BMM350_INT_OUTPUT_EN_OFF 0x0u
#define BMM350_INT_OUTPUT_EN_ON  0x1u

#define BMM350_INT_DRDY_EN  0x1u
#define BMM350_INT_DRDY_DIS 0x0u

#define BMM350_MR_MR1K8 0x0u
#define BMM350_MR_MR2K1 0x1u
#define BMM350_MR_MR1K5 0x2u
#define BMM350_MR_MR0K6 0x3u

#define BMM350_SEL_DTB1X_PAD_PAD_INT 0x0u
#define BMM350_SEL_DTB1X_PAD_PAD_BYP 0x1u

#define BMM350_TMR_TST_HIZ_VTMR_VTMR_ON  0x0u
#define BMM350_TMR_TST_HIZ_VTMR_VTMR_HIZ 0x1u

#define BMM350_LSB_MASK  0x00FFu
#define BMM350_MSB_MASK  0xFF00u
#define BMM350_AXIS_EN_X 0x01u
#define BMM350_AXIS_EN_Y 0x02u
#define BMM350_AXIS_EN_Z 0x04u

/********************** Pad drive strength ************************/
#define BMM350_PAD_DRIVE_WEAKEST   0u
#define BMM350_PAD_DRIVE_STRONGEST 7u

/********************** I2C Register Addresses ************************/

/*! Register to set I2C address to LOW */
#define BMM350_I2C_ADSEL_SET_LOW 0x14u

/*! Register to set I2C address to HIGH */
#define BMM350_I2C_ADSEL_SET_HIGH 0x15u

#define BMM350_DUMMY_BYTES        2u
#define BMM350_READ_BUFFER_LENGTH 14u

    /********************** Register Addresses ************************/
#define BMM350_REG_SEL 0x00u

#define BMM350_REG_CHIP_ID           0x00u
#define BMM350_REG_REV_ID            0x01u
#define BMM350_REG_ERR_REG           0x02u
#define BMM350_REG_PAD_CTRL          0x03u
#define BMM350_REG_PMU_CMD_AGGR_SET  0x04u
#define BMM350_REG_PMU_CMD_AXIS_EN   0x05u
#define BMM350_REG_PMU_CMD           0x06u
#define BMM350_REG_PMU_CMD_STATUS_0  0x07u
#define BMM350_REG_PMU_CMD_STATUS_1  0x08u
#define BMM350_REG_I3C_ERR           0x09u
#define BMM350_REG_I2C_WDT_SET       0x0Au
#define BMM350_REG_TRSDCR_REV_ID     0x0Du
#define BMM350_REG_TC_SYNC_TU        0x21u
#define BMM350_REG_TC_SYNC_ODR       0x22u
#define BMM350_REG_TC_SYNC_TPH_1     0x23u
#define BMM350_REG_TC_SYNC_TPH_2     0x24u
#define BMM350_REG_TC_SYNC_DT        0x25u
#define BMM350_REG_TC_SYNC_ST_0      0x26u
#define BMM350_REG_TC_SYNC_ST_1      0x27u
#define BMM350_REG_TC_SYNC_ST_2      0x28u
#define BMM350_REG_TC_SYNC_STATUS    0x29u
#define BMM350_REG_INT_CTRL          0x2Eu
#define BMM350_REG_INT_CTRL_IBI      0x2Fu
#define BMM350_REG_INT_STATUS        0x30u
#define BMM350_REG_MAG_X_XLSB        0x31u
#define BMM350_REG_MAG_X_LSB         0x32u
#define BMM350_REG_MAG_X_MSB         0x33u
#define BMM350_REG_MAG_Y_XLSB        0x34u
#define BMM350_REG_MAG_Y_LSB         0x35u
#define BMM350_REG_MAG_Y_MSB         0x36u
#define BMM350_REG_MAG_Z_XLSB        0x37u
#define BMM350_REG_MAG_Z_LSB         0x38u
#define BMM350_REG_MAG_Z_MSB         0x39u
#define BMM350_REG_TEMP_XLSB         0x3Au
#define BMM350_REG_TEMP_LSB          0x3Bu
#define BMM350_REG_TEMP_MSB          0x3Cu
#define BMM350_REG_SENSORTIME_XLSB   0x3Du
#define BMM350_REG_SENSORTIME_LSB    0x3Eu
#define BMM350_REG_SENSORTIME_MSB    0x3Fu
#define BMM350_REG_OTP_CMD_REG       0x50u
#define BMM350_REG_OTP_DATA_MSB_REG  0x52u
#define BMM350_REG_OTP_DATA_LSB_REG  0x53u
#define BMM350_REG_OTP_STATUS_REG    0x55u
#define BMM350_REG_TMR_SELFTEST_USER 0x60u
#define BMM350_REG_CTRL_USER         0x61u
#define BMM350_REG_CMD               0x7Eu

#ifdef __cplusplus
}
#endif
#endif // BMM350_REGS_H
