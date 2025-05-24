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
 * @file    bhy_phy_sensor_ctrl_param.c
 * @brief   Example to set/get physical sensor control parameters
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "bhy.h"
#include "bhy_parse.h"
#include "common.h"
#include "bhy_phy_sensor_ctrl_param.h"
#include "bhy_param_defs.h"

#include "bhi360/Bosch_Shuttle3_BHI360_BMM350C_BMP580_BME688.fw.h"

static void   print_api_error(int8_t rslt, struct bhy_dev *dev);
static int8_t upload_firmware(struct bhy_dev *dev);
static void   run_phy_accel_sample(struct bhy_dev *dev);
static void   run_phy_gyro_sample(struct bhy_dev *dev);
static void   run_phy_magnet_sample(struct bhy_dev *dev);
static void   run_phy_wrist_wear_wakeup_sample(struct bhy_dev *dev);
static void   run_phy_any_motion_sample(struct bhy_dev *dev);
static void   run_phy_no_motion_sample(struct bhy_dev *dev);
static void   run_phy_wrist_gesture_detector_sample(struct bhy_dev *dev);
static void   run_phy_baro_type_1_sample(struct bhy_dev *dev);
static void   run_phy_baro_type_2_sample(struct bhy_dev *dev);
static void   run_phy_step_counter_sample(struct bhy_dev *dev);

enum bhy_intf intf;

int
main (void)
{
    uint8_t        chip_id = 0;
    uint16_t       version = 0;
    int8_t         rslt;
    struct bhy_dev bhy;
    uint8_t        hintr_ctrl, hif_ctrl, boot_status;

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
        uint8_t sensor_error;
        int8_t  temp_rslt;
        printf("Loading firmware.\r\n");

        rslt      = upload_firmware(&bhy);
        temp_rslt = bhy_get_error_value(&sensor_error, &bhy);
        if (sensor_error)
        {
            printf("%s\r\n", get_sensor_error_text(sensor_error));
        }

        print_api_error(rslt, &bhy);
        print_api_error(temp_rslt, &bhy);

        printf("Booting from RAM.\r\n");
        rslt = bhy_boot_from_ram(&bhy);

        temp_rslt = bhy_get_error_value(&sensor_error, &bhy);
        if (sensor_error)
        {
            printf("%s\r\n", get_sensor_error_text(sensor_error));
        }

        print_api_error(rslt, &bhy);
        print_api_error(temp_rslt, &bhy);

        rslt = bhy_get_kernel_version(&version, &bhy);
        print_api_error(rslt, &bhy);
        if ((rslt == BHY_OK) && (version != 0))
        {
            printf("Boot successful. Kernel version %u.\r\n", version);
        }

        run_phy_accel_sample(&bhy);
        run_phy_gyro_sample(&bhy);
        run_phy_magnet_sample(&bhy);
        run_phy_wrist_wear_wakeup_sample(&bhy);
        run_phy_any_motion_sample(&bhy);
        run_phy_no_motion_sample(&bhy);
        run_phy_wrist_gesture_detector_sample(&bhy);
        run_phy_baro_type_1_sample(&bhy);
        run_phy_baro_type_2_sample(&bhy);
        run_phy_step_counter_sample(&bhy);
    }
    else
    {
        printf("Host interface not ready. Exiting\r\n");

        close_interfaces(intf);

        return 0;
    }

    close_interfaces(intf);

    return rslt;
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

static int8_t
upload_firmware (struct bhy_dev *dev)
{
    uint32_t incr = 256; /* Max command packet size */
    uint32_t len  = sizeof(bhy_firmware_image);
    int8_t   rslt = BHY_OK;

    if ((incr % 4) != 0) /* Round off to higher 4 bytes */
    {
        incr = ((incr >> 2) + 1) << 2;
    }

    for (uint32_t i = 0; (i < len) && (rslt == BHY_OK); i += incr)
    {
        if (incr > (len - i)) /* If last payload */
        {
            incr = len - i;
            if ((incr % 4) != 0) /* Round off to higher 4 bytes */
            {
                incr = ((incr >> 2) + 1) << 2;
            }
        }

        rslt = bhy_upload_firmware_to_ram_partly(&bhy_firmware_image[i], len, i, incr, dev);

        printf("%.2f%% complete\r", (float)(i + incr) / (float)len * 100.0f);
    }

    printf("\n");

    return rslt;
}

