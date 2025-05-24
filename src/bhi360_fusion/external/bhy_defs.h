/**
 * Copyright (c) 2025 Bosch Sensortec GmbH. All rights reserved.
 *
 * BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @file       bhy_defs.h
 * @date       2025-03-28
 * @version    v2.1.0
 *
 */

#ifndef __BHY_DEFS_H__
#define __BHY_DEFS_H__

/* Start of CPP Guard */
#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus */

#ifdef __KERNEL__
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/types.h>
#else
#include <string.h>
#include <stdint.h>
#endif /*~ __KERNEL__ */

#ifndef UNUSED
#define UNUSED(__x__) ((void)(__x__))
#endif /*~ UNUSED */

#ifdef __KERNEL__
#define bhy_float u32
#else
#define bhy_float float
#endif /*~ __KERNEL__ */

#ifdef __KERNEL__

#if !defined(UINT8_C) && !defined(INT8_C)
#define INT8_C(x)  S8_C(x)
#define UINT8_C(x) U8_C(x)
#endif

#if !defined(UINT16_C) && !defined(INT16_C)
#define INT16_C(x)  S16_C(x)
#define UINT16_C(x) U16_C(x)
#endif

#if !defined(INT32_C) && !defined(UINT32_C)
#define INT32_C(x)  S32_C(x)
#define UINT32_C(x) U32_C(x)
#endif

#if !defined(INT64_C) && !defined(UINT64_C)
#define INT64_C(x)  S64_C(x)
#define UINT64_C(x) U64_C(x)
#endif

#else /* __KERNEL__ */

#if !defined(UINT8_C) && !defined(INT8_C)
#define INT8_C(x)  (x)
#define UINT8_C(x) (x##U)
#endif

#if !defined(UINT16_C) && !defined(INT16_C)
#define INT16_C(x)  (x)
#define UINT16_C(x) (x##U)
#endif

#if !defined(INT32_C) && !defined(UINT32_C)
#define INT32_C(x)  (x)
#define UINT32_C(x) (x##U)
#endif

#if !defined(INT64_C) && !defined(UINT64_C)
#define INT64_C(x)  (x##LL)
#define UINT64_C(x) (x##ULL)
#endif

#endif /* __KERNEL__ */

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#ifndef BHY_PACKED
#define BHY_PACKED __attribute__((__packed__))
#endif

/** API error codes */
#define BHY_OK                               INT8_C(0)
#define BHY_E_NULL_PTR                       INT8_C(-1)
#define BHY_E_INVALID_PARAM                  INT8_C(-2)
#define BHY_E_IO                             INT8_C(-3)
#define BHY_E_MAGIC                          INT8_C(-4)
#define BHY_E_TIMEOUT                        INT8_C(-5)
#define BHY_E_BUFFER                         INT8_C(-6)
#define BHY_E_INVALID_FIFO_TYPE              INT8_C(-7)
#define BHY_E_INVALID_EVENT_SIZE             INT8_C(-8)
#define BHY_E_PARAM_NOT_SET                  INT8_C(-9)
#define BHY_E_INSUFFICIENT_MAX_SIMUL_SENSORS INT8_C(-10)

#ifndef BHY_COMMAND_PACKET_LEN
#define BHY_COMMAND_PACKET_LEN UINT16_C(256)
#endif

#ifndef BHY_COMMAND_HEADER_LEN
#define BHY_COMMAND_HEADER_LEN UINT16_C(4)
#endif

#if (BHY_COMMAND_PACKET_LEN < BHY_COMMAND_HEADER_LEN)
#error "BHY_COMMAND_PACKET_LEN should be at least 4 bytes"
#endif

#define BHY_PRODUCT_ID UINT8_C(0x89)

/*! Register map */
#define BHY_REG_CHAN_CMD            UINT8_C(0x00)
#define BHY_REG_CHAN_FIFO_W         UINT8_C(0x01)
#define BHY_REG_CHAN_FIFO_NW        UINT8_C(0x02)
#define BHY_REG_CHAN_STATUS         UINT8_C(0x03)
#define BHY_REG_CHIP_CTRL           UINT8_C(0x05)
#define BHY_REG_HOST_INTERFACE_CTRL UINT8_C(0x06)
#define BHY_REG_HOST_INTERRUPT_CTRL UINT8_C(0x07)
#define BHY_REG_RESET_REQ           UINT8_C(0x14)
#define BHY_REG_TIME_EV_REQ         UINT8_C(0x15)
#define BHY_REG_HOST_CTRL           UINT8_C(0x16)
#define BHY_REG_HOST_STATUS         UINT8_C(0x17)
#define BHY_REG_CRC_0               UINT8_C(0x18) /* Totally 4 */
#define BHY_REG_PRODUCT_ID          UINT8_C(0x1C)
#define BHY_REG_REVISION_ID         UINT8_C(0x1D)
#define BHY_REG_ROM_VERSION_0       UINT8_C(0x1E) /* Totally 2 */
#define BHY_REG_KERNEL_VERSION_0    UINT8_C(0x20) /* Totally 2 */
#define BHY_REG_USER_VERSION_0      UINT8_C(0x22) /* Totally 2 */
#define BHY_REG_FEATURE_STATUS      UINT8_C(0x24)
#define BHY_REG_BOOT_STATUS         UINT8_C(0x25)
#define BHY_REG_HOST_INTR_TIME_0    UINT8_C(0x26) /* Totally 5 */
#define BHY_REG_CHIP_ID             UINT8_C(0x2B)
#define BHY_REG_INT_STATUS          UINT8_C(0x2D)
#define BHY_REG_ERROR_VALUE         UINT8_C(0x2E)
#define BHY_REG_ERROR_AUX           UINT8_C(0x2F)
#define BHY_REG_DEBUG_VALUE         UINT8_C(0x30)
#define BHY_REG_DEBUG_STATE         UINT8_C(0x31)
#define BHY_REG_GP_5                UINT8_C(0x32)
#define BHY_REG_GP_6                UINT8_C(0x36)
#define BHY_REG_GP_7                UINT8_C(0x3A)

