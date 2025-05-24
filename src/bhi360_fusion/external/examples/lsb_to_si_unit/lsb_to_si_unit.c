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
 * @file    lsb_to_si_unit.c
 * @brief   Example to convert Accel/Gyro/Mag sensor outputs from LSB to SI-unit
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

#include "bhy.h"
#include "common.h"
#include "bhy_virtual_sensor_conf_param.h"
#include "bhy_event_data.h"

#include "bhi360/Bosch_Shuttle3_BHI360_BMM350C_BMP580_BME688.fw.h"

#define WORK_BUFFER_SIZE             UINT16_C(2048)
#define SCALING_FACTOR_INVALID_LIMIT -1.0f

/*! @brief Parse meta event.
 *
 *  @param[in] callback_info : Sensor data info.
 *  @param[in] callback_ref  : Parse reference.
 */
static void parse_meta_event(const struct bhy_fifo_parse_data_info *callback_info,
                             void                                  *callback_ref);

/*! @brief Prints API error code.
 *
 *  @param[in] rslt      : API Error code.
 *  @param[in] dev       : Device reference.
 */
static void print_api_error(int8_t rslt, struct bhy_dev *dev);

/*! @brief Loads firmware image to BHy ram.
 *
 *  @param[in] boot_stat : Boot status.
 *  @param[in] dev       : Device reference.
 */
static void upload_firmware(uint8_t boot_stat, struct bhy_dev *dev);

/*! @brief Parse sensor data.
 *
 *  @param[in] callback_info : sensor data info.
 *  @param[in] callback_ref  : Parse reference.
 */
static void parse_sensor_data(const struct bhy_fifo_parse_data_info *callback_info,
                              void                                  *callback_ref);

/*! Note: Test the gyro data with the smaller range. e.g: 125dps, 500dps. board with violently
 * shaking movements. For accel, back and forth fast movements of the board in x / y axis by setting
 * the smallest range 2G. For Mag, use strong magnet to close to the shuttle and check the mag
 * outputs. Then could see the largest value will not beyond the set range.*/

uint8_t sensor_id = BHY_SENSOR_ID_ACC_PASS; /* Accel:BHY_SENSOR_ID_ACC_PASS
                                             * Gyro:BHY_SENSOR_ID_GYRO_PASS
                                             * Mag:BHY_SENSOR_ID_MAG_PASS */
float dynamic_range = (float)
    BHY_ACCEL_2G; /* Dynamic Range set at a value, so that the largest accel / gyro / mag value
                   * printed will not beyond the set range. Accel dynamic ranges: 2G,4G,8G,16G.
                   * Gyro dynamic ranges: 125dps,250dps,500dps,1000dps,2000dps.
                   * Mag dynamic ranges: Fixed at 2500 microtesla for BMM150 in BHI. */

enum bhy_intf intf;

int
main (void)
{
    uint8_t        chip_id = 0;
    uint16_t       version = 0;
    int8_t         rslt;
    struct bhy_dev bhy;
    uint8_t        hif_ctrl, boot_status, hintr_ctrl;
    uint8_t
        accuracy; /* Accuracy is reported as a meta event. It is being printed alongside the data */
    uint8_t work_buffer[WORK_BUFFER_SIZE];
    uint8_t loop  = 0;
    uint8_t limit = 50;

#ifdef BHY_USE_I2C
    intf = BHY_I2C_INTERFACE;
#else
    intf = BHY_SPI_INTERFACE;
#endif

    setup_interfaces(true, intf); /* Perform a power on reset */

#ifdef BHY_USE_I2C
    rslt = bhy_init(
        BHY_I2C_INTERFACE, bhy_i2c_read, bhy_i2c_write, bhy_delay_us, BHY_RD_WR_LEN, NULL, &bhy);
#else
    rslt = bhy_init(
        BHY_SPI_INTERFACE, bhy_spi_read, bhy_spi_write, bhy_delay_us, BHY_RD_WR_LEN, NULL, &bhy);
#endif
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

    /* Configure the host interface */
    hif_ctrl = BHY_HIF_CTRL_ASYNC_STATUS_CHANNEL;
    rslt     = bhy_set_host_intf_ctrl(hif_ctrl, &bhy);
    print_api_error(rslt, &bhy);

    /* Check the interrupt pin and FIFO configurations. Disable status and debug */
    hintr_ctrl = BHY_ICTL_DISABLE_STATUS_FIFO | BHY_ICTL_DISABLE_DEBUG;

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

        rslt = bhy_register_fifo_parse_callback(
            BHY_SYS_ID_META_EVENT, parse_meta_event, (void *)&accuracy, &bhy);
        print_api_error(rslt, &bhy);
        rslt = bhy_register_fifo_parse_callback(
            BHY_SYS_ID_META_EVENT_WU, parse_meta_event, (void *)&accuracy, &bhy);
        print_api_error(rslt, &bhy);
        rslt = bhy_register_fifo_parse_callback(sensor_id, parse_sensor_data, NULL, &bhy);
        print_api_error(rslt, &bhy);

        rslt = bhy_get_and_process_fifo(work_buffer, WORK_BUFFER_SIZE, &bhy);
        print_api_error(rslt, &bhy);
    }
    else
    {
        printf("Host interface not ready. Exiting\r\n");

        close_interfaces(intf);

        return 0;
    }

    /* Update the callback table to enable parsing of sensor data */
    rslt = bhy_update_virtual_sensor_list(&bhy);
    print_api_error(rslt, &bhy);

    struct bhy_virtual_sensor_conf_param_conf sensor_conf = { 0 };

    sensor_conf.sample_rate = 100.0f; /* Read out data measured at 100Hz */
    sensor_conf.latency     = 0;      /* Report immediately */
    rslt                    = bhy_virtual_sensor_conf_param_set_cfg(sensor_id, &sensor_conf, &bhy);
    print_api_error(rslt, &bhy);
    printf("Enable %s at %.2fHz.\r\n", get_sensor_name(sensor_id), sensor_conf.sample_rate);

    rslt = bhy_set_virt_sensor_range(sensor_id, dynamic_range, &bhy);
    print_api_error(rslt, &bhy);

    while (rslt == BHY_OK && loop < limit)
    {
        if (get_interrupt_status())
        {
            /* Data from the FIFO is read and the relevant callbacks if registered are called */
            rslt = bhy_get_and_process_fifo(work_buffer, WORK_BUFFER_SIZE, &bhy);
            loop++;
            print_api_error(rslt, &bhy);
        }
    }

    close_interfaces(intf);

    return rslt;
}

