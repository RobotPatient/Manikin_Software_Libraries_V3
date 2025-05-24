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
 * @file    head_orientation.c
 * @brief   Head orientation example for the BHY
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

#include "bhy.h"
#include "bhy_parse.h"
#include "bhy_head_orientation_param_defs.h"
#include "bhy_head_orientation_param.h"
#include "bhy_virtual_sensor_conf_param.h"
#include "bhy_event_data.h"
#include "common.h"

#include "bhi360/Bosch_Shuttle3_BHI360_IMU_HeadOrientation.fw.h"

#define WORK_BUFFER_SIZE 2048

#define PARAM_BUF_LEN 252

#define PARSE_DATA_WINDOW_SIZE UINT16_C(3000)

#define HEAD_ORIENTATION_SENSOR_ID BHY_SENSOR_ID_HEAD_ORI_MIS_ALG

/*! @brief Reads sensor data.
 *
 *  @param[in] sensor_id : Sensor ID.
 *  @param[in] count     : Number of samples to read.
 *  @param[in] bhy      : Device reference.
 */
static void read_sensor_data(uint8_t sensor_id, uint32_t count, struct bhy_dev *bhy);

/*! @brief Prints API error code.
 *
 *  @param[in] rslt      : API Error code.
 *  @param[in] dev       : Device reference.
 */
static void print_api_error(int8_t rslt, struct bhy_dev *dev);

/*! @brief Loads firmware image to ram.
 *
 *  @param[in] boot_stat : Boot status.
 *  @param[in] dev       : Device reference.
 */
static void upload_firmware(uint8_t boot_stat, struct bhy_dev *dev);

/*! @brief Parse head orientation quaternion data.
 *
 *  @param[in] callback_info : Sensor data info.
 *  @param[in] callback_ref  : Parse reference.
 */
static void parse_ho_quaternion(const struct bhy_fifo_parse_data_info *callback_info,
                                void                                  *callback_ref);

/*! @brief Parse head orientation euler data.
 *
 *  @param[in] callback_info : Sensor data info.
 *  @param[in] callback_ref  : Parse reference.
 */
static void parse_ho_euler(const struct bhy_fifo_parse_data_info *callback_info,
                           void                                  *callback_ref);

/*! @brief Parse meta event.
 *
 *  @param[in] callback_info : Sensor data info.
 *  @param[in] callback_ref  : Parse reference.
 */
static void parse_meta_event(const struct bhy_fifo_parse_data_info *callback_info,
                             void                                  *callback_ref);

/*! @brief Converts time ticks to seconds and nanoseconds.
 *
 *  @param[in]  time_ticks : Time ticks.
 *  @param[out] s          : Seconds.
 *  @param[out] ns         : Nanoseconds.
 *  @param[out] tns        : Total nanoseconds.
 */
static void time_to_s_ns(uint64_t time_ticks, uint32_t *s, uint32_t *ns, uint64_t *tns);

enum bhy_intf intf;