/*! Command packets */
#define BHY_CMD_REQ_POST_MORTEM_DATA  UINT16_C(0x0001)
#define BHY_CMD_UPLOAD_TO_PROGRAM_RAM UINT16_C(0x0002)
#define BHY_CMD_BOOT_PROGRAM_RAM      UINT16_C(0x0003)
#define BHY_CMD_SET_INJECT_MODE       UINT16_C(0x0007)
#define BHY_CMD_INJECT_DATA           UINT16_C(0x0008)
#define BHY_CMD_FIFO_FLUSH            UINT16_C(0x0009)
#define BHY_CMD_SW_PASSTHROUGH        UINT16_C(0x000A)
#define BHY_CMD_REQ_SELF_TEST         UINT16_C(0x000B)
#define BHY_CMD_REQ_FOC               UINT16_C(0x000C)
#define BHY_CMD_CONFIG_SENSOR         UINT16_C(0x000D)
#define BHY_CMD_CHANGE_RANGE          UINT16_C(0x000E)
#define BHY_CMD_FIFO_FORMAT_CTRL      UINT16_C(0x0015)

/*! Soft passthrough feature */
#define BHY_SPASS_READ                  UINT8_C(0)
#define BHY_SPASS_WRITE                 UINT8_C(1)
#define BHY_SPASS_SINGLE_TRANS          UINT8_C(0)
#define BHY_SPASS_MULTI_TRANS           UINT8_C(1)
#define BHY_SPASS_DELAY_DIS             UINT8_C(0)
#define BHY_SPASS_DELAY_EN              UINT8_C(1)
#define BHY_SPASS_SIF1                  UINT8_C(1)
#define BHY_SPASS_SIF2                  UINT8_C(2)
#define BHY_SPASS_SIF3                  UINT8_C(3)
#define BHY_SPASS_SPI_4_WIRE            UINT8_C(0)
#define BHY_SPASS_SPI_3_WIRE            UINT8_C(1)
#define BHY_SPASS_SPI_CPOL_0            UINT8_C(0)
#define BHY_SPASS_SPI_CPOL_1            UINT8_C(1)
#define BHY_SPASS_SPI_CPHA_0            UINT8_C(0)
#define BHY_SPASS_SPI_CPHA_1            UINT8_C(1)
#define BHY_SPASS_SPI_CS_LOW            UINT8_C(0)
#define BHY_SPASS_SPI_CS_HIGH           UINT8_C(1)
#define BHY_SPASS_SPI_LSB_FIRST_DIS     UINT8_C(0)
#define BHY_SPASS_SPI_LSB_FIRST_EN      UINT8_C(1)
#define BHY_SPASS_SPI_READ_BIT_POL_LOW  UINT8_C(0)
#define BHY_SPASS_SPI_READ_BIT_POL_HIGH UINT8_C(1)
#define BHY_SPASS_SPI_READ_BIT_POS_0    UINT8_C(0)
#define BHY_SPASS_SPI_READ_BIT_POS_1    UINT8_C(1)
#define BHY_SPASS_SPI_READ_BIT_POS_2    UINT8_C(2)
#define BHY_SPASS_SPI_READ_BIT_POS_3    UINT8_C(3)
#define BHY_SPASS_SPI_READ_BIT_POS_4    UINT8_C(4)
#define BHY_SPASS_SPI_READ_BIT_POS_5    UINT8_C(5)
#define BHY_SPASS_SPI_READ_BIT_POS_6    UINT8_C(6)
#define BHY_SPASS_SPI_READ_BIT_POS_7    UINT8_C(7)
#define BHY_SPASS_READ_PACKET_LEN       UINT8_C(0x13)
#define BHY_SPASS_WRITE_RESP_PACKET_LEN UINT8_C(16)

/*! Helper macros */
#define BHY_CHK_BIT(data, bit)   (((uint32_t)data >> bit) & 0x01)
#define BHY_ROUND_WORD_LOWER(x)  ((x >> 2) << 2)
#define BHY_ROUND_WORD_HIGHER(x) (((x >> 2) + 1) << 2)

#define BHY_COUNTOF(__BUFFER__)          (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
#define BHY_DATA_INJECT_MODE_PAYLOAD_LEN UINT8_C(0x04)

/*! Firmware header identifier */
#define BHY_FW_MAGIC UINT16_C(0x662B)

/*! Boot status */
#define BHY_BST_HOST_INTERFACE_READY UINT8_C(0x10)
#define BHY_BST_HOST_FW_VERIFY_DONE  UINT8_C(0x20)
#define BHY_BST_HOST_FW_VERIFY_ERROR UINT8_C(0x40)
#define BHY_BST_HOST_FW_IDLE         UINT8_C(0x80)
#define BHY_BST_CHECK_RETRY          UINT8_C(100)

/*! Host status */
#define BHY_HST_POWER_STATE    UINT8_C(0x01)
#define BHY_HST_HOST_PROTOCOL  UINT8_C(0x02)
#define BHY_HST_HOST_CHANNEL_0 UINT8_C(0x10)
#define BHY_HST_HOST_CHANNEL_1 UINT8_C(0x20)
#define BHY_HST_HOST_CHANNEL_2 UINT8_C(0x40)
#define BHY_HST_HOST_CHANNEL_3 UINT8_C(0x80)

/*! Interrupt status masks */
#define BHY_IST_MASK_ASSERTED           (0x1)
#define BHY_IST_MASK_FIFO_W             (0x6)
#define BHY_IST_MASK_FIFO_NW            (0x18)
#define BHY_IST_MASK_STATUS             (0x20)
#define BHY_IST_MASK_DEBUG              (0x40)
#define BHY_IST_MASK_RESET_FAULT        (0x80)
#define BHY_IST_FIFO_W_DRDY             UINT8_C(0x2)
#define BHY_IST_FIFO_W_LTCY             UINT8_C(0x4)
#define BHY_IST_FIFO_W_WM               UINT8_C(0x6)
#define BHY_IST_FIFO_NW_DRDY            UINT8_C(0x8)
#define BHY_IST_FIFO_NW_LTCY            UINT8_C(0x10)
#define BHY_IST_FIFO_NW_WM              UINT8_C(0x18)
#define BHY_IS_INT_FIFO_W(status)       (status & BHY_IST_MASK_FIFO_W)
#define BHY_IS_INT_FIFO_NW(status)      (status & BHY_IST_MASK_FIFO_NW)
#define BHY_IS_INT_STATUS(status)       (status & BHY_IST_MASK_STATUS)
#define BHY_IS_INT_ASYNC_STATUS(status) (status & BHY_IST_MASK_DEBUG)
#define BHY_IS_INT_RESET(status)        (status & BHY_IST_MASK_RESET_FAULT)
#define BHY_IST_MASK_FIFO               (BHY_IST_MASK_FIFO_W | BHY_IST_MASK_FIFO_NW)
#define BHY_IS_INT_FIFO(status)         (status & BHY_IST_MASK_FIFO)

