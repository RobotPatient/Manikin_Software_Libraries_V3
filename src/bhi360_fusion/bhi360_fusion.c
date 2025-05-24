/**
 * @file            bhi360.c
 * @brief           Driver implementation for a BHI360 programmable IMU
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

#include <bhi360/bhi360.h>
#include "i2c/i2c.h"
#include "error_handler/error_handler.h"
#include "bhy.h"
#include "bhy_virtual_sensor_conf_param.h"
#include "bhy_event_data.h"
#include "manikin_platform.h"
#include "external/bhy.h"
#include "external/bhy_defs.h"
#include "external/bhi3_defs.h"

#include "firmware/bhi360/Bosch_Shuttle3_BHI360_BMM350C_BMP580_BME688.fw.h"

#include <inttypes.h>
#include <stdio.h>

#define HASH_BHI360      0xC3A556E3u
#define WORK_BUFFER_SIZE 2048
#define QUAT_SENSOR_ID   BHY_SENSOR_ID_RV

// static void parse_quaternion_data(const struct bhy_fifo_parse_data_info *callback_info, void
// *callback_ref); static void parse_meta_event(const struct bhy_fifo_parse_data_info
// *callback_info, void *callback_ref);
static void print_api_error(int8_t rslt, struct bhy_dev *dev);
// static void upload_firmware(uint8_t boot_stat, struct bhy_dev *dev);

char *
get_api_error (int8_t error_code)
{
    char *ret = " ";

    switch (error_code)
    {
        case BHY_OK:
            break;
        case BHY_E_NULL_PTR:
            ret = "[API Error] Null pointer";
            break;
        case BHY_E_INVALID_PARAM:
            ret = "[API Error] Invalid parameter";
            break;
        case BHY_E_IO:
            ret = "[API Error] IO error";
            break;
        case BHY_E_MAGIC:
            ret = "[API Error] Invalid firmware";
            break;
        case BHY_E_TIMEOUT:
            ret = "[API Error] Timed out";
            break;
        case BHY_E_BUFFER:
            ret = "[API Error] Invalid buffer";
            break;
        case BHY_E_INVALID_FIFO_TYPE:
            ret = "[API Error] Invalid FIFO type";
            break;
        case BHY_E_INVALID_EVENT_SIZE:
            ret = "[API Error] Invalid Event size";
            break;
        case BHY_E_PARAM_NOT_SET:
            ret = "[API Error] Parameter not set";
            break;
        default:
            ret = "[API Error] Unknown API error code";
    }

    return ret;
}
enum bhy_intf intf;
static void
print_api_error (int8_t rslt, struct bhy_dev *dev)
{
    if (rslt != BHY_OK)
    {
        printf("%s\r\n", get_api_error(rslt));
        if ((rslt == BHY_E_IO) && (dev != NULL))
        {
            printf("%s\r\n", "i2c error!");
            dev->hif.intf_rslt = BHY_INTF_RET_SUCCESS;
        }

        exit(0);
    }
}

/**
 * @brief Internal function to check the parameters entered into function
 * @param sensor_ctx The sensor settings ptr which should contain i2c bus details
 * @return - MANIKIN_STATUS_OK if all parameters are valid
 *         - MANIKIN_STATUS_ERR_NULL_PARAM if invalid
 */