int
main (void)
{
    uint8_t        chip_id = 0;
    uint16_t       version = 0;
    int8_t         rslt;
    struct bhy_dev bhy;
    uint8_t        hif_ctrl, boot_status;
    uint8_t
        accuracy; /* Accuracy is reported as a meta event. It is being printed alongside the data */
    uint8_t                                           work_buffer[WORK_BUFFER_SIZE];
    struct bhy_virtual_sensor_conf_param_conf         sensor_conf                = { 0 };
    bhy_head_orientation_param_ver                    hmc_ver                    = { 0 };
    bhy_head_orientation_param_ver                    ho_version                 = { 0 };
    bhy_head_orientation_param_misalignment_config    misalign_config            = { 0 };
    uint8_t                                           ho_quat_head_corr_state[4] = { 0 };
    uint8_t                                           ho_eul_head_corr_state[4]  = { 0 };
    bhy_head_orientation_param_misalignment_quat_corr hmc_quat_corr              = { { 0 } };
    bhy_head_misalignment_mode_vector_x               hmc_mode_vect_x            = { 0 };

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
    rslt = bhy_get_host_interrupt_ctrl(&hif_ctrl, &bhy);
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

        rslt = bhy_register_fifo_parse_callback(
            BHY_SYS_ID_META_EVENT, parse_meta_event, (void *)&accuracy, &bhy);
        print_api_error(rslt, &bhy);
        rslt = bhy_register_fifo_parse_callback(
            BHY_SYS_ID_META_EVENT_WU, parse_meta_event, (void *)&accuracy, &bhy);
        print_api_error(rslt, &bhy);
        rslt = bhy_register_fifo_parse_callback(
            BHY_SENSOR_ID_IMU_HEAD_ORI_Q, parse_ho_quaternion, (void *)&accuracy, &bhy);
        print_api_error(rslt, &bhy);
        rslt = bhy_register_fifo_parse_callback(
            BHY_SENSOR_ID_IMU_HEAD_ORI_E, parse_ho_euler, (void *)&accuracy, &bhy);
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

    hmc_mode_vect_x.mode             = 0;
    hmc_mode_vect_x.vector_x_0.f_val = 0.0;
    hmc_mode_vect_x.vector_x_1.f_val = 0.0;
    hmc_mode_vect_x.vector_x_2.f_val = 1.0;

    rslt = bhy_head_orientation_param_set_hmc_mode_vector_x(&hmc_mode_vect_x, &bhy);
    print_api_error(rslt, &bhy);

    rslt = bhy_head_orientation_param_get_hmc_mode_vector_x(&hmc_mode_vect_x, &bhy);
    print_api_error(rslt, &bhy);

    printf("HMC Mode: 0x%02x, Vector X: %f, Vector Y: %f, Vector Z: %f \r\n",
           hmc_mode_vect_x.mode,
           hmc_mode_vect_x.vector_x_0.f_val,
           hmc_mode_vect_x.vector_x_1.f_val,
           hmc_mode_vect_x.vector_x_2.f_val);

    rslt = bhy_head_orientation_param_get_hmc_version(&hmc_ver, &bhy);
    print_api_error(rslt, &bhy);

    printf("HMC version: %u.%u.%u\r\n", hmc_ver.major, hmc_ver.minor, hmc_ver.patch);

    rslt = bhy_head_orientation_param_get_ho_version(&ho_version, &bhy);
    print_api_error(rslt, &bhy);

    printf("HO version: %u.%u.%u\r\n", ho_version.major, ho_version.minor, ho_version.patch);

    rslt = bhy_head_orientation_param_set_hmc_configuration(&misalign_config, &bhy);
    print_api_error(rslt, &bhy);

    rslt = bhy_head_orientation_param_get_hmc_configuration(&misalign_config, &bhy);
    print_api_error(rslt, &bhy);

    /*lint -e10 Error 10: Lint does not understand PRIxxx */
    printf(
        "Misalignment config: still_phase_max_dur: %u, still_phase_min_dur: %u, "
        "still_phase_max_samples: %u, acc_diff_threshold: %" PRId32 "\r\n",
        misalign_config.still_phase_max_dur,
        misalign_config.still_phase_min_dur,
        misalign_config.still_phase_max_samples,
        misalign_config.acc_diff_threshold);

    /*lint +e10 */

    /* Enabling head orientation misallignement virtual sensor */
    sensor_conf.sample_rate = 25.0;
    sensor_conf.latency     = 0; /* Report immediately */
    rslt
        = bhy_virtual_sensor_conf_param_set_cfg(BHY_SENSOR_ID_HEAD_ORI_MIS_ALG, &sensor_conf, &bhy);
    print_api_error(rslt, &bhy);
    printf("Enable %s at %.2fHz.\r\n",
           get_sensor_name(BHY_SENSOR_ID_HEAD_ORI_MIS_ALG),
           sensor_conf.sample_rate);

    /* Disabling head orientation misallignement virtual sensor */
    sensor_conf.sample_rate = 0.0;
    rslt
        = bhy_virtual_sensor_conf_param_set_cfg(BHY_SENSOR_ID_HEAD_ORI_MIS_ALG, &sensor_conf, &bhy);
    print_api_error(rslt, &bhy);
    printf("Disable %s at %.2fHz.\r\n",
           get_sensor_name(BHY_SENSOR_ID_HEAD_ORI_MIS_ALG),
           sensor_conf.sample_rate);

    rslt = bhy_head_orientation_param_trigger_hmc_calibration(&bhy);
    print_api_error(rslt, &bhy);

    rslt = bhy_head_orientation_param_set_default_hmc_cfg(&bhy);
    print_api_error(rslt, &bhy);

    ho_quat_head_corr_state[0] = 1;

    rslt = bhy_head_orientation_param_set_quat_init_head_corr(ho_quat_head_corr_state, &bhy);
    print_api_error(rslt, &bhy);

    rslt = bhy_head_orientation_param_get_quat_init_head_corr(ho_quat_head_corr_state, &bhy);
    print_api_error(rslt, &bhy);

    printf("\n");
    printf("Quaternion Initial Heading Correction Status : %s\r\n",
           (ho_quat_head_corr_state[0] == BHY_HEAD_ORIENTATION_PARAM_QUAT_INITIAL_HEAD_CORR_ENABLE)
               ? "Enabled"
               : "Disabled");

    ho_eul_head_corr_state[0] = 1;

    rslt = bhy_head_orientation_param_set_eul_init_head_corr(ho_eul_head_corr_state, &bhy);
    print_api_error(rslt, &bhy);

    rslt = bhy_head_orientation_param_get_eul_init_head_corr(ho_eul_head_corr_state, &bhy);
    print_api_error(rslt, &bhy);

    printf("Euler Initial Heading Correction Status : %s\r\n",
           (ho_eul_head_corr_state[0] == BHY_HEAD_ORIENTATION_PARAM_EUL_INITIAL_HEAD_CORR_ENABLE)
               ? "Enabled"
               : "Disabled");

    hmc_quat_corr.quaternion_x.f_val = 0.0;
    hmc_quat_corr.quaternion_y.f_val = 0.0;
    hmc_quat_corr.quaternion_z.f_val = 0.0;
    hmc_quat_corr.quaternion_w.f_val = 1.0;
    hmc_quat_corr.accuracy.f_val     = 0.0;

    rslt = bhy_head_orientation_param_set_hmc_quat_cal_cor_cfg(&hmc_quat_corr, &bhy);
    print_api_error(rslt, &bhy);

    rslt = bhy_head_orientation_param_get_hmc_quat_cal_cor_cfg(&hmc_quat_corr, &bhy);
    print_api_error(rslt, &bhy);

    printf("quaternion_x : %f\r\n", hmc_quat_corr.quaternion_x.f_val);
    printf("quaternion_y : %f\r\n", hmc_quat_corr.quaternion_y.f_val);
    printf("quaternion_z : %f\r\n", hmc_quat_corr.quaternion_z.f_val);
    printf("quaternion_w : %f\r\n", hmc_quat_corr.quaternion_w.f_val);

    /* Enabling head orientation quaternion virtual sensor */
    sensor_conf.sample_rate = 25.0;
    sensor_conf.latency     = 0; /* Report immediately */
    rslt = bhy_virtual_sensor_conf_param_set_cfg(BHY_SENSOR_ID_IMU_HEAD_ORI_Q, &sensor_conf, &bhy);
    print_api_error(rslt, &bhy);
    printf("Enable %s at %.2fHz.\r\n",
           get_sensor_name(BHY_SENSOR_ID_IMU_HEAD_ORI_Q),
           sensor_conf.sample_rate);

    read_sensor_data(BHY_SENSOR_ID_IMU_HEAD_ORI_Q, PARSE_DATA_WINDOW_SIZE, &bhy);

    /* Disabling head orientation quaternion virtual sensor */
    sensor_conf.sample_rate = 0.0;
    rslt = bhy_virtual_sensor_conf_param_set_cfg(BHY_SENSOR_ID_IMU_HEAD_ORI_Q, &sensor_conf, &bhy);
    print_api_error(rslt, &bhy);
    printf("Disable %s at %.2fHz.\r\n",
           get_sensor_name(BHY_SENSOR_ID_IMU_HEAD_ORI_Q),
           sensor_conf.sample_rate);

    /* Enabling head orientation euler virtual sensor */
    sensor_conf.sample_rate = 25.0;
    sensor_conf.latency     = 0; /* Report immediately */

    rslt = bhy_virtual_sensor_conf_param_set_cfg(BHY_SENSOR_ID_IMU_HEAD_ORI_E, &sensor_conf, &bhy);
    print_api_error(rslt, &bhy);
    printf("Enable %s at %.2fHz.\r\n",
           get_sensor_name(BHY_SENSOR_ID_IMU_HEAD_ORI_E),
           sensor_conf.sample_rate);

    read_sensor_data(BHY_SENSOR_ID_IMU_HEAD_ORI_E, PARSE_DATA_WINDOW_SIZE, &bhy);

    /* Disabling head orientation euler virtual sensor */
    sensor_conf.sample_rate = 0.0;
    rslt = bhy_virtual_sensor_conf_param_set_cfg(BHY_SENSOR_ID_IMU_HEAD_ORI_E, &sensor_conf, &bhy);
    print_api_error(rslt, &bhy);
    printf("Disable %s at %.2fHz.\r\n",
           get_sensor_name(BHY_SENSOR_ID_IMU_HEAD_ORI_E),
           sensor_conf.sample_rate);

    close_interfaces(intf);

    return rslt;
}