/*! Chip control bits */
#define BHY_CHIP_CTRL_DISABLE      UINT8_C(0x00)
#define BHY_CHIP_CTRL_TURBO_ENABLE UINT8_C(0x01)
#define BHY_CHIP_CTRL_CLR_ERR_REG  UINT8_C(0x02)

/*! Host interface control bits */
#define BHY_HIF_CTRL_ABORT_TRANSFER_CHANNEL_0 UINT8_C(0x01)
#define BHY_HIF_CTRL_ABORT_TRANSFER_CHANNEL_1 UINT8_C(0x02)
#define BHY_HIF_CTRL_ABORT_TRANSFER_CHANNEL_2 UINT8_C(0x04)
#define BHY_HIF_CTRL_ABORT_TRANSFER_CHANNEL_3 UINT8_C(0x08)
#define BHY_HIF_CTRL_AP_SUSPENDED             UINT8_C(0x10)
#define BHY_HIF_CTRL_TIMESTAMP_EV_CTRL        UINT8_C(0x40)
#define BHY_HIF_CTRL_ASYNC_STATUS_CHANNEL     UINT8_C(0x80)

/*! Interrupt control bits */
#define BHY_ICTL_DISABLE_FIFO_W      UINT8_C(0x01)
#define BHY_ICTL_DISABLE_FIFO_NW     UINT8_C(0x02)
#define BHY_ICTL_DISABLE_STATUS_FIFO UINT8_C(0x04)
#define BHY_ICTL_DISABLE_DEBUG       UINT8_C(0x08)
#define BHY_ICTL_DISABLE_FAULT       UINT8_C(0x10)
#define BHY_ICTL_ACTIVE_LOW          UINT8_C(0x20)
#define BHY_ICTL_EDGE                UINT8_C(0x40)
#define BHY_ICTL_OPEN_DRAIN          UINT8_C(0x80)

/*! Reset command */
#define BHY_REQUEST_RESET UINT8_C(0x01)

/*! FIFO Format bit */
#define BHY_FIFO_FORMAT_CTRL_MASK         UINT8_C(0x03)
#define BHY_FIFO_FORMAT_CTRL_DIS_DELTA_TS UINT8_C(0x01)
#define BHY_FIFO_FORMAT_CTRL_DIS_FULL_TS  UINT8_C(0x02)
#define BHY_FIFO_FORMAT_CTRL_DIS_ALL_TS   UINT8_C(0x03)

/*! System parameters */
#define BHY_PARAM_READ_MASK               UINT16_C(0x1000)
#define BHY_PARAM_FIFO_CTRL               UINT16_C(0x103)
#define BHY_PARAM_SYS_VIRT_SENSOR_PRESENT UINT16_C(0x11F)
#define BHY_PARAM_SYS_PHYS_SENSOR_PRESENT UINT16_C(0x120)
#define BHY_PARAM_PHYSICAL_SENSOR_BASE    UINT16_C(0x120)

#define BHY_QUERY_PARAM_STATUS_READY_MAX_RETRY UINT16_C(1000)

/*! Meta event definitions */
#define BHY_META_EVENT_FLUSH_COMPLETE        UINT8_C(1)
#define BHY_META_EVENT_SAMPLE_RATE_CHANGED   UINT8_C(2)
#define BHY_META_EVENT_POWER_MODE_CHANGED    UINT8_C(3)
#define BHY_META_EVENT_ALGORITHM_EVENTS      UINT8_C(5)
#define BHY_META_EVENT_SENSOR_STATUS         UINT8_C(6)
#define BHY_META_EVENT_BSX_DO_STEPS_MAIN     UINT8_C(7)
#define BHY_META_EVENT_BSX_DO_STEPS_CALIB    UINT8_C(8)
#define BHY_META_EVENT_BSX_GET_OUTPUT_SIGNAL UINT8_C(9)
#define BHY_META_EVENT_RESERVED1             UINT8_C(10)
#define BHY_META_EVENT_SENSOR_ERROR          UINT8_C(11)
#define BHY_META_EVENT_FIFO_OVERFLOW         UINT8_C(12)
#define BHY_META_EVENT_DYNAMIC_RANGE_CHANGED UINT8_C(13)
#define BHY_META_EVENT_FIFO_WATERMARK        UINT8_C(14)
#define BHY_META_EVENT_RESERVED2             UINT8_C(15)
#define BHY_META_EVENT_INITIALIZED           UINT8_C(16)
#define BHY_META_TRANSFER_CAUSE              UINT8_C(17)
#define BHY_META_EVENT_SENSOR_FRAMEWORK      UINT8_C(18)
#define BHY_META_EVENT_RESET                 UINT8_C(19)
#define BHY_META_EVENT_SPACER                UINT8_C(20)

/* Sensor IDs */
#define BHY_SENSOR_ID_CUSTOM_START UINT8_C(160)
#define BHY_SENSOR_ID_CUSTOM_END   UINT8_C(191)

#define BHY_SENSOR_ID_MAX          UINT8_C(200)
#define BHY_SENSOR_ID_TBD          UINT8_C(BHY_SENSOR_ID_MAX - 1)
#define BHY_PHYSICAL_SENSOR_ID_MAX UINT8_C(64)

/* Virtual Sensor IDs */
#define BHY_SENSOR_ID_ACC_PASS   UINT8_C(1) /* Accelerometer passthrough */
#define BHY_SENSOR_ID_ACC_RAW    UINT8_C(3) /* Accelerometer uncalibrated */
#define BHY_SENSOR_ID_ACC        UINT8_C(4) /* Accelerometer corrected */
#define BHY_SENSOR_ID_ACC_BIAS   UINT8_C(5) /* Accelerometer offset */
#define BHY_SENSOR_ID_ACC_WU     UINT8_C(6) /* Accelerometer corrected wake up */
#define BHY_SENSOR_ID_ACC_RAW_WU UINT8_C(7) /* Accelerometer uncalibrated wake up */
#define BHY_SENSOR_ID_SI_ACCEL                                                       \
    UINT8_C(8)                                /* Virtual Sensor ID for Accelerometer \
                                               * */
