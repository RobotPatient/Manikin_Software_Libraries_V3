/**
 * @file            bhi360.c
 * @brief           Driver implementation for the BHI360 programmable IMU
 *
 * @copyright       Copyright 2025 (C) RobotPatient Simulators
 * @license         Licensed under the Apache License, Version 2.0
 *                  http://www.apache.org/licenses/LICENSE-2.0
 *
 * This file is part of the Manikin Software Libraries V3 project
 * Author:          Victor Hogeweij
 */

#include <bhi360_fusion/bhi360_fusion.h>
#include "i2c/i2c.h"
#include "error_handler/error_handler.h"
#include "bhy.h"
#include "bhy_virtual_sensor_conf_param.h"
#include "bhy_event_data.h"
#include "manikin_platform.h"
#include "external/bhy.h"
#include "external/bhy_defs.h"
#include "external/bhi3_defs.h"
#include "firmware/BHI360.fw.h"

#include <inttypes.h>
#ifdef DEBUG
#include <stdio.h>
#define LOG_DEBUG(...) printf(__VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

#define HASH_BHI360      0xC3A556E3u
#define WORK_BUFFER_SIZE 2048
#define QUAT_SENSOR_ID   BHY_SENSOR_ID_RV
#define BHY_RD_WR_LEN    256
// Constants for orientation scaling
#define ORIENTATION_SCALE_FACTOR (360.0f / 32768.0f)

// Valid payload size for orientation data (includes sensor ID)
#define ORI_PAYLOAD_SIZE_BYTES 7

// Timestamp conversion (15625 ns per unit)
#define TIMESTAMP_UNIT_NS 15625ULL
#define NSEC_PER_SEC      1000000000ULL

// Default sample rate for sensor configuration
#define DEFAULT_SAMPLE_RATE_HZ 50.0f
#define DEFAULT_SENSOR_LATENCY 0

static uint8_t        write_buf[1024];
static uint8_t        work_buffer[WORK_BUFFER_SIZE];
static struct bhy_dev bhy;
static enum bhy_intf  intf;

static void print_api_error(int8_t rslt, struct bhy_dev *dev);
void        parse_euler(const struct bhy_fifo_parse_data_info *info, void *ref);

bhi360_fusion_sample_data_t sample_data;

void
parse_euler (const struct bhy_fifo_parse_data_info *info, void *ref)
{
    (void)ref;
    if (info->data_size != ORI_PAYLOAD_SIZE_BYTES)
    {
        return;
    }

    struct bhy_event_data_orientation data;
    bhy_event_data_parse_orientation(info->data_ptr, &data);

    sample_data.pitch_deg = data.pitch * ORIENTATION_SCALE_FACTOR;
    sample_data.roll_deg  = data.roll * ORIENTATION_SCALE_FACTOR;
    sample_data.yaw_deg   = data.heading * ORIENTATION_SCALE_FACTOR;
}

#ifdef DEBUG
static char *
get_api_error (int8_t error_code)
{
    switch (error_code)
    {
        case BHY_OK: {
            return " ";
        }
        case BHY_E_NULL_PTR: {
            return "[API Error] Null pointer";
        }
        case BHY_E_INVALID_PARAM: {
            return "[API Error] Invalid parameter";
        }
        case BHY_E_IO: {
            return "[API Error] IO error";
        }
        case BHY_E_MAGIC: {
            return "[API Error] Invalid firmware";
        }
        case BHY_E_TIMEOUT: {
            return "[API Error] Timed out";
        }
        case BHY_E_BUFFER: {
            return "[API Error] Invalid buffer";
        }
        case BHY_E_INVALID_FIFO_TYPE: {
            return "[API Error] Invalid FIFO type";
        }
        case BHY_E_INVALID_EVENT_SIZE: {
            return "[API Error] Invalid Event size";
        }
        case BHY_E_PARAM_NOT_SET: {
            return "[API Error] Parameter not set";
        }
        default: {
            return "[API Error] Unknown API error code";
        }
    }
}
#endif

static void
print_api_error (int8_t rslt, struct bhy_dev *dev)
{
    if (rslt != BHY_OK)
    {
        LOG_DEBUG("%s\r\n", get_api_error(rslt));
        if (rslt == BHY_E_IO && dev != NULL)
        {
            LOG_DEBUG("i2c error!\r\n");
            dev->hif.intf_rslt = BHY_INTF_RET_SUCCESS;
        }
        exit(0);
    }
}