static void
parse_ho_quaternion (const struct bhy_fifo_parse_data_info *callback_info, void *callback_ref)
{
    bhy_event_data_head_orientation_quat data;
    uint32_t                             s, ns;
    uint64_t                             tns;

    if (!callback_info)
    {
        printf("Null reference\r\n");

        return;
    }

    bhy_event_data_head_orientation_quat_parsing(callback_info->data_ptr, &data);

    time_to_s_ns(*callback_info->time_stamp, &s, &ns, &tns);

    /*lint -e10 Error 10: Lint does not understand PRIxxx */
    printf("SID: %u; T: %" PRIu32 ".%09" PRIu32 "; x: %f, y: %f, z: %f, w: %f\r\n",
           callback_info->sensor_id,
           s,
           ns,
           data.x / 16384.0f,
           data.y / 16384.0f,
           data.z / 16384.0f,
           data.w / 16384.0f);

    /*lint +e10 */
}

static void
parse_ho_euler (const struct bhy_fifo_parse_data_info *callback_info, void *callback_ref)
{
    bhy_event_data_head_orientation_eul data;
    uint32_t                            s, ns;
    uint64_t                            tns;

    if (!callback_info)
    {
        printf("Null reference\r\n");

        return;
    }

    bhy_event_data_head_orientation_eul_parsing(callback_info->data_ptr, &data);

    time_to_s_ns(*callback_info->time_stamp, &s, &ns, &tns);

    /*lint -e10 Error 10: Lint does not understand PRIxxx */
    printf("SID: %u; T: %" PRIu32 ".%09" PRIu32 "; h: %f, p: %f, r: %f\r\n",
           callback_info->sensor_id,
           s,
           ns,
           (data.heading * 360.0f) / 32768.0f,
           (data.pitch * 360.0f) / 32768.0f,
           (data.roll * 360.0f) / 32768.0f);

    /*lint +e10 */
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

static void
read_sensor_data (uint8_t sensor_id, uint32_t count, struct bhy_dev *bhy)
{
    int8_t   rslt;
    uint32_t curr_ts;
    uint8_t  work_buffer[WORK_BUFFER_SIZE];
    uint32_t start_ts = coines_get_millis();

    do
    {
        curr_ts = coines_get_millis();
        if (get_interrupt_status())
        {
            /* Data from the FIFO is read and the relevant callbacks if registered are called */
            rslt = bhy_get_and_process_fifo(work_buffer, WORK_BUFFER_SIZE, bhy);
            print_api_error(rslt, bhy);
        }
    } while ((curr_ts - start_ts) < count);
}

static void
time_to_s_ns (uint64_t time_ticks, uint32_t *s, uint32_t *ns, uint64_t *tns)
{
    *tns = time_ticks * 15625; /* timestamp is now in nanoseconds */
    *s   = (uint32_t)(*tns / UINT64_C(1000000000));
    *ns  = (uint32_t)(*tns - ((*s) * UINT64_C(1000000000)));
}