#define BHY_SENSOR_ID_GYRO_PASS   UINT8_C(10) /* Gyroscope passthrough */
#define BHY_SENSOR_ID_GYRO_RAW    UINT8_C(12) /* Gyroscope uncalibrated */
#define BHY_SENSOR_ID_GYRO        UINT8_C(13) /* Gyroscope corrected */
#define BHY_SENSOR_ID_GYRO_BIAS   UINT8_C(14) /* Gyroscope offset */
#define BHY_SENSOR_ID_GYRO_WU     UINT8_C(15) /* Gyroscope wake up */
#define BHY_SENSOR_ID_GYRO_RAW_WU UINT8_C(16) /* Gyroscope uncalibrated wake up */
#define BHY_SENSOR_ID_SI_GYROS    UINT8_C(17) /* Virtual Sensor ID for Gyroscope */
#define BHY_SENSOR_ID_MAG_PASS    UINT8_C(19) /* Magnetometer passthrough */
#define BHY_SENSOR_ID_MAG_RAW     UINT8_C(21) /* Magnetometer uncalibrated */
#define BHY_SENSOR_ID_MAG         UINT8_C(22) /* Magnetometer corrected */
#define BHY_SENSOR_ID_MAG_BIAS    UINT8_C(23) /* Magnetometer offset */
#define BHY_SENSOR_ID_MAG_WU      UINT8_C(24) /* Magnetometer wake up */
#define BHY_SENSOR_ID_MAG_RAW_WU  UINT8_C(25) /* Magnetometer uncalibrated wake up */
#define BHY_SENSOR_ID_GRA         UINT8_C(28) /* Gravity vector */
#define BHY_SENSOR_ID_GRA_WU      UINT8_C(29) /* Gravity vector wake up */
#define BHY_SENSOR_ID_LACC        UINT8_C(31) /* Linear acceleration */
#define BHY_SENSOR_ID_LACC_WU     UINT8_C(32) /* Linear acceleration wake up */
#define BHY_SENSOR_ID_RV          UINT8_C(34) /* Rotation vector */
#define BHY_SENSOR_ID_RV_WU       UINT8_C(35) /* Rotation vector wake up */
#define BHY_SENSOR_ID_GAMERV      UINT8_C(37) /* Game rotation vector */
#define BHY_SENSOR_ID_GAMERV_WU   UINT8_C(38) /* Game rotation vector wake up */
#define BHY_SENSOR_ID_GEORV       UINT8_C(40) /* Geo-magnetic rotation vector */
#define BHY_SENSOR_ID_GEORV_WU                                                                \
    UINT8_C(41)                                       /* Geo-magnetic rotation vector wake up \
                                                       * */
#define BHY_SENSOR_ID_ORI                UINT8_C(43)  /* Orientation */
#define BHY_SENSOR_ID_ORI_WU             UINT8_C(44)  /* Orientation wake up */
#define BHY_SENSOR_ID_TILT_DETECTOR      UINT8_C(48)  /* Tilt detector */
#define BHY_SENSOR_ID_STD                UINT8_C(50)  /* Step detector */
#define BHY_SENSOR_ID_STC                UINT8_C(52)  /* Step counter */
#define BHY_SENSOR_ID_STC_WU             UINT8_C(53)  /* Step counter wake up */
#define BHY_SENSOR_ID_SIG                UINT8_C(55)  /* Significant motion */
#define BHY_SENSOR_ID_WAKE_GESTURE       UINT8_C(57)  /* Wake gesture */
#define BHY_SENSOR_ID_GLANCE_GESTURE     UINT8_C(59)  /* Glance gesture */
#define BHY_SENSOR_ID_PICKUP_GESTURE     UINT8_C(61)  /* Pickup gesture */
#define BHY_SENSOR_ID_AR                 UINT8_C(63)  /* Activity recognition */
#define BHY_SENSOR_ID_WRIST_TILT_GESTURE UINT8_C(67)  /* Wrist tilt gesture */
#define BHY_SENSOR_ID_DEVICE_ORI         UINT8_C(69)  /* Device orientation */
#define BHY_SENSOR_ID_DEVICE_ORI_WU      UINT8_C(70)  /* Device orientation wake up */
#define BHY_SENSOR_ID_STATIONARY_DET     UINT8_C(75)  /* Stationary detect */
#define BHY_SENSOR_ID_MOTION_DET         UINT8_C(77)  /* Motion detect */
#define BHY_SENSOR_ID_ACC_BIAS_WU        UINT8_C(91)  /* Accelerometer offset wake up */
#define BHY_SENSOR_ID_GYRO_BIAS_WU       UINT8_C(92)  /* Gyroscope offset wake up */
#define BHY_SENSOR_ID_MAG_BIAS_WU        UINT8_C(93)  /* Magnetometer offset wake up */
#define BHY_SENSOR_ID_STD_WU             UINT8_C(94)  /* Step detector wake up */
#define BHY_SENSOR_ID_ML_1               UINT8_C(95)  /* Machine learning 1 */
#define BHY_SENSOR_ID_ML_2               UINT8_C(96)  /* Machine learning 2 */
#define BHY_SENSOR_ID_ML_3               UINT8_C(97)  /* Machine learning 3 */
#define BHY_SENSOR_ID_AIR_QUALITY        UINT8_C(115) /* Air Quality*/
#define BHY_SENSOR_ID_HEAD_ORI_MIS_ALG   UINT8_C(120) /* Head Orientation Misalignment */
#define BHY_SENSOR_ID_IMU_HEAD_ORI_Q     UINT8_C(121) /* Head Orientation in Quaternion*/
#define BHY_SENSOR_ID_NDOF_HEAD_ORI_Q                                          \
    UINT8_C(122)                                   /* NDOF Head Orientation in \
                                                    * Quaternion*/
#define BHY_SENSOR_ID_IMU_HEAD_ORI_E  UINT8_C(123) /* Head Orientation in Euler*/
#define BHY_SENSOR_ID_NDOF_HEAD_ORI_E UINT8_C(124) /* NDOF Head Orientation in Euler*/
#define BHY_SENSOR_ID_TEMP            UINT8_C(128) /* Temperature */
#define BHY_SENSOR_ID_BARO            UINT8_C(129) /* Barometer */
#define BHY_SENSOR_ID_HUM             UINT8_C(130) /* Humidity */
#define BHY_SENSOR_ID_GAS             UINT8_C(131) /* Gas */
#define BHY_SENSOR_ID_TEMP_WU         UINT8_C(132) /* Temperature wake up */
#define BHY_SENSOR_ID_BARO_WU         UINT8_C(133) /* Barometer wake up */
#define BHY_SENSOR_ID_HUM_WU          UINT8_C(134) /* Humidity wake up */
#define BHY_SENSOR_ID_GAS_WU          UINT8_C(135) /* Gas wake up */
#define BHY_SENSOR_ID_STC_LP          UINT8_C(136) /* Step counter Low Power */
#define BHY_SENSOR_ID_STD_LP          UINT8_C(137) /* Step detector Low Power */
#define BHY_SENSOR_BMP_TEMPERATURE    UINT8_C(138) /* BMP temperature */
#define BHY_SENSOR_ID_STC_LP_WU       UINT8_C(139) /* Step counter Low Power wake up */
#define BHY_SENSOR_ID_STD_LP_WU       UINT8_C(140) /* Step detector Low Power wake up */
#define BHY_SENSOR_ID_SIG_LP_WU                                                          \
    UINT8_C(141)                                    /* Significant motion Low Power wake \
                                                     * up */
