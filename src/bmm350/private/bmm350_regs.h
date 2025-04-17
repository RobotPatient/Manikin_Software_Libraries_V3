#ifndef BMM350_REGS_H
#define BMM350_REGS_H
#ifdef __cplusplus
extern "C"
{
#endif

#define BMM350_SOFT_RESET_DELAY                    24000
#define BMM350_MAGNETIC_RESET_DELAY                40000
#define BMM350_START_UP_TIME_FROM_POR              3000

#define BMM350_GOTO_SUSPEND_DELAY                  6000
#define BMM350_SUSPEND_TO_NORMAL_DELAY             38000

#define BMM350_SUS_TO_FORCEDMODE_NO_AVG_DELAY      15000
#define BMM350_SUS_TO_FORCEDMODE_AVG_2_DELAY       17000
#define BMM350_SUS_TO_FORCEDMODE_AVG_4_DELAY       20000
#define BMM350_SUS_TO_FORCEDMODE_AVG_8_DELAY       28000

#define BMM350_SUS_TO_FORCEDMODE_FAST_NO_AVG_DELAY 4000
#define BMM350_SUS_TO_FORCEDMODE_FAST_AVG_2_DELAY  5000
#define BMM350_SUS_TO_FORCEDMODE_FAST_AVG_4_DELAY  9000
#define BMM350_SUS_TO_FORCEDMODE_FAST_AVG_8_DELAY  16000

#define BMM350_UPD_OAE_DELAY                        1000

#define BMM350_BR_DELAY                             14000
#define BMM350_FGR_DELAY                            18000

/************************ Length macros ************************/
#define BMM350_OTP_DATA_LENGTH                      32
#define BMM350_READ_BUFFER_LENGTH                   127
#define BMM350_MAG_TEMP_DATA_LEN                    12

/************************ Averaging macros **********************/
#define BMM350_AVG_NO_AVG                           0x0
#define BMM350_AVG_2                                0x1
#define BMM350_AVG_4                                0x2
#define BMM350_AVG_8                                0x3

/******************************* ODR **************************/
#define BMM350_ODR_400HZ                            0x2
#define BMM350_ODR_200HZ                            0x3
#define BMM350_ODR_100HZ                            0x4
#define BMM350_ODR_50HZ                             0x5
#define BMM350_ODR_25HZ                             0x6
#define BMM350_ODR_12_5HZ                           0x7
#define BMM350_ODR_6_25HZ                           0x8
#define BMM350_ODR_3_125HZ                          0x9
#define BMM350_ODR_1_5625HZ                         0xA

/********************* Power modes *************************/
#define BMM350_PMU_CMD_SUS                          0x00
#define BMM350_PMU_CMD_NM                           0x01
#define BMM350_PMU_CMD_UPD_OAE                      0x02
#define BMM350_PMU_CMD_FM                           0x03
#define BMM350_PMU_CMD_FM_FAST                      0x04
#define BMM350_PMU_CMD_FGR                          0x05
#define BMM350_PMU_CMD_FGR_FAST                     0x06
#define BMM350_PMU_CMD_BR                           0x07
#define BMM350_PMU_CMD_BR_FAST                      0x08
#define BMM350_PMU_CMD_NM_TC                        0x09

#define BMM350_PMU_STATUS_0                         0x0

#define BMM350_DISABLE                              0x0
#define BMM350_ENABLE                               0x1

#define BMM350_CMD_NOP                              0x0
#define BMM350_CMD_SOFTRESET                        0xB6

#define BMM350_TARGET_PAGE_PAGE0                    0x0
#define BMM350_TARGET_PAGE_PAGE1                    0x1

#define BMM350_INT_MODE_LATCHED                     0x1
#define BMM350_INT_MODE_PULSED                      0x0

#define BMM350_INT_POL_ACTIVE_HIGH                  0x1
#define BMM350_INT_POL_ACTIVE_LOW                   0x0

#define BMM350_INT_OD_PUSHPULL                      0x1
#define BMM350_INT_OD_OPENDRAIN                     0x0

#define BMM350_INT_OUTPUT_EN_OFF                    0x0
#define BMM350_INT_OUTPUT_EN_ON                     0x1

#define BMM350_INT_DRDY_EN                          0x1
#define BMM350_INT_DRDY_DIS                         0x0

#define BMM350_MR_MR1K8                             0x0
#define BMM350_MR_MR2K1                             0x1
#define BMM350_MR_MR1K5                             0x2
#define BMM350_MR_MR0K6                             0x3

#define BMM350_SEL_DTB1X_PAD_PAD_INT                0x0
#define BMM350_SEL_DTB1X_PAD_PAD_BYP                0x1

#define BMM350_TMR_TST_HIZ_VTMR_VTMR_ON             0x0
#define BMM350_TMR_TST_HIZ_VTMR_VTMR_HIZ            0x1

#define BMM350_LSB_MASK                             0x00FF
#define BMM350_MSB_MASK                             0xFF00

/********************** Pad drive strength ************************/
#define BMM350_PAD_DRIVE_WEAKEST                    0
#define BMM350_PAD_DRIVE_STRONGEST                  7

/********************** I2C Register Addresses ************************/

/*! Register to set I2C address to LOW */
#define BMM350_I2C_ADSEL_SET_LOW                    0x14

/*! Register to set I2C address to HIGH */
#define BMM350_I2C_ADSEL_SET_HIGH                   0x15

#define BMM350_DUMMY_BYTES                          2

/********************** Register Addresses ************************/

#define BMM350_REG_CHIP_ID                          0x00
#define BMM350_REG_REV_ID                           0x01
#define BMM350_REG_ERR_REG                          0x02
#define BMM350_REG_PAD_CTRL                         0x03
#define BMM350_REG_PMU_CMD_AGGR_SET                 0x04
#define BMM350_REG_PMU_CMD_AXIS_EN                  0x05
#define BMM350_REG_PMU_CMD                          0x06
#define BMM350_REG_PMU_CMD_STATUS_0                 0x07
#define BMM350_REG_PMU_CMD_STATUS_1                 0x08
#define BMM350_REG_I3C_ERR                          0x09
#define BMM350_REG_I2C_WDT_SET                      0x0A
#define BMM350_REG_TRSDCR_REV_ID                    0x0D
#define BMM350_REG_TC_SYNC_TU                       0x21
#define BMM350_REG_TC_SYNC_ODR                      0x22
#define BMM350_REG_TC_SYNC_TPH_1                    0x23
#define BMM350_REG_TC_SYNC_TPH_2                    0x24
#define BMM350_REG_TC_SYNC_DT                       0x25
#define BMM350_REG_TC_SYNC_ST_0                     0x26
#define BMM350_REG_TC_SYNC_ST_1                     0x27
#define BMM350_REG_TC_SYNC_ST_2                     0x28
#define BMM350_REG_TC_SYNC_STATUS                   0x29
#define BMM350_REG_INT_CTRL                         0x2E
#define BMM350_REG_INT_CTRL_IBI                     0x2F
#define BMM350_REG_INT_STATUS                       0x30
#define BMM350_REG_MAG_X_XLSB                       0x31
#define BMM350_REG_MAG_X_LSB                        0x32
#define BMM350_REG_MAG_X_MSB                        0x33
#define BMM350_REG_MAG_Y_XLSB                       0x34
#define BMM350_REG_MAG_Y_LSB                        0x35
#define BMM350_REG_MAG_Y_MSB                        0x36
#define BMM350_REG_MAG_Z_XLSB                       0x37
#define BMM350_REG_MAG_Z_LSB                        0x38
#define BMM350_REG_MAG_Z_MSB                        0x39
#define BMM350_REG_TEMP_XLSB                        0x3A
#define BMM350_REG_TEMP_LSB                         0x3B
#define BMM350_REG_TEMP_MSB                         0x3C
#define BMM350_REG_SENSORTIME_XLSB                  0x3D
#define BMM350_REG_SENSORTIME_LSB                   0x3E
#define BMM350_REG_SENSORTIME_MSB                   0x3F
#define BMM350_REG_OTP_CMD_REG                      0x50
#define BMM350_REG_OTP_DATA_MSB_REG                 0x52
#define BMM350_REG_OTP_DATA_LSB_REG                 0x53
#define BMM350_REG_OTP_STATUS_REG                   0x55
#define BMM350_REG_TMR_SELFTEST_USER                0x60
#define BMM350_REG_CTRL_USER                        0x61
#define BMM350_REG_CMD                              0x7E


#ifdef __cplusplus
}
#endif
#endif /* BMM350_REGS_H */