static manikin_status_t
bhi360_check_params (const manikin_sensor_ctx_t *sensor_ctx)
{
    MANIKIN_ASSERT(HASH_BHI360, sensor_ctx != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_BHI360, sensor_ctx->i2c != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}

int8_t
bhi360_i2c_read (uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    manikin_sensor_ctx_t *ctx = (manikin_sensor_ctx_t *)intf_ptr;
    size_t status             = MANIKIN_I2C_HAL_WRITE_BYTES(ctx->i2c, ctx->i2c_addr, &reg_addr, 1);
    size_t res = MANIKIN_I2C_HAL_READ_BYTES(ctx->i2c, ctx->i2c_addr, reg_data, length);
    return (res == length && status == 1) ? 0 : -1;
}

int8_t
bhi360_i2c_write (uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    manikin_sensor_ctx_t *ctx = (manikin_sensor_ctx_t *)intf_ptr;
    write_buf[0]              = reg_addr;
    memcpy(write_buf + 1, reg_data, length);
    size_t res = MANIKIN_I2C_HAL_WRITE_BYTES(ctx->i2c, ctx->i2c_addr, write_buf, length + 1);
    return (res == length + 1) ? 0 : -1;
}

void
bhi360_delay (uint32_t period, void *intf_ptr)
{
    (void)intf_ptr;
    MANIKIN_DELAY_US(period);
}

static void
upload_firmware (uint8_t boot_stat, struct bhy_dev *dev)
{
    (void)boot_stat;
    LOG_DEBUG("Loading firmware into RAM.\r\n");

    int8_t  rslt = bhy_upload_firmware_to_ram(bhy_firmware_image, sizeof(bhy_firmware_image), dev);
    uint8_t sensor_error;
    int8_t  temp_rslt = bhy_get_error_value(&sensor_error, dev);
    print_api_error(rslt, dev);
    print_api_error(temp_rslt, dev);

    LOG_DEBUG("Booting from RAM.\r\n");
    rslt      = bhy_boot_from_ram(dev);
    temp_rslt = bhy_get_error_value(&sensor_error, dev);
    print_api_error(rslt, dev);
    print_api_error(temp_rslt, dev);
}

static void
parse_meta_event (const struct bhy_fifo_parse_data_info *info, void *ref)
{
    (void)ref;
    const uint8_t meta_event_type = info->data_ptr[0];
#ifdef DEBUG
    const uint8_t byte1 = info->data_ptr[1];
    const uint8_t byte2 = info->data_ptr[2];
#endif
    const char *event_text = (info->sensor_id == BHY_SYS_ID_META_EVENT) ? "[META EVENT]"
                             : (info->sensor_id == BHY_SYS_ID_META_EVENT_WU)
                                 ? "[META EVENT WAKE UP]"
                                 : NULL;

    if (!event_text)
    {
        return;
    }

    switch (meta_event_type)
    {
        case BHY_META_EVENT_FLUSH_COMPLETE:
            LOG_DEBUG("%s Flush complete for sensor id %u\r\n", event_text, byte1);
            break;
        case BHY_META_EVENT_SAMPLE_RATE_CHANGED:
            LOG_DEBUG("%s Sample rate changed for sensor id %u\r\n", event_text, byte1);
            break;
        case BHY_META_EVENT_POWER_MODE_CHANGED:
            LOG_DEBUG("%s Power mode changed for sensor id %u\r\n", event_text, byte1);
            break;
        case BHY_META_EVENT_ALGORITHM_EVENTS:
            LOG_DEBUG("%s Algorithm event\r\n", event_text);
            break;
        case BHY_META_EVENT_SENSOR_STATUS:
            LOG_DEBUG("%s Accuracy for sensor id %u changed to %u\r\n", event_text, byte1, byte2);
            break;
        case BHY_META_EVENT_SENSOR_ERROR:
            LOG_DEBUG("%s Sensor id %u reported error 0x%02X\r\n", event_text, byte1, byte2);
            break;
        case BHY_META_EVENT_INITIALIZED:
            LOG_DEBUG("%s Firmware initialized. Firmware version %u\r\n",
                      event_text,
                      ((uint16_t)byte2 << 8) | byte1);
            break;
        case BHY_META_EVENT_FIFO_OVERFLOW:
        case BHY_META_EVENT_FIFO_WATERMARK:
        case BHY_META_EVENT_RESET:
        case BHY_META_EVENT_DYNAMIC_RANGE_CHANGED:
        case BHY_META_EVENT_BSX_DO_STEPS_MAIN:
        case BHY_META_EVENT_BSX_DO_STEPS_CALIB:
        case BHY_META_EVENT_BSX_GET_OUTPUT_SIGNAL:
        case BHY_META_EVENT_SENSOR_FRAMEWORK:
        case BHY_META_TRANSFER_CAUSE:
            LOG_DEBUG("%s Event type %u\r\n", event_text, meta_event_type);
            break;
        default:
            LOG_DEBUG("%s Unknown meta event with id: %u\r\n", event_text, meta_event_type);
            break;
    }
}

manikin_status_t
bhi360_fusion_init_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    MANIKIN_ASSERT(HASH_BHI360,
                   bhi360_check_params(sensor_ctx) == MANIKIN_STATUS_OK,
                   MANIKIN_STATUS_ERR_NULL_PARAM);
    sensor_ctx->needs_reinit = 0;
    intf                     = BHY_I2C_INTERFACE;

    int8_t rslt = bhy_init(BHY_I2C_INTERFACE,
                           bhi360_i2c_read,
                           bhi360_i2c_write,
                           bhi360_delay,
                           BHY_RD_WR_LEN,
                           sensor_ctx,
                           &bhy);
    print_api_error(rslt, &bhy);

    rslt = bhy_soft_reset(&bhy);
    print_api_error(rslt, &bhy);

    uint8_t chip_id;
    rslt = bhy_get_chip_id(&chip_id, &bhy);
    print_api_error(rslt, &bhy);
    LOG_DEBUG("Chip ID read 0x%X\r\n", chip_id);

    if (chip_id != BHI3_CHIP_ID_BHI360)
    {
        LOG_DEBUG("Device not found. Exiting.\r\n");
        return 1;
    }

    uint8_t hintr_ctrl = BHY_ICTL_DISABLE_STATUS_FIFO | BHY_ICTL_DISABLE_DEBUG;
    rslt               = bhy_set_host_interrupt_ctrl(hintr_ctrl, &bhy);
    print_api_error(rslt, &bhy);

    uint8_t boot_status;
    rslt = bhy_get_boot_status(&boot_status, &bhy);
    print_api_error(rslt, &bhy);

    if (!(boot_status & BHY_BST_HOST_INTERFACE_READY))
    {
        LOG_DEBUG("Host interface not ready. Exiting.\r\n");
        return 1;
    }

    upload_firmware(boot_status, &bhy);

    uint16_t version;
    rslt = bhy_get_kernel_version(&version, &bhy);
    print_api_error(rslt, &bhy);
    if (version)
    {
        LOG_DEBUG("Boot successful. Kernel version %u\r\n", version);
    }

    bhy_register_fifo_parse_callback(BHY_SYS_ID_META_EVENT, parse_meta_event, NULL, &bhy);
    bhy_register_fifo_parse_callback(BHY_SYS_ID_META_EVENT_WU, parse_meta_event, NULL, &bhy);
    bhy_register_fifo_parse_callback(BHY_SENSOR_ID_ORI, parse_euler, NULL, &bhy);
    bhy_get_and_process_fifo(work_buffer, WORK_BUFFER_SIZE, &bhy);

    bhy_update_virtual_sensor_list(&bhy);
    struct bhy_virtual_sensor_conf_param_conf sensor_conf
        = { .sample_rate = DEFAULT_SAMPLE_RATE_HZ, .latency = DEFAULT_SENSOR_LATENCY };
    bhy_virtual_sensor_conf_param_set_cfg(QUAT_SENSOR_ID, &sensor_conf, &bhy);
    bhy_virtual_sensor_conf_param_set_cfg(BHY_SENSOR_ID_ORI, &sensor_conf, &bhy);
    LOG_DEBUG("Enable virtual ori sensor at %.2fHz\r\n", sensor_conf.sample_rate);

    return MANIKIN_STATUS_OK;
}

manikin_status_t
bhi360_fusion_read_sensor (manikin_sensor_ctx_t *sensor_ctx, uint8_t *read_buf)
{
    (void)read_buf;
    MANIKIN_ASSERT(HASH_BHI360,
                   bhi360_check_params(sensor_ctx) == MANIKIN_STATUS_OK,
                   MANIKIN_STATUS_ERR_NULL_PARAM);

    if (sensor_ctx->needs_reinit)
    {
        bhi360_fusion_init_sensor(sensor_ctx);
    }
    print_api_error(bhy_get_and_process_fifo(work_buffer, WORK_BUFFER_SIZE, &bhy), &bhy);

    memcpy(read_buf, &sample_data, sizeof(sample_data));
    return MANIKIN_STATUS_OK;
}

manikin_status_t
bhi360_fusion_deinit_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    return bhi360_check_params(sensor_ctx);
}

manikin_status_t
bhi360_fusion_parse_raw_data (const uint8_t *raw_data, bhi360_fusion_sample_data_t *data)
{
    MANIKIN_ASSERT(HASH_BHI360, raw_data != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_BHI360, data != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);
    memcpy(data, raw_data, sizeof(sample_data));
    return MANIKIN_STATUS_OK;
}