#define BHY_SENSOR_BMP_TEMPERATURE_WU  UINT8_C(142) /* BMP temperature wakeup */
#define BHY_SENSOR_ID_ANY_MOTION_LP_WU UINT8_C(143) /* Any motion Low Power wake up */
#define BHY_SENSOR_ID_EXCAMERA         UINT8_C(144) /* External camera trigger */
#define BHY_SENSOR_ID_GPS              UINT8_C(145) /* GPS */
#define BHY_SENSOR_ID_LIGHT            UINT8_C(146) /* Light */
#define BHY_SENSOR_ID_PROX             UINT8_C(147) /* Proximity */
#define BHY_SENSOR_ID_LIGHT_WU         UINT8_C(148) /* Light wake up */
#define BHY_SENSOR_ID_PROX_WU          UINT8_C(149) /* Proximity wake up */

/*! Physical sensor IDs*/
#define BHY_PHYS_SENSOR_ID_ACCELEROMETER        UINT8_C(1)
#define BHY_PHYS_SENSOR_ID_NOT_SUPPORTED        UINT8_C(2)
#define BHY_PHYS_SENSOR_ID_GYROSCOPE            UINT8_C(3)
#define BHY_PHYS_SENSOR_ID_MAGNETOMETER         UINT8_C(5)
#define BHY_PHYS_SENSOR_ID_TEMP_GYRO            UINT8_C(7)
#define BHY_PHYS_SENSOR_ID_ANY_MOTION           UINT8_C(9)
#define BHY_PHYS_SENSOR_ID_PRESSURE             UINT8_C(11)
#define BHY_PHYS_SENSOR_ID_POSITION             UINT8_C(13)
#define BHY_PHYS_SENSOR_ID_HUMIDITY             UINT8_C(15)
#define BHY_PHYS_SENSOR_ID_TEMPERATURE          UINT8_C(17)
#define BHY_PHYS_SENSOR_ID_GAS_RESISTOR         UINT8_C(19)
#define BHY_PHYS_SENSOR_ID_PHYS_STEP_COUNTER    UINT8_C(32)
#define BHY_PHYS_SENSOR_ID_PHYS_STEP_DETECTOR   UINT8_C(33)
#define BHY_PHYS_SENSOR_ID_PHYS_SIGN_MOTION     UINT8_C(34)
#define BHY_PHYS_SENSOR_ID_PHYS_ANY_MOTION      UINT8_C(35)
#define BHY_PHYS_SENSOR_ID_EX_CAMERA_INPUT      UINT8_C(36)
#define BHY_PHYS_SENSOR_ID_GPS                  UINT8_C(48)
#define BHY_PHYS_SENSOR_ID_LIGHT                UINT8_C(49)
#define BHY_PHYS_SENSOR_ID_PROXIMITY            UINT8_C(50)
#define BHY_PHYS_SENSOR_ID_ACT_REC              UINT8_C(52)
#define BHY_PHYS_SENSOR_ID_PHYS_NO_MOTION       UINT8_C(55)
#define BHY_PHYS_SENSOR_ID_WRIST_GESTURE_DETECT UINT8_C(56)
#define BHY_PHYS_SENSOR_ID_WRIST_WEAR_WAKEUP    UINT8_C(57)

/*! System data IDs */
#define BHY_IS_SYS_ID(sid) ((sid) >= 224)

#define BHY_SYS_ID_PADDING            UINT8_C(0)
#define BHY_SYS_ID_TS_SMALL_DELTA     UINT8_C(251)
#define BHY_SYS_ID_TS_LARGE_DELTA     UINT8_C(252)
#define BHY_SYS_ID_TS_FULL            UINT8_C(253)
#define BHY_SYS_ID_META_EVENT         UINT8_C(254)
#define BHY_SYS_ID_TS_SMALL_DELTA_WU  UINT8_C(245)
#define BHY_SYS_ID_TS_LARGE_DELTA_WU  UINT8_C(246)
#define BHY_SYS_ID_TS_FULL_WU         UINT8_C(247)
#define BHY_SYS_ID_META_EVENT_WU      UINT8_C(248)
#define BHY_SYS_ID_FILLER             UINT8_C(255)
#define BHY_SYS_ID_DEBUG_MSG          UINT8_C(250)
#define BHY_SYS_ID_BHY_LOG_UPDATE_SUB UINT8_C(243)
#define BHY_SYS_ID_BHY_LOG_DOSTEP     UINT8_C(244)

/*! Status code definitions */
#define BHY_STATUS_INITIALIZED            UINT8_C(0x1)
#define BHY_STATUS_DEBUG_OUTPUT           UINT8_C(0x2)
#define BHY_STATUS_CRASH_DUMP             UINT8_C(0x3)
#define BHY_STATUS_INJECT_SENSOR_CONF_REQ UINT8_C(0x4)
#define BHY_STATUS_SW_PASS_THRU_RES       UINT8_C(0x5)
#define BHY_STATUS_SELF_TEST_RES          UINT8_C(0x6)
#define BHY_STATUS_FOC_RES                UINT8_C(0x7)
#define BHY_STATUS_SYSTEM_ERROR           UINT8_C(0x8)
#define BHY_STATUS_SENSOR_ERROR           UINT8_C(0x9)
#define BHY_STATUS_HOST_EV_TIMESTAMP      UINT8_C(0xD)
#define BHY_STATUS_DUT_TEST_RES           UINT8_C(0xE)
#define BHY_STATUS_CMD_ERR                UINT8_C(0xF)

#define BHY_IS_STATUS_GET_PARAM_OUTPUT(status) ((status) >= 0x100 && (status) <= 0xFFF)

/*! Activity bits */
#define BHY_STILL_ACTIVITY_ENDED      (0x0001)
#define BHY_WALKING_ACTIVITY_ENDED    (0x0002)
#define BHY_RUNNING_ACTIVITY_ENDED    (0x0004)
#define BHY_ON_BICYCLE_ACTIVITY_ENDED (0x0008)
#define BHY_IN_VEHICLE_ACTIVITY_ENDED (0x0010)
#define BHY_TILTING_ACTIVITY_ENDED    (0x0020)