static void
parse_sensor_data (const struct bhy_fifo_parse_data_info *callback_info, void *callback_ref)
{
    struct bhy_event_data_xyz data;
    uint32_t                  s, ns;
    float                     scaling_factor = SCALING_FACTOR_INVALID_LIMIT;
    char                     *si_unit;

    if (!callback_info)
    {
        printf("Null reference\r\n");

        return;
    }

    si_unit = get_sensor_si_unit(callback_info->sensor_id);
    bhy_event_data_parse_xyz(callback_info->data_ptr, &data);

    uint64_t timestamp = *callback_info->time_stamp; /* Store the last timestamp */
    timestamp          = timestamp * 15625;          /* Timestamp is now in nanoseconds */
    s                  = (uint32_t)(timestamp / UINT64_C(1000000000));
    ns                 = (uint32_t)(timestamp - (s * UINT64_C(1000000000)));

    scaling_factor = get_sensor_dynamic_range_scaling(callback_info->sensor_id, dynamic_range);

    if (scaling_factor > SCALING_FACTOR_INVALID_LIMIT)
    {
        /*lint -e10 Error 10: Lint does not understand PRIxxx */
        printf("T: %" PRIu32 ".%09" PRIu32 "; x: %f, y: %f, z: %f; unit: %s\r\n",
               s,
               ns,
               data.x * scaling_factor,
               data.y * scaling_factor,
               data.z * scaling_factor,
               si_unit);

        /*lint +e10 */
    }
}

static void
parse_meta_event (const struct bhy_fifo_parse_data_info *callback_info, void *callback_ref)
{
    (void)callback_ref;
    uint8_t  meta_event_type = callback_info->data_ptr[0];
    uint8_t  byte1           = callback_info->data_ptr[1];
    uint8_t  byte2           = callback_info->data_ptr[2];
    uint8_t *accuracy        = (uint8_t *)callback_ref;
    char    *event_text;

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
            if (accuracy)
            {
                *accuracy = byte2;
            }

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

static void
print_api_error (int8_t rslt, struct bhy_dev *dev)
{
    if (rslt != BHY_OK)
    {
        printf("%s\r\n", get_api_error(rslt));
        if ((rslt == BHY_E_IO) && (dev != NULL))
        {
            printf("%s\r\n", get_coines_error(dev->hif.intf_rslt));
            dev->hif.intf_rslt = BHY_INTF_RET_SUCCESS;
        }

        close_interfaces(intf);
        exit(0);
    }
}

static void
upload_firmware (uint8_t boot_stat, struct bhy_dev *dev)
{
    uint8_t sensor_error;
    int8_t  temp_rslt;
    int8_t  rslt = BHY_OK;

    printf("Loading firmware into RAM.\r\n");
    rslt      = bhy_upload_firmware_to_ram(bhy_firmware_image, sizeof(bhy_firmware_image), dev);
    temp_rslt = bhy_get_error_value(&sensor_error, dev);
    if (sensor_error)
    {
        printf("%s\r\n", get_sensor_error_text(sensor_error));
    }

    print_api_error(rslt, dev);
    print_api_error(temp_rslt, dev);

    printf("Booting from RAM.\r\n");
    rslt = bhy_boot_from_ram(dev);

    temp_rslt = bhy_get_error_value(&sensor_error, dev);
    if (sensor_error)
    {
        printf("%s\r\n", get_sensor_error_text(sensor_error));
    }

    print_api_error(rslt, dev);
    print_api_error(temp_rslt, dev);
}