static manikin_status_t
bhi360_check_params (const manikin_sensor_ctx_t *sensor_ctx)
{
    MANIKIN_ASSERT(HASH_BHI360, (sensor_ctx != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_BHI360, (sensor_ctx->i2c != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}

/*!
 * I2C read function map to COINES platform
 */
int8_t
bhi360_i2c_read (uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    manikin_sensor_ctx_t *sensor_ctx = (manikin_sensor_ctx_t *)intf_ptr;

    size_t status
        = MANIKIN_I2C_HAL_WRITE_BYTES(sensor_ctx->i2c, sensor_ctx->i2c_addr, &reg_addr, 1);
    size_t res
        = MANIKIN_I2C_HAL_READ_BYTES(sensor_ctx->i2c, sensor_ctx->i2c_addr, reg_data, length);

    return res == length && status == 1 ? 0 : -1;
}
#define BHY_RD_WR_LEN 256 /* Coines bridge maximum read write length */
uint8_t write_buf[1024];
/*!
 * I2C write function map to COINES platform
 */
int8_t
bhi360_i2c_write (uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    manikin_sensor_ctx_t *sensor_ctx = (manikin_sensor_ctx_t *)intf_ptr;
    write_buf[0]                     = reg_addr;
    memcpy(write_buf + 1, reg_data, length);
    size_t res = MANIKIN_I2C_HAL_WRITE_BYTES(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, write_buf, (uint16_t)length + 1);
    return res == length + 1 ? 0 : -1;
}

void
bhi360_delay (uint32_t period, void *intf_ptr)
{
    (void)intf_ptr;
    MANIKIN_DELAY_US(period);
}
static void
parse_quaternion_data (const struct bhy_fifo_parse_data_info *callback_info, void *callback_ref)
{
    (void)callback_ref;
    struct bhy_event_data_quaternion data;
    uint32_t                         s, ns;
    if (callback_info->data_size != 11) /* Check for a valid payload size. Includes sensor ID */
    {
        return;
    }

    bhy_event_data_parse_quaternion(callback_info->data_ptr, &data);

    uint64_t timestamp = *callback_info->time_stamp; /* Store the last timestamp */

    timestamp = timestamp * 15625; /* Timestamp is now in nanoseconds */
    s         = (uint32_t)(timestamp / UINT64_C(1000000000));
    ns        = (uint32_t)(timestamp - (s * UINT64_C(1000000000)));

    /*lint -e10 Error 10: Lint does not understand PRIxxx */
    printf("SID: %u; T: %" PRIu32 ".%09" PRIu32 "; x: %f, y: %f, z: %f, w: %f; acc: %.2f\r\n",
           callback_info->sensor_id,
           s,
           ns,
           data.x / 16384.0,
           data.y / 16384.0,
           data.z / 16384.0,
           data.w / 16384.0,
           ((data.accuracy * 180.0) / 16384.0) / 3.141592653589793);

    /*lint +e10 */
}

static void
upload_firmware (uint8_t boot_stat, struct bhy_dev *dev)
{
    uint8_t sensor_error;
    int8_t  temp_rslt;
    int8_t  rslt = BHY_OK;
    (void)boot_stat;
    printf("Loading firmware into RAM.\r\n");
    rslt      = bhy_upload_firmware_to_ram(bhy_firmware_image, sizeof(bhy_firmware_image), dev);
    temp_rslt = bhy_get_error_value(&sensor_error, dev);
    if (sensor_error)
    {
        // printf("%s\r\n", get_sensor_error_text(sensor_error));
    }

    print_api_error(rslt, dev);
    print_api_error(temp_rslt, dev);

    printf("Booting from RAM.\r\n");
    rslt = bhy_boot_from_ram(dev);

    temp_rslt = bhy_get_error_value(&sensor_error, dev);
    if (sensor_error)
    {
        // printf("%s\r\n", get_sensor_error_text(sensor_error));
    }

    print_api_error(rslt, dev);
    print_api_error(temp_rslt, dev);
}
static void
parse_meta_event (const struct bhy_fifo_parse_data_info *callback_info, void *callback_ref)
{
    (void)callback_ref;
    uint8_t meta_event_type = callback_info->data_ptr[0];
    uint8_t byte1           = callback_info->data_ptr[1];
    uint8_t byte2           = callback_info->data_ptr[2];
    char   *event_text;

    if (callback_info->sensor_id == BHY_SYS_ID_META_EVENT)
    {
        event_text = "[META EVENT]";
    }
    else if (callback_info->sensor_id == BHY_SYS_ID_META_EVENT_WU)
    {
        event_text = "[META EVENT WAKE UP]";
    }
    else
    {
        return;
    }

    switch (meta_event_type)
    {
        case BHY_META_EVENT_FLUSH_COMPLETE:
            printf("%s Flush complete for sensor id %u\r\n", event_text, byte1);
            break;
        case BHY_META_EVENT_SAMPLE_RATE_CHANGED:
            printf("%s Sample rate changed for sensor id %u\r\n", event_text, byte1);
            break;
        case BHY_META_EVENT_POWER_MODE_CHANGED:
            printf("%s Power mode changed for sensor id %u\r\n", event_text, byte1);
            break;
        case BHY_META_EVENT_ALGORITHM_EVENTS:
            printf("%s Algorithm event\r\n", event_text);
            break;
        case BHY_META_EVENT_SENSOR_STATUS:
            printf("%s Accuracy for sensor id %u changed to %u\r\n", event_text, byte1, byte2);
            break;
        case BHY_META_EVENT_BSX_DO_STEPS_MAIN:
            printf("%s BSX event (do steps main)\r\n", event_text);
            break;
        case BHY_META_EVENT_BSX_DO_STEPS_CALIB:
            printf("%s BSX event (do steps calib)\r\n", event_text);
            break;
        case BHY_META_EVENT_BSX_GET_OUTPUT_SIGNAL:
            printf("%s BSX event (get output signal)\r\n", event_text);
            break;
        case BHY_META_EVENT_SENSOR_ERROR:
            printf("%s Sensor id %u reported error 0x%02X\r\n", event_text, byte1, byte2);
            break;
        case BHY_META_EVENT_FIFO_OVERFLOW:
            printf("%s FIFO overflow\r\n", event_text);
            break;
        case BHY_META_EVENT_DYNAMIC_RANGE_CHANGED:
            printf("%s Dynamic range changed for sensor id %u\r\n", event_text, byte1);
            break;
        case BHY_META_EVENT_FIFO_WATERMARK:
            printf("%s FIFO watermark reached\r\n", event_text);
            break;
        case BHY_META_EVENT_INITIALIZED:
            printf("%s Firmware initialized. Firmware version %u\r\n",
                   event_text,
                   ((uint16_t)byte2 << 8) | byte1);
            break;
        case BHY_META_TRANSFER_CAUSE:
            printf("%s Transfer cause for sensor id %u\r\n", event_text, byte1);
            break;
        case BHY_META_EVENT_SENSOR_FRAMEWORK:
            printf("%s Sensor framework event for sensor id %u\r\n", event_text, byte1);
            break;
        case BHY_META_EVENT_RESET:
            printf("%s Reset event\r\n", event_text);
            break;
        case BHY_META_EVENT_SPACER:
            break;
        default:
            printf("%s Unknown meta event with id: %u\r\n", event_text, meta_event_type);
            break;
    }
}
manikin_status_t
bhi360_init_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = bhi360_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BHI360, (status == MANIKIN_STATUS_OK), status);

    // NOTE: needs_reinit is internal variable, which might be uninitialized when entered as param.
    sensor_ctx->needs_reinit = 0;
    uint8_t        chip_id   = 0;
    uint16_t       version   = 0;
    int8_t         rslt;
    struct bhy_dev bhy;
    uint8_t        work_buffer[WORK_BUFFER_SIZE];
    uint8_t        hintr_ctrl, hif_ctrl, boot_status;
    // uint8_t loop = 0;
    // uint8_t limit = 50;

#ifdef BHY_USE_I2C
    intf = BHY_I2C_INTERFACE;
#else
    intf = BHY_SPI_INTERFACE;
#endif

    // setup_interfaces(true, intf); /* Perform a power on reset */

    rslt = bhy_init(BHY_I2C_INTERFACE,
                    bhi360_i2c_read,
                    bhi360_i2c_write,
                    bhi360_delay,
                    BHY_RD_WR_LEN,
                    NULL,
                    &bhy);
    print_api_error(rslt, &bhy);

    rslt = bhy_soft_reset(&bhy);
    print_api_error(rslt, &bhy);

    rslt = bhy_get_chip_id(&chip_id, &bhy);
    print_api_error(rslt, &bhy);

    /* Check for a valid Chip ID */
    if (chip_id == BHI3_CHIP_ID_BHI360)
    {
        printf("Chip ID read 0x%X\r\n", chip_id);
    }
    else
    {
        printf("Device not found. Chip ID read 0x%X\r\n", chip_id);
    }
    /* Check the interrupt pin and FIFO configurations. Disable status and debug */
    hintr_ctrl = BHY_ICTL_DISABLE_STATUS_FIFO | BHY_ICTL_DISABLE_DEBUG;

    rslt = bhy_set_host_interrupt_ctrl(hintr_ctrl, &bhy);
    print_api_error(rslt, &bhy);
    rslt = bhy_get_host_interrupt_ctrl(&hintr_ctrl, &bhy);
    print_api_error(rslt, &bhy);

    printf("Host interrupt control\r\n");
    printf("    Wake up FIFO %s.\r\n",
           (hintr_ctrl & BHY_ICTL_DISABLE_FIFO_W) ? "disabled" : "enabled");
    printf("    Non wake up FIFO %s.\r\n",
           (hintr_ctrl & BHY_ICTL_DISABLE_FIFO_NW) ? "disabled" : "enabled");
    printf("    Status FIFO %s.\r\n",
           (hintr_ctrl & BHY_ICTL_DISABLE_STATUS_FIFO) ? "disabled" : "enabled");
    printf("    Debugging %s.\r\n", (hintr_ctrl & BHY_ICTL_DISABLE_DEBUG) ? "disabled" : "enabled");
    printf("    Fault %s.\r\n", (hintr_ctrl & BHY_ICTL_DISABLE_FAULT) ? "disabled" : "enabled");
    printf("    Interrupt is %s.\r\n",
           (hintr_ctrl & BHY_ICTL_ACTIVE_LOW) ? "active low" : "active high");
    printf("    Interrupt is %s triggered.\r\n", (hintr_ctrl & BHY_ICTL_EDGE) ? "pulse" : "level");
    printf("    Interrupt pin drive is %s.\r\n",
           (hintr_ctrl & BHY_ICTL_OPEN_DRAIN) ? "open drain" : "push-pull");

    /* Configure the host interface */
    hif_ctrl = 0;
    rslt     = bhy_set_host_intf_ctrl(hif_ctrl, &bhy);
    print_api_error(rslt, &bhy);

    /* Check if the sensor is ready to load firmware */
    rslt = bhy_get_boot_status(&boot_status, &bhy);
    print_api_error(rslt, &bhy);

    if (boot_status & BHY_BST_HOST_INTERFACE_READY)
    {
        upload_firmware(boot_status, &bhy);

        rslt = bhy_get_kernel_version(&version, &bhy);
        print_api_error(rslt, &bhy);
        if ((rslt == BHY_OK) && (version != 0))
        {
            printf("Boot successful. Kernel version %u.\r\n", version);
        }

        rslt
            = bhy_register_fifo_parse_callback(BHY_SYS_ID_META_EVENT, parse_meta_event, NULL, &bhy);
        print_api_error(rslt, &bhy);
        rslt = bhy_register_fifo_parse_callback(
            BHY_SYS_ID_META_EVENT_WU, parse_meta_event, NULL, &bhy);
        print_api_error(rslt, &bhy);
        rslt = bhy_register_fifo_parse_callback(QUAT_SENSOR_ID, parse_quaternion_data, NULL, &bhy);
        print_api_error(rslt, &bhy);

        rslt = bhy_get_and_process_fifo(work_buffer, WORK_BUFFER_SIZE, &bhy);
        print_api_error(rslt, &bhy);
    }
    else
    {
        printf("Host interface not ready. Exiting\r\n");

        // close_interfaces(intf);

        return 0;
    }

    // /* Update the callback table to enable parsing of sensor data */
    // rslt = bhy_update_virtual_sensor_list(&bhy);
    // print_api_error(rslt, &bhy);
    //
    // struct bhy_virtual_sensor_conf_param_conf sensor_conf = { 0 };
    //
    // sensor_conf.sample_rate = 100.0f; /* Read out data measured at 100Hz */
    // sensor_conf.latency = 0; /* Report immediately */
    // rslt = bhy_virtual_sensor_conf_param_set_cfg(QUAT_SENSOR_ID, &sensor_conf, &bhy);
    // print_api_error(rslt, &bhy);
    // printf("Enable %s at %.2fHz.\r\n", get_sensor_name(QUAT_SENSOR_ID), sensor_conf.sample_rate);
    //
    // while (rslt == BHY_OK && loop < limit)
    // {
    //     if (get_interrupt_status())
    //     {
    //         /* Data from the FIFO is read and the relevant callbacks if registered are called */
    //         rslt = bhy_get_and_process_fifo(work_buffer, WORK_BUFFER_SIZE, &bhy);
    //         loop++;
    //         print_api_error(rslt, &bhy);
    //     }
    // }

    return MANIKIN_STATUS_OK;
}

manikin_status_t
bhi360_read_sensor (manikin_sensor_ctx_t *sensor_ctx, uint8_t *read_buf)
{
    (void)read_buf;
    manikin_status_t status = bhi360_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BHI360, (status == MANIKIN_STATUS_OK), status);

    if (sensor_ctx->needs_reinit)
    {
        bhi360_init_sensor(sensor_ctx);
    }
    return MANIKIN_STATUS_OK;
}

manikin_status_t
bhi360_deinit_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = bhi360_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BHI360, (status == MANIKIN_STATUS_OK), status);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
bhi360_parse_raw_data (const uint8_t *raw_data, bhi360_sample_data_t *data)
{
    MANIKIN_ASSERT(HASH_BHI360, (raw_data != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_BHI360, (data != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}