#define BHY_STILL_ACTIVITY_STARTED      (0x0100)
#define BHY_WALKING_ACTIVITY_STARTED    (0x0200)
#define BHY_RUNNING_ACTIVITY_STARTED    (0x0400)
#define BHY_ON_BICYCLE_ACTIVITY_STARTED (0x0800)
#define BHY_IN_VEHICLE_ACTIVITY_STARTED (0x1000)
#define BHY_TILTING_ACTIVITY_STARTED    (0x2000)

/*! Feature status */
#define BHY_FEAT_STATUS_OPEN_RTOS_MSK UINT8_C(0x02)
#define BHY_FEAT_STATUS_OPEN_RTOS_POS (1)
#define BHY_FEAT_STATUS_HOST_ID_MSK   UINT8_C(0x1C)
#define BHY_FEAT_STATUS_HOST_ID_POS   (2)
#define BHY_FEAT_STATUS_ALGO_ID_MSK   UINT8_C(0xE0)
#define BHY_FEAT_STATUS_ALGO_ID_POS   (5)

/*! Fast offset compensation status codes */
#define BHY_FOC_PASS            UINT8_C(0x00)
#define BHY_FOC_FAILED          UINT8_C(0x65)
#define BHY_FOC_UNKNOWN_FAILURE UINT8_C(0x23)

/*! Self test status codes */
#define BHY_ST_PASSED             UINT8_C(0x00)
#define BHY_ST_X_AXIS_FAILED      UINT8_C(0x01)
#define BHY_ST_Y_AXIS_FAILED      UINT8_C(0x02)
#define BHY_ST_Z_AXIS_FAILED      UINT8_C(0x04)
#define BHY_ST_MULTI_AXIS_FAILURE UINT8_C(0x07)
#define BHY_ST_NOT_SUPPORTED      UINT8_C(0x08)
#define BHY_ST_INVALID_PHYS_ID    UINT8_C(0x09)

/*! Gyroscope dynamic ranges */
#define BHY_GYRO_125DPS  UINT8_C(0x7D)
#define BHY_GYRO_250DPS  UINT8_C(0xFA)
#define BHY_GYRO_500DPS  UINT16_C(0x1F4)
#define BHY_GYRO_1000DPS UINT16_C(0x3E8)
#define BHY_GYRO_2000DPS UINT16_C(0x7D0)

/*! Accelerometer dynamic ranges */
#define BHY_ACCEL_2G  UINT8_C(0x2)
#define BHY_ACCEL_4G  UINT8_C(0x4)
#define BHY_ACCEL_8G  UINT8_C(0x8)
#define BHY_ACCEL_16G UINT8_C(0x10)

/*! Magnetometer dynamic ranges */
#define BHY_MAG_1300MICROTESLA UINT16_C(0x514)
#define BHY_MAG_2500MICROTESLA UINT16_C(0x9C4)

#define BHY_LE2U16(x) ((uint16_t)((x)[0] | (x)[1] << 8))
#define BHY_LE2S16(x) ((int16_t)BHY_LE2U16(x))
#define BHY_LE2U24(x) ((uint32_t)((x)[0] | (uint32_t)(x)[1] << 8 | (uint32_t)(x)[2] << 16))
#define BHY_LE2S24(x) ((int32_t)(BHY_LE2U24(x) << 8) >> 8)
#define BHY_LE2U32(x) \
    ((uint32_t)((x)[0] | (uint32_t)(x)[1] << 8 | (uint32_t)(x)[2] << 16 | (uint32_t)(x)[3] << 24))
#define BHY_LE2S32(x) ((int32_t)BHY_LE2U32(x))
#define BHY_LE2U40(x) (BHY_LE2U32(x) | (uint64_t)(x)[4] << 32)
#define BHY_LE2U48(x) ((BHY_LE2U32(x) | (uint64_t)(x)[4] << 32 | (uint64_t)(x)[5] << 40))
#define BHY_LE2U64(x) (BHY_LE2U32(x) | (uint64_t)BHY_LE2U32(&(x)[4]) << 32)

