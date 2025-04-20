#ifndef MANIKIN_TYPES_H
#define MANIKIN_TYPES_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stddef.h>
#include "manikin_software_conf.h"

    typedef enum
    {
        MANIKIN_STATUS_OK = 0,
        MANIKIN_STATUS_ERR_NULL_PARAM,
        MANIKIN_STATUS_ERR_INVALID_I2C_BAUD,
        MANIKIN_STATUS_ERR_INVALID_TIMER_SAMPLE_RATE,
        MANIKIN_STATUS_ERR_INVALID_BUFFER_SIZE,
        MANIKIN_STATUS_ERR_WRITE_FAIL,
        MANIKIN_STATUS_ERR_READ_FAIL,
        MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL,
        MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL,
        MANIKIN_STATUS_ERR_PERIPHERAL_INIT_FAIL,
        MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG,
        MANIKIN_STATUS_ERR_INVALID_SPI_BAUD,
    } manikin_status_t;

    typedef enum
    {
        MANIKIN_MEMORY_RESULT_OK = 0, /* 0: Successful */
        MANIKIN_MEMORY_RESULT_ERROR,  /* 1: R/W Error */
        MANIKIN_MEMORY_RESULT_WRPRT,  /* 2: Write Protected */
        MANIKIN_MEMORY_RESULT_NOTRDY, /* 3: Not Ready */
        MANIKIN_MEMORY_RESULT_PARERR  /* 4: Invalid Parameter */
    } manikin_memory_result_t;

    typedef enum
    {
        MANIKIN_MEMORY_STATE_OK     = 0,
        MANIKIN_MEMORY_STATE_NOINIT = 1,
        MANIKIN_MEMORY_STATE_NOMEM  = 2,
        MANIKIN_MEMORY_STATE_WP     = 4,
    } manikin_memory_status_t;

    typedef enum
    {
        MANIKIN_MEMORY_IOCTL_SYNC             = 0,
        MANIKIN_MEMORY_IOCTL_GET_SECTOR_COUNT = 1,
        MANIKIN_MEMORY_IOCTL_GET_SECTOR_SIZE  = 2,
        MANIKIN_MEMORY_IOCTL_GET_BLOCK_SIZE   = 3,
        MANIKIN_MEMORY_IOCTL_TRIM             = 4,
        MANIKIN_MEMORY_IOCTL_POWER            = 5,
        MANIKIN_MEMORY_IOCTL_LOCK             = 6,
        MANIKIN_MEMORY_IOCTL_EJECT            = 7,
        MANIKIN_MEMORY_IOCTL_FORMAT           = 8,
        MANIKIN_MEMORY_IOCTL_GET_TYPE         = 10,
        MANIKIN_MEMORY_IOCTL_GET_CSD          = 11,
        MANIKIN_MEMORY_IOCTL_GET_CID          = 12,
        MANIKIN_MEMORY_IOCTL_GET_OCR          = 13,
        MANIKIN_MEMORY_IOCTL_GET_SDSTAT       = 14,
        MANIKIN_MEMORY_IOCTL_ISDIO_READ       = 55,
        MANIKIN_MEMORY_IOCTL_ISDIO_WRITE      = 56,
        MANIKIN_MEMORY_IOCTL_ISDIO_MRITE      = 57,
        MANIKIN_MEMORY_IOCTL_ATA_GET_REV      = 20,
        MANIKIN_MEMORY_IOCTL_ATA_GET_MODEL    = 21,
        MANIKIN_MEMORY_IOCTL_ATA_GET_SN       = 22
    } manikin_memory_ioctl_t;

    typedef enum
    {
        MANIKIN_I2C_SPEED_NONE   = 0,
        MANIKIN_I2C_SPEED_100KHz = 100000,
        MANIKIN_I2C_SPEED_400KHz = 400000,
        MANIKIN_I2C_SPEED_1MHz   = 1000000,
    } manikin_i2c_speed_t;

    typedef struct
    {
        manikin_i2c_inst_t i2c;
        uint8_t            i2c_addr;
        uint8_t            needs_reinit;
    } manikin_sensor_ctx_t;

    typedef struct
    {
        manikin_spi_inst_t spi;
        manikin_spi_cs_t   spi_cs;
        size_t             mem_size;
    } manikin_spi_memory_ctx_t;

    typedef enum
    {
        MANIKIN_SENSOR_REG_TYPE_WRITE = 0,
        MANIKIN_SENSOR_REG_TYPE_READ  = 1,
    } manikin_sensor_reg_type_t;

    typedef struct
    {
        uint16_t                  reg;
        uint16_t                  val;
        manikin_sensor_reg_type_t opcode;
    } manikin_sensor_reg_t;

    typedef struct
    {
        const manikin_sensor_reg_t *reg;
        uint8_t                     len;
    } manikin_sensor_reg_seq_t;

#ifdef __cplusplus
}
#endif
#endif /* MANIKIN_TYPES_H */