static void
run_phy_accel_sample (struct bhy_dev *dev)
{
    int8_t                                            rslt;
    uint8_t                                           mode;
    uint8_t                                           status;
    bhy_phy_sensor_ctrl_param_accel_fast_offset_calib calib;
    bhy_phy_sensor_ctrl_param_accel_axis_remap        remap;

    printf("Get accelerometer calibration.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_accel_get_foc_calibration(&calib, dev);
    print_api_error(rslt, dev);
    printf("    Accelerometer calibration X-axis: %d\r\n", calib.x_offset);
    printf("    Accelerometer calibration Y-axis: %d\r\n", calib.y_offset);
    printf("    Accelerometer calibration Z-axis: %d\r\n", calib.z_offset);

    printf("Get accelerometer power mode.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_accel_get_power_mode(&mode, dev);
    print_api_error(rslt, dev);
    printf("    Accelerometer power mode: %d\r\n", mode);
    printf("Change accelerometer power mode.\r\n");
    mode = 2;
    rslt = bhy_phy_sensor_ctrl_param_accel_set_power_mode(mode, dev);
    print_api_error(rslt, dev);
    printf("Get accelerometer power mode again.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_accel_get_power_mode(&mode, dev);
    print_api_error(rslt, dev);
    printf("    Accelerometer power mode: %d\r\n", mode);

    printf("Get accelerometer axis remapping.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_accel_get_axis_remapping(&remap, dev);
    print_api_error(rslt, dev);
    printf("    Accelerometer remapping X-axis: %d\r\n", remap.map_x_axis);
    printf("    Accelerometer remapping X-axis sign: %d\r\n", remap.map_x_axis_sign);
    printf("    Accelerometer remapping Y-axis: %d\r\n", remap.map_y_axis);
    printf("    Accelerometer remapping Y-axis sign: %d\r\n", remap.map_y_axis_sign);
    printf("    Accelerometer remapping Z-axis: %d\r\n", remap.map_z_axis);
    printf("    Accelerometer remapping Z-axis sign: %d\r\n", remap.map_z_axis_sign);
    printf("Change accelerometer axis remapping.\r\n");
    remap.map_x_axis      = 1;
    remap.map_x_axis_sign = 1;
    remap.map_y_axis      = 1;
    remap.map_y_axis_sign = 1;
    remap.map_z_axis      = 1;
    remap.map_z_axis_sign = 1;
    rslt                  = bhy_phy_sensor_ctrl_param_accel_set_axis_remapping(&remap, dev);
    print_api_error(rslt, dev);
    printf("Get accelerometer axis remapping again.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_accel_get_axis_remapping(&remap, dev);
    print_api_error(rslt, dev);
    printf("    Accelerometer remapping X-axis: %d\r\n", remap.map_x_axis);
    printf("    Accelerometer remapping X-axis sign: %d\r\n", remap.map_x_axis_sign);
    printf("    Accelerometer remapping Y-axis: %d\r\n", remap.map_y_axis);
    printf("    Accelerometer remapping Y-axis sign: %d\r\n", remap.map_y_axis_sign);
    printf("    Accelerometer remapping Z-axis: %d\r\n", remap.map_z_axis);
    printf("    Accelerometer remapping Z-axis sign: %d\r\n", remap.map_z_axis_sign);

    printf("Get accelerometer NVM writing status.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_accel_get_nvm_status(&status, dev);
    print_api_error(rslt, dev);
    printf("    Accelerometer NVM writing status: %d\r\n", status);
    printf("Trigger a NVM writing for accelerometer.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_accel_trigger_nvm_writing(dev);
    print_api_error(rslt, dev);
    printf("Get accelerometer NVM writing status.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_accel_get_nvm_status(&status, dev);
    print_api_error(rslt, dev);
    printf("    Accelerometer NVM writing status: %d\r\n", status);
}

static void
run_phy_gyro_sample (struct bhy_dev *dev)
{
    int8_t                                           rslt;
    uint8_t                                          mode;
    uint8_t                                          status;
    uint8_t                                          ois_conf;
    uint8_t                                          fast_startup_conf;
    uint8_t                                          auto_trim_conf;
    bhy_phy_sensor_ctrl_param_gyro_fast_offset_calib calib;

    printf("Get gyroscope calibration.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_gyro_get_foc_calibration(&calib, dev);
    print_api_error(rslt, dev);
    printf("    Gyroscope calibration X-axis: %d\r\n", calib.x_offset);
    printf("    Gyroscope calibration Y-axis: %d\r\n", calib.y_offset);
    printf("    Gyroscope calibration Z-axis: %d\r\n", calib.z_offset);

    printf("Get gyroscope OIS configuration.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_gyro_get_ois_config(&ois_conf, dev);
    print_api_error(rslt, dev);
    printf("    Gyroscope OIS configuration: %d\r\n", ois_conf);
    printf("Change gyroscope OIS configuration.\r\n");
    ois_conf = 1;
    rslt     = bhy_phy_sensor_ctrl_param_gyro_set_ois_config(ois_conf, dev);
    print_api_error(rslt, dev);
    printf("Get gyroscope OIS configuration again.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_gyro_get_ois_config(&ois_conf, dev);
    print_api_error(rslt, dev);
    printf("    Gyroscope OIS configuration: %d\r\n", ois_conf);

    printf("Get gyroscope Fast start up configuration.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_gyro_get_fast_startup_cfg(&fast_startup_conf, dev);
    print_api_error(rslt, dev);
    printf("    Gyroscope Fast start up configuration: %d\r\n", fast_startup_conf);
    printf("Change gyroscope Fast start up configuration.\r\n");
    fast_startup_conf = 1;
    rslt              = bhy_phy_sensor_ctrl_param_gyro_set_fast_startup_cfg(fast_startup_conf, dev);
    print_api_error(rslt, dev);
    printf("Get gyroscope Fast start up configuration again.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_gyro_get_fast_startup_cfg(&fast_startup_conf, dev);
    print_api_error(rslt, dev);
    printf("    Gyroscope Fast start up configuration: %d\r\n", fast_startup_conf);

    printf("Get gyroscope power mode.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_gyro_get_power_mode(&mode, dev);
    print_api_error(rslt, dev);
    printf("    Gyroscope power mode: %d\r\n", mode);
    printf("Change gyroscope power mode.\r\n");
    mode = 1;
    rslt = bhy_phy_sensor_ctrl_param_gyro_set_power_mode(mode, dev);
    print_api_error(rslt, dev);
    printf("Get gyroscope power mode again.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_gyro_get_power_mode(&mode, dev);
    print_api_error(rslt, dev);
    printf("    Gyroscope power mode: %d\r\n", mode);

    printf("Get gyroscope timer auto trim configuration.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_gyro_get_auto_trim_cfg(&auto_trim_conf, dev);
    print_api_error(rslt, dev);
    printf("    Gyroscope timer auto trim configuration: %d\r\n", auto_trim_conf);
    printf("Change gyroscope timer auto trim configuration.\r\n");
    auto_trim_conf = 1;
    rslt           = bhy_phy_sensor_ctrl_param_gyro_set_auto_trim_cfg(auto_trim_conf, dev);
    print_api_error(rslt, dev);
    printf("Get gyroscope timer auto trim configuration again.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_gyro_get_auto_trim_cfg(&auto_trim_conf, dev);
    print_api_error(rslt, dev);
    printf("    Gyroscope timer auto trim configuration: %d\r\n", auto_trim_conf);

    printf("Get gyroscope NVM writing status.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_gyro_get_nvm_status(&status, dev);
    print_api_error(rslt, dev);
    printf("    Gyroscope NVM writing status: %d\r\n", status);
    printf("Trigger a NVM writing for gyroscope.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_gyro_trigger_nvm_writing(dev);
    print_api_error(rslt, dev);
    printf("Get gyroscope NVM writing status.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_gyro_get_nvm_status(&status, dev);
    print_api_error(rslt, dev);
    printf("    Gyroscope NVM writing status: %d\r\n", status);
}

static void
run_phy_magnet_sample (struct bhy_dev *dev)
{
    int8_t  rslt;
    uint8_t mode;

    printf("Get magnetometer power mode.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_magnet_get_power_mode(&mode, dev);
    print_api_error(rslt, dev);
    printf("    Magnetometer power mode: %d\r\n", mode);
    printf("Change magnetometer power mode.\r\n");
    mode = 1;
    rslt = bhy_phy_sensor_ctrl_param_magnet_set_power_mode(mode, dev);
    print_api_error(rslt, dev);
    printf("Get magnetometer power mode again.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_magnet_get_power_mode(&mode, dev);
    print_api_error(rslt, dev);
    printf("    Magnetometer power mode: %d\r\n", mode);
}

static void
run_phy_wrist_wear_wakeup_sample (struct bhy_dev *dev)
{
    int8_t                                      rslt;
    bhy_phy_sensor_ctrl_param_wrist_wear_wakeup conf;

    printf("Get Wrist Wear Wakeup configuration.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_get_wrist_wear_wakeup_cfg(&conf, dev);
    print_api_error(rslt, dev);
    printf("    Wrist Wear Wakeup configuration:\r\n");
    printf("     - <min_angle_focus>: %u\r\n", conf.min_angle_focus);
    printf("     - <min_angle_non_focus>: %u\r\n", conf.min_angle_non_focus);
    printf("     - <angle_landscape_right>: %u\r\n", conf.angle_landscape_right);
    printf("     - <angle_landscape_left>: %u\r\n", conf.angle_landscape_left);
    printf("     - <angle_portrait_down>: %u\r\n", conf.angle_portrait_down);
    printf("     - <angle_portrait_up>: %u\r\n", conf.angle_portrait_up);
    printf("     - <min_dur_moved>: %u\r\n", conf.min_dur_moved);
    printf("     - <min_dur_quite>: %u\r\n", conf.min_dur_quite);
    printf("Change Wrist Wear Wakeup configuration.\r\n");
    conf.min_angle_focus = 1024;
    rslt                 = bhy_phy_sensor_ctrl_param_set_wrist_wear_wakeup_cfg(&conf, dev);
    print_api_error(rslt, dev);
    printf("Get Wrist Wear Wakeup configuration again.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_get_wrist_wear_wakeup_cfg(&conf, dev);
    print_api_error(rslt, dev);
    printf("    Wrist Wear Wakeup configuration:\r\n");
    printf("     - <min_angle_focus>: %u\r\n", conf.min_angle_focus);
    printf("     - <min_angle_non_focus>: %u\r\n", conf.min_angle_non_focus);
    printf("     - <angle_landscape_right>: %u\r\n", conf.angle_landscape_right);
    printf("     - <angle_landscape_left>: %u\r\n", conf.angle_landscape_left);
    printf("     - <angle_portrait_down>: %u\r\n", conf.angle_portrait_down);
    printf("     - <angle_portrait_up>: %u\r\n", conf.angle_portrait_up);
    printf("     - <min_dur_moved>: %u\r\n", conf.min_dur_moved);
    printf("     - <min_dur_quite>: %u\r\n", conf.min_dur_quite);
}
static void
run_phy_any_motion_sample (struct bhy_dev *dev)
{
    int8_t                               rslt;
    bhy_phy_sensor_ctrl_param_any_motion conf;

    printf("Get Any Motion configuration.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_get_any_motion_config(&conf, dev);
    print_api_error(rslt, dev);
    printf("    Any Motion configuration:\r\n");
    printf("     - <duration>: %u\r\n", conf.duration);
    printf("     - <axis_sel>: %u\r\n", conf.axis_sel);
    printf("     - <threshold>: %u\r\n", conf.threshold);
    printf("Change Any Motion configuration.\r\n");
    conf.duration = 10;
    rslt          = bhy_phy_sensor_ctrl_param_set_any_motion_config(&conf, dev);
    print_api_error(rslt, dev);
    printf("Get Any Motion configuration again.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_get_any_motion_config(&conf, dev);
    print_api_error(rslt, dev);
    printf("    Any Motion configuration:\r\n");
    printf("     - <duration>: %u\r\n", conf.duration);
    printf("     - <axis_sel>: %u\r\n", conf.axis_sel);
    printf("     - <threshold>: %u\r\n", conf.threshold);
}

static void
run_phy_no_motion_sample (struct bhy_dev *dev)
{
    int8_t                              rslt;
    bhy_phy_sensor_ctrl_param_no_motion conf;

    printf("Get No Motion configuration.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_get_no_motion_config(&conf, dev);
    print_api_error(rslt, dev);
    printf("    No Motion configuration:\r\n");
    printf("     - <duration>: %u\r\n", conf.duration);
    printf("     - <axis_sel>: %u\r\n", conf.axis_sel);
    printf("     - <threshold>: %u\r\n", conf.threshold);
    printf("Change No Motion configuration.\r\n");
    conf.duration = 10;
    rslt          = bhy_phy_sensor_ctrl_param_set_no_motion_config(&conf, dev);
    print_api_error(rslt, dev);
    printf("Get No Motion configuration again.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_get_no_motion_config(&conf, dev);
    print_api_error(rslt, dev);
    printf("    No Motion configuration:\r\n");
    printf("     - <duration>: %u\r\n", conf.duration);
    printf("     - <axis_sel>: %u\r\n", conf.axis_sel);
    printf("     - <threshold>: %u\r\n", conf.threshold);
}

static void
run_phy_wrist_gesture_detector_sample (struct bhy_dev *dev)
{
    int8_t                                           rslt;
    bhy_phy_sensor_ctrl_param_wrist_gesture_detector conf;

    printf("Get Wrist Gesture Detector configuration.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_get_wrist_gesture_cfg(&conf, dev);
    print_api_error(rslt, dev);
    printf("    Wrist Gesture Detector configuration:\r\n");
    printf("     - <min_flick_peak_y_thres>: 0x%04x\r\n", conf.min_flick_peak_y_thres);
    printf("     - <min_flick_peak_z_thres>: 0x%04x\r\n", conf.min_flick_peak_z_thres);
    printf("     - <gravity_bounds_x_pos>: 0x%04x\r\n", conf.gravity_bounds_x_pos);
    printf("     - <gravity_bounds_x_neg>: 0x%04x\r\n", conf.gravity_bounds_x_neg);
    printf("     - <gravity_bounds_y_neg>: 0x%04x\r\n", conf.gravity_bounds_y_neg);
    printf("     - <gravity_bounds_z_neg>: 0x%04x\r\n", conf.gravity_bounds_z_neg);
    printf("     - <flick_peak_decay_coeff>: 0x%04x\r\n", conf.flick_peak_decay_coeff);
    printf("     - <lp_mean_filter_coeff>: 0x%04x\r\n", conf.lp_mean_filter_coeff);
    printf("     - <max_duration_jiggle_peaks>: 0x%04x\r\n", conf.max_duration_jiggle_peaks);
    printf("     - <device_pos>: 0x%02x\r\n", conf.device_pos);
    printf("Change Wrist Gesture Detector configuration.\r\n");
    conf.min_flick_peak_y_thres = 0x3E8U;
    rslt                        = bhy_phy_sensor_ctrl_param_set_wrist_gesture_cfg(&conf, dev);
    print_api_error(rslt, dev);
    printf("Get Wrist Gesture Detector configuration again.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_get_wrist_gesture_cfg(&conf, dev);
    print_api_error(rslt, dev);
    printf("    Wrist Gesture Detector configuration:\r\n");
    printf("     - <min_flick_peak_y_thres>: 0x%04x\r\n", conf.min_flick_peak_y_thres);
    printf("     - <min_flick_peak_z_thres>: 0x%04x\r\n", conf.min_flick_peak_z_thres);
    printf("     - <gravity_bounds_x_pos>: 0x%04x\r\n", conf.gravity_bounds_x_pos);
    printf("     - <gravity_bounds_x_neg>: 0x%04x\r\n", conf.gravity_bounds_x_neg);
    printf("     - <gravity_bounds_y_neg>: 0x%04x\r\n", conf.gravity_bounds_y_neg);
    printf("     - <gravity_bounds_z_neg>: 0x%04x\r\n", conf.gravity_bounds_z_neg);
    printf("     - <flick_peak_decay_coeff>: 0x%04x\r\n", conf.flick_peak_decay_coeff);
    printf("     - <lp_mean_filter_coeff>: 0x%04x\r\n", conf.lp_mean_filter_coeff);
    printf("     - <max_duration_jiggle_peaks>: 0x%04x\r\n", conf.max_duration_jiggle_peaks);
    printf("     - <device_pos>: 0x%02x\r\n", conf.device_pos);
}

static void
run_phy_baro_type_1_sample (struct bhy_dev *dev)
{
    int8_t                                rslt;
    bhy_phy_sensor_ctrl_param_baro_type_1 conf;

    printf("Get barometer pressure type 1 configuration.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_baro_get_press_type_1_cfg(&conf, dev);
    print_api_error(rslt, dev);
    printf("    Barometer pressure type 1 configuration:\r\n");
    printf("     - <osr_p>: %u\r\n", conf.osr_p);
    printf("     - <osr_t>: %u\r\n", conf.osr_t);
    printf("     - <iir_filter>: %u\r\n", conf.iir_filter);
    printf("Change barometer pressure type 1 configuration.\r\n");
    conf.osr_p = 1;
    rslt       = bhy_phy_sensor_ctrl_param_baro_set_press_type_1_cfg(&conf, dev);
    print_api_error(rslt, dev);
    printf("Get barometer pressure type 1 configuration again.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_baro_get_press_type_1_cfg(&conf, dev);
    print_api_error(rslt, dev);
    printf("    Barometer pressure type 1 configuration:\r\n");
    printf("     - <osr_p>: %u\r\n", conf.osr_p);
    printf("     - <osr_t>: %u\r\n", conf.osr_t);
    printf("     - <iir_filter>: %u\r\n", conf.iir_filter);
}

static void
run_phy_baro_type_2_sample (struct bhy_dev *dev)
{
    int8_t                                rslt;
    bhy_phy_sensor_ctrl_param_baro_type_2 conf;

    printf("Get barometer pressure type 2 configuration.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_baro_get_press_type_2_cfg(&conf, dev);
    print_api_error(rslt, dev);
    printf("    Barometer pressure type 2 configuration:\r\n");
    printf("     - <osr_p>: %u\r\n", conf.osr_p);
    printf("     - <osr_t>: %u\r\n", conf.osr_t);
    printf("     - <iir_filter_p>: %u\r\n", conf.iir_filter_p);
    printf("     - <iir_filter_t>: %u\r\n", conf.iir_filter_t);
    printf("     - <dsp_config>: %u\r\n", conf.dsp_config);
    printf("Change barometer pressure type 2 configuration.\r\n");
    conf.osr_t = 1;
    rslt       = bhy_phy_sensor_ctrl_param_baro_set_press_type_2_cfg(&conf, dev);
    print_api_error(rslt, dev);
    printf("Get barometer pressure type 2 configuration again.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_baro_get_press_type_2_cfg(&conf, dev);
    print_api_error(rslt, dev);
    printf("    Barometer pressure type 2 configuration:\r\n");
    printf("     - <osr_p>: %u\r\n", conf.osr_p);
    printf("     - <osr_t>: %u\r\n", conf.osr_t);
    printf("     - <iir_filter_p>: %u\r\n", conf.iir_filter_p);
    printf("     - <iir_filter_t>: %u\r\n", conf.iir_filter_t);
    printf("     - <dsp_config>: %u\r\n", conf.dsp_config);
}

static void
run_phy_step_counter_sample (struct bhy_dev *dev)
{
    int8_t                                 rslt;
    bhy_phy_sensor_ctrl_param_step_counter conf;

    printf("Get Step Counter configuration.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_get_step_counter_config(&conf, dev);
    print_api_error(rslt, dev);
    printf("    Step Counter configuration:\r\n");
    printf("     - <env_min_dist_up>: %u\r\n", conf.env_min_dist_up);
    printf("     - <env_coef_up>: %u\r\n", conf.env_coef_up);
    printf("     - <env_min_dist_down>: %u\r\n", conf.env_min_dist_down);
    printf("     - <env_coef_down>: %u\r\n", conf.env_coef_down);
    printf("     - <step_buffer_size>: %u\r\n", conf.step_buffer_size);
    printf("     - <mean_val_decay>: %u\r\n", conf.mean_val_decay);
    printf("     - <mean_step_dur>: %u\r\n", conf.mean_step_dur);
    printf("     - <filter_coeff_b2>: %u\r\n", conf.filter_coeff_b2);
    printf("     - <filter_coeff_b1>: %u\r\n", conf.filter_coeff_b1);
    printf("     - <filter_coeff_b0>: %u\r\n", conf.filter_coeff_b0);
    printf("     - <filter_coeff_a2>: %u\r\n", conf.filter_coeff_a2);
    printf("     - <filter_coeff_a1>: %u\r\n", conf.filter_coeff_a1);
    printf("     - <filter_cascade_enabled>: %u\r\n", conf.filter_cascade_enabled);
    printf("     - <peak_duration_min_walking>: %u\r\n", conf.peak_duration_min_walking);
    printf("     - <peak_duration_min_running>: %u\r\n", conf.peak_duration_min_running);
    printf("     - <step_duration_max>: %u\r\n", conf.step_duration_max);
    printf("     - <step_duration_window>: %u\r\n", conf.step_duration_window);
    printf("     - <half_step_enabled>: %u\r\n", conf.half_step_enabled);
    printf("     - <activity_detection_factor>: %u\r\n", conf.activity_detection_factor);
    printf("     - <activity_detection_thres>: %u\r\n", conf.activity_detection_thres);
    printf("     - <step_counter_increment>: %u\r\n", conf.step_counter_increment);
    printf("     - <step_duration_pp_enabled>: %u\r\n", conf.step_duration_pp_enabled);
    printf("     - <step_dur_thres>: %u\r\n", conf.step_dur_thres);
    printf("     - <en_mcr_pp>: %u\r\n", conf.en_mcr_pp);
    printf("     - <mcr_thres>: %u\r\n", conf.mcr_thres);
    printf("     - <sc_26>: %u\r\n", conf.sc_26);
    printf("     - <sc_27>: %u\r\n", conf.sc_27);
    printf("Change Step Counter configuration.\r\n");
    conf.env_min_dist_up = 10;
    rslt                 = bhy_phy_sensor_ctrl_param_set_step_counter_config(&conf, dev);
    print_api_error(rslt, dev);
    printf("Get Step Counter configuration again.\r\n");
    rslt = bhy_phy_sensor_ctrl_param_get_step_counter_config(&conf, dev);
    print_api_error(rslt, dev);
    printf("    Step Counter configuration:\r\n");
    printf("     - <env_min_dist_up>: %u\r\n", conf.env_min_dist_up);
    printf("     - <env_coef_up>: %u\r\n", conf.env_coef_up);
    printf("     - <env_min_dist_down>: %u\r\n", conf.env_min_dist_down);
    printf("     - <env_coef_down>: %u\r\n", conf.env_coef_down);
    printf("     - <step_buffer_size>: %u\r\n", conf.step_buffer_size);
    printf("     - <mean_val_decay>: %u\r\n", conf.mean_val_decay);
    printf("     - <mean_step_dur>: %u\r\n", conf.mean_step_dur);
    printf("     - <filter_coeff_b2>: %u\r\n", conf.filter_coeff_b2);
    printf("     - <filter_coeff_b1>: %u\r\n", conf.filter_coeff_b1);
    printf("     - <filter_coeff_b0>: %u\r\n", conf.filter_coeff_b0);
    printf("     - <filter_coeff_a2>: %u\r\n", conf.filter_coeff_a2);
    printf("     - <filter_coeff_a1>: %u\r\n", conf.filter_coeff_a1);
    printf("     - <filter_cascade_enabled>: %u\r\n", conf.filter_cascade_enabled);
    printf("     - <peak_duration_min_walking>: %u\r\n", conf.peak_duration_min_walking);
    printf("     - <peak_duration_min_running>: %u\r\n", conf.peak_duration_min_running);
    printf("     - <step_duration_max>: %u\r\n", conf.step_duration_max);
    printf("     - <step_duration_window>: %u\r\n", conf.step_duration_window);
    printf("     - <half_step_enabled>: %u\r\n", conf.half_step_enabled);
    printf("     - <activity_detection_factor>: %u\r\n", conf.activity_detection_factor);
    printf("     - <activity_detection_thres>: %u\r\n", conf.activity_detection_thres);
    printf("     - <step_counter_increment>: %u\r\n", conf.step_counter_increment);
    printf("     - <step_duration_pp_enabled>: %u\r\n", conf.step_duration_pp_enabled);
    printf("     - <step_dur_thres>: %u\r\n", conf.step_dur_thres);
    printf("     - <en_mcr_pp>: %u\r\n", conf.en_mcr_pp);
    printf("     - <mcr_thres>: %u\r\n", conf.mcr_thres);
    printf("     - <sc_26>: %u\r\n", conf.sc_26);
    printf("     - <sc_27>: %u\r\n", conf.sc_27);
}