/*! Error register values */
#define BHY_ERR_NO_ERROR                                           UINT8_C(0x00)
#define BHY_ERR_FW_EXPECTED_VERSION_MISMATCH                       UINT8_C(0x10)
#define BHY_ERR_FW_BAD_HEADER_CRC                                  UINT8_C(0x11)
#define BHY_ERR_FW_SHA_HASH_MISMATCH                               UINT8_C(0x12)
#define BHY_ERR_FW_BAD_IMAGE_CRC                                   UINT8_C(0x13)
#define BHY_ERR_FW_ECDSA_SIGNATURE_VERIFICATION_FAILED             UINT8_C(0x14)
#define BHY_ERR_FW_BAD_PUBLIC_KEY_CRC                              UINT8_C(0x15)
#define BHY_ERR_FW_SIGNED_FW_REQUIRED                              UINT8_C(0x16)
#define BHY_ERR_FW_HEADER_MISSING                                  UINT8_C(0x17)
#define BHY_ERR_UNEXPECTED_WATCHDOG_RESET                          UINT8_C(0x19)
#define BHY_ERR_ROM_VERSION_MISMATCH                               UINT8_C(0x1A)
#define BHY_ERR_FATAL_FIRMWARE_ERROR                               UINT8_C(0x1B)
#define BHY_ERR_NEXT_PAYLOAD_NOT_FOUND                             UINT8_C(0x1C)
#define BHY_ERR_PAYLOAD_NOT_VALID                                  UINT8_C(0x1D)
#define BHY_ERR_PAYLOAD_ENTRIES_INVALID                            UINT8_C(0x1E)
#define BHY_ERR_OTP_CRC_INVALID                                    UINT8_C(0x1F)
#define BHY_ERR_FIRMWARE_INIT_FAILED                               UINT8_C(0x20)
#define BHY_ERR_UNEXPECTED_DEVICE_ID                               UINT8_C(0x21)
#define BHY_ERR_NO_RESPONSE_FROM_DEVICE                            UINT8_C(0x22)
#define BHY_ERR_SENSOR_INIT_FAILED_UNKNOWN                         UINT8_C(0x23)
#define BHY_ERR_SENSOR_ERROR_NO_VALID_DATA                         UINT8_C(0x24)
#define BHY_ERR_SLOW_SAMPLE_RATE                                   UINT8_C(0x25)
#define BHY_ERR_DATA_OVERFLOW                                      UINT8_C(0x26)
#define BHY_ERR_STACK_OVERFLOW                                     UINT8_C(0x27)
#define BHY_ERR_INSUFFICIENT_FREE_RAM                              UINT8_C(0x28)
#define BHY_ERR_DRIVER_PARSING_ERROR                               UINT8_C(0x29)
#define BHY_ERR_RAM_BANKS                                          UINT8_C(0x2A)
#define BHY_ERR_INVALID_EVENT                                      UINT8_C(0x2B)
#define BHY_ERR_MORE_THAN_32_ON_CHANGE                             UINT8_C(0x2C)
#define BHY_ERR_FIRMWARE_TOO_LARGE                                 UINT8_C(0x2D)
#define BHY_ERR_INVALID_RAM_BANKS                                  UINT8_C(0x2F)
#define BHY_ERR_MATH_ERROR                                         UINT8_C(0x30)
#define BHY_ERR_MEMORRY_ERROR                                      UINT8_C(0x40)
#define BHY_ERR_SWI3_ERROR                                         UINT8_C(0x41)
#define BHY_ERR_SWI4_ERROR                                         UINT8_C(0x42)
#define BHY_ERR_ILLEGAL_INSTRUCTION_ERROR                          UINT8_C(0x43)
#define BHY_ERR_UNHANDLED_INTERRUPT_EXCEPTION_POSTMORTEM_AVAILABLE UINT8_C(0x44)
#define BHY_ERR_INVALID_MEMORY_ACCESS                              UINT8_C(0x45)
#define BHY_ERR_ALGO_BSX_INIT                                      UINT8_C(0x50)
#define BHY_ERR_ALGO_BSX_DO_STEP                                   UINT8_C(0x51)
#define BHY_ERR_ALGO_UPDATE_SUB                                    UINT8_C(0x52)
#define BHY_ERR_ALGO_GET_SUB                                       UINT8_C(0x53)
#define BHY_ERR_ALGO_GET_PHYS                                      UINT8_C(0x54)
#define BHY_ERR_ALGO_UNSUPPORTED_PHYS_RATE                         UINT8_C(0x55)
#define BHY_ERR_ALGO_BSX_DRIVER_NOT_FOUND                          UINT8_C(0x56)
#define BHY_ERR_SENSOR_SELF_TEST_FAILURE                           UINT8_C(0x60)
#define BHY_ERR_SENSOR_SELF_TEST_X_AXIS_FAILURE                    UINT8_C(0x61)
#define BHY_ERR_SENSOR_SELF_TEST_Y_AXIS_FAILURE                    UINT8_C(0x62)
#define BHY_ERR_SENSOR_SELF_TEST_Z_AXIS_FAILURE                    UINT8_C(0x64)
#define BHY_ERR_SENSOR_FOC_FAILURE                                 UINT8_C(0x65)
#define BHY_ERR_SENSOR_BUSY                                        UINT8_C(0x66)
#define BHY_ERR_SELF_TEST_FOC_UNSUPPORTED                          UINT8_C(0x6F)
#define BHY_ERR_NO_HOST_INTERRUPT_SET                              UINT8_C(0x72)
#define BHY_ERR_EVENT_ID_SIZE                                      UINT8_C(0x73)
#define BHY_ERR_HOST_DOWNLOAD_CHANNEL_UNDERFLOW                    UINT8_C(0x75)
#define BHY_ERR_HOST_UPLOAD_CHANNEL_OVERFLOW                       UINT8_C(0x76)
#define BHY_ERR_HOST_DOWNLOAD_CHANNEL_EMPTY                        UINT8_C(0x77)
#define BHY_ERR_DMA_ERROR                                          UINT8_C(0x78)
#define BHY_ERR_CORRUPTED_INPUT_BLOCK_CHAIN                        UINT8_C(0x79)
#define BHY_ERR_CORRUPTED_OUTPUT_BLOCK_CHAIN                       UINT8_C(0x7A)
#define BHY_ERR_BUFFER_BLOCK_MANAGER                               UINT8_C(0x7B)
#define BHY_ERR_INPUT_CHANNEL_NOT_WORD_ALIGNED                     UINT8_C(0x7C)
#define BHY_ERR_TOO_MANY_FLUSH_EVENTS                              UINT8_C(0x7D)
#define BHY_ERR_UNKNOWN_HOST_CHANNEL_ERROR                         UINT8_C(0x7E)
#define BHY_ERR_DECIMATION_TOO_LARGE                               UINT8_C(0x81)
#define BHY_ERR_MASTER_SPI_I2C_QUEUE_OVERFLOW                      UINT8_C(0x90)
#define BHY_ERR_SPI_I2C_CALLBACK_ERROR                             UINT8_C(0x91)
#define BHY_ERR_TIMER_SCHEDULING_ERROR                             UINT8_C(0xA0)
#define BHY_ERR_INVALID_GPIO_FOR_HOST_IRQ                          UINT8_C(0xB0)
#define BHY_ERR_SENDING_INITIALIZED_META_EVENTS                    UINT8_C(0xB1)
#define BHY_ERR_CMD_ERROR                                          UINT8_C(0xC0)
#define BHY_ERR_CMD_TOO_LONG                                       UINT8_C(0xC1)
#define BHY_ERR_CMD_BUFFER_OVERFLOW                                UINT8_C(0xC2)
#define BHY_ERR_SYS_CALL_INVALID                                   UINT8_C(0xD0)
#define BHY_ERR_TRAP_INVALID                                       UINT8_C(0xD2)
#define BHY_ERR_FW_HEADER_CORRUPT                                  UINT8_C(0xE0)
#define BHY_ERR_SENSOR_DATA_INJECTION                              UINT8_C(0xE2)

#define BHY_LE24MUL(x) (((x) % 4) ? (uint16_t)((((x) / 4) + 1) * 4) : (uint16_t)((x) + 4))

/*! Maximum no of available virtual sensor */
#define BHY_N_VIRTUAL_SENSOR_MAX UINT8_C(256)

#ifndef BHY_MAX_SIMUL_SENSORS
#define BHY_MAX_SIMUL_SENSORS 48
#endif

/* Special & debug virtual sensor id starts at 245 */
#define BHY_SPECIAL_SENSOR_ID_OFFSET UINT8_C(245)

#ifndef BHY_INTF_RET_TYPE
#define BHY_INTF_RET_TYPE int8_t
#endif

#ifndef BHY_INTF_RET_SUCCESS
#define BHY_INTF_RET_SUCCESS 0
#endif

/* Macros to replace the constants */
#define BHY_TS_SMALL_DELTA_FIFO_RD_SIZE UINT8_C(2)
#define BHY_TS_LARGE_DELTA_RD_FIFO_SIZE UINT8_C(3)
#define BHY_TS_FULL_RD_FIFO_SIZE        UINT8_C(6)
#define BHY_LOG_DOSTEP_RD_FIFO_SIZE     UINT8_C(23)
#define BHY_FOC_STATUS_RD_FIFO_SIZE     UINT8_C(12)

#define BHY_ACCEL_FOC UINT8_C(1)
#define BHY_GYRO_FOC  UINT8_C(3)

/*FOC status values*/
#define BHY_FOC_SUCCESS       UINT8_C(0x00)
#define BHY_FOC_FAILURE       UINT8_C(0x65)
#define BHY_FOC_UNKNOWN_ERROR UINT8_C(0x23)

    /* Type definitions for the function pointers */
    typedef BHY_INTF_RET_TYPE (*bhy_read_fptr_t)(uint8_t  reg_addr,
                                                 uint8_t *reg_data,
                                                 uint32_t length,
                                                 void    *intf_ptr);
    typedef BHY_INTF_RET_TYPE (*bhy_write_fptr_t)(uint8_t        reg_addr,
                                                  const uint8_t *reg_data,
                                                  uint32_t       length,
                                                  void          *intf_ptr);
    typedef void (*bhy_delay_us_fptr_t)(uint32_t period_us, void *intf_ptr);

    enum bhy_intf
    {
        BHY_SPI_INTERFACE = 1,
        BHY_I2C_INTERFACE
    };

    /* HIF device structure */
    struct bhy_hif_dev
    {
        bhy_read_fptr_t     read;
        bhy_write_fptr_t    write;
        bhy_delay_us_fptr_t delay_us;
        enum bhy_intf       intf;
        void               *intf_ptr;
        BHY_INTF_RET_TYPE   intf_rslt;
        uint32_t            read_write_len;
    };

    enum bhy_fifo_type
    {
        BHY_FIFO_TYPE_WAKEUP,
        BHY_FIFO_TYPE_NON_WAKEUP,
        BHY_FIFO_TYPE_STATUS,
        BHY_FIFO_TYPE_MAX
    };

    struct BHY_PACKED bhy_fifo_parse_data_info
    {
        uint8_t            sensor_id;
        enum bhy_fifo_type fifo_type;
        uint8_t            data_size;
        uint8_t           *data_ptr;
        uint64_t          *time_stamp;
    };

    typedef void (*bhy_fifo_parse_callback_t)(const struct bhy_fifo_parse_data_info *callback_info,
                                              void                                  *private_data);

    struct BHY_PACKED bhy_fifo_parse_callback_table
    {
        uint8_t                   sensor_id;
        bhy_fifo_parse_callback_t callback;
        void                     *callback_ref;
    };

    /* Device structure */
    struct bhy_dev
    {
        struct bhy_hif_dev                   hif;
        struct bhy_fifo_parse_callback_table table[BHY_MAX_SIMUL_SENSORS];
        uint8_t                              event_size[BHY_N_VIRTUAL_SENSOR_MAX];
        uint64_t                             last_time_stamp[BHY_FIFO_TYPE_MAX];
        uint8_t                              present_buff[32];
        uint8_t                              phy_present_buff[8];
    };

    struct bhy_fifo_buffer
    {
        uint32_t read_pos;
        uint32_t read_length;
        uint32_t remain_length;
        uint32_t buffer_size;
        uint8_t *buffer;
    };

    typedef int16_t (*bhy_frame_parse_func_t)(struct bhy_fifo_buffer *p_fifo_buffer,
                                              struct bhy_dev         *bhy_p);

    struct bhy_virt_sensor_conf
    {
        uint16_t  sensitivity;
        uint16_t  range;
        uint32_t  latency;
        bhy_float sample_rate;
    };

    union BHY_PACKED bhy_spt_dev_id
    {
        uint8_t slave_address;
        uint8_t cs_pin;
    };

    struct BHY_PACKED bhy_spt_bits
    {
        /*! byte 1 */
        uint8_t direction : 1;  /**< 0: read; 1: write. */
        uint8_t trans_type : 1; /**< 0: single burst; 1:multi single transfers. */
        uint8_t delay_ctrl : 1; /**< 0: none; 1: delay between bytes. */
        uint8_t master_bus : 2; /**< 1: SIF1; 2: SIF2; 3:SIF3. */
        uint8_t spi_mode : 1;   /**< 0: 4 wire; 1: 3 wire. */
        uint8_t cpol : 1;       /**< spi clock polarity. */
        uint8_t cpha : 1;       /**< spi clock phase. */
        /*! byte 2 */
        uint8_t delay_val : 6; /**< multiples of 50us. min is 200us (4LSB) */
        uint8_t cs_level : 1;  /**< chip select level. */
        uint8_t lsb_first : 1; /**< least significant byte first. */
        /*! byte 3~4 */
        uint16_t trans_rate; /**< spi clock rate. */
        /*! byte 5 */
        uint8_t address_shift : 4; /**< number of bits to shift register address. */
        uint8_t read_bit_pol : 1;  /**< 0: active low; 1: active high. */
        uint8_t read_bit_pos : 3;  /**< bit number of read bit in command byte. */
        /*! byte 6 */
        union bhy_spt_dev_id func_set;

        /*! byte 7 */
        uint8_t trans_count;

        /*! byte 8 */
        uint8_t reg;
    };

    union bhy_soft_passthrough_conf
    {
        struct bhy_spt_bits conf;
        uint8_t             data[8];
    };

    union bhy_u16_conv
    {
        uint16_t u16_val;
        uint8_t  bytes[2];
    };
    union bhy_u32_conv
    {
        uint32_t u32_val;
        uint8_t  bytes[4];
    };
    union bhy_float_conv
    {
        bhy_float f_val;
        uint32_t  u32_val;
        uint8_t   bytes[4];
    };

    struct bhy_self_test_resp
    {
        uint8_t test_status;
        int16_t x_offset, y_offset, z_offset;
    };

    struct bhy_foc_resp
    {
        uint8_t foc_status;
        int16_t x_offset, y_offset, z_offset;
    };

    enum bhy_data_inj_mode
    {
        BHY_NORMAL_MODE            = 0,
        BHY_REAL_TIME_INJECTION    = 1,
        BHY_STEP_BY_STEP_INJECTION = 2
    };

#define BHY_BYTE_TO_NIBBLE(X) (((uint8_t)(X)[0] & 0x0F) | (((uint8_t)(X)[1] << 4) & 0xF0))

/* End of CPP Guard */
#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __BHY_DEFS_H__ */
