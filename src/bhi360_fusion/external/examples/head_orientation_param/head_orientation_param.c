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
 * @file    head_orientation_param.c
 * @brief   Head orientation parameters configuarion example for the BHY
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

#include "bhy.h"
#include "bhy_parse.h"
#include "common.h"
#include "bhy_virtual_sensor_conf_param.h"
#include "bhy_head_orientation_param.h"

#include "bhi360/Bosch_Shuttle3_BHI360_IMU_HeadOrientation.fw.h"

static void print_api_error(int8_t rslt, struct bhy_dev *dev);
static void upload_firmware(uint8_t boot_stat, struct bhy_dev *dev);

enum bhy_intf intf;

int
main (void)
{
    uint8_t                                           chip_id = 0;
    uint16_t                                          version = 0;
    int8_t                                            rslt;
    struct bhy_dev                                    bhy;
    uint8_t                                           hintr_ctrl, hif_ctrl, boot_status;
    struct bhy_virtual_sensor_conf_param_conf         sensor_conf                = { 0 };
    bhy_head_orientation_param_misalignment_config    hmc_config                 = { 0 };
    bhy_head_orientation_param_ver                    hmc_version                = { 0 };
    bhy_head_orientation_param_misalignment_quat_corr config                     = { { 0 } };
    uint8_t                                           ho_quat_head_corr_state[4] = { 0 };
    bhy_head_orientation_param_ver                    ho_version                 = { 0 };
    uint8_t                                           ho_eul_head_corr_state[4]  = { 0 };

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
        upload_firmware(boot_status, &bhy);

        rslt = bhy_get_kernel_version(&version, &bhy);
        print_api_error(rslt, &bhy);
        if ((rslt == BHY_OK) && (version != 0))
        {
            printf("Boot successful. Kernel version %u.\r\n", version);
        }

        /* Enabling head orientation misallignement virtual sensor */
        sensor_conf.sample_rate = 25.0;
        sensor_conf.latency     = 0; /* Report immediately */
        rslt                    = bhy_virtual_sensor_conf_param_set_cfg(
            BHY_SENSOR_ID_HEAD_ORI_MIS_ALG, &sensor_conf, &bhy);
        print_api_error(rslt, &bhy);
        printf("Enable %s at %.2fHz.\r\n",
               get_sensor_name(BHY_SENSOR_ID_HEAD_ORI_MIS_ALG),
               sensor_conf.sample_rate);

        /* Disabling head orientation misallignement virtual sensor */
        sensor_conf.sample_rate = 0.0;
        rslt                    = bhy_virtual_sensor_conf_param_set_cfg(
            BHY_SENSOR_ID_HEAD_ORI_MIS_ALG, &sensor_conf, &bhy);
        print_api_error(rslt, &bhy);
        printf("Disable %s at %.2fHz.\r\n",
               get_sensor_name(BHY_SENSOR_ID_HEAD_ORI_MIS_ALG),
               sensor_conf.sample_rate);

        rslt = bhy_head_orientation_param_trigger_hmc_calibration(&bhy);
        print_api_error(rslt, &bhy);

        hmc_config.still_phase_max_dur     = 0x06;
        hmc_config.still_phase_min_dur     = 0x02;
        hmc_config.still_phase_max_samples = 0x32;
        hmc_config.acc_diff_threshold      = 0x00002042;

        rslt = bhy_head_orientation_param_set_hmc_configuration(&hmc_config, &bhy);
        print_api_error(rslt, &bhy);
        printf("\n");
        printf("Head Misalignment Configuration set successfully \r\n");

        rslt = bhy_head_orientation_param_get_hmc_configuration(&hmc_config, &bhy);
        print_api_error(rslt, &bhy);

        printf("\n");
        printf("Head Misalignment Configuration\r\n");

        printf("    Still phase max duration 0x%x\r\n", hmc_config.still_phase_max_dur);
        printf("    Still phase min duration 0x%x\r\n", hmc_config.still_phase_min_dur);
        printf("    Still phase max samples 0x%x\r\n", hmc_config.still_phase_max_samples);

        /*lint -e10 Error 10: Lint does not understand PRIxxx */
        printf("    Accelerometer difference threshold %" PRIi32 "\r\n",
               hmc_config.acc_diff_threshold);

        /*lint +e10 */

        rslt = bhy_head_orientation_param_set_default_hmc_cfg(&bhy);
        print_api_error(rslt, &bhy);
        printf("Set Default Head Misalignment Calibration successfully\r\n");

        rslt = bhy_head_orientation_param_get_hmc_version(&hmc_version, &bhy);
        print_api_error(rslt, &bhy);

        config.quaternion_x.f_val = 0.0;
        config.quaternion_y.f_val = 0.0;
        config.quaternion_z.f_val = 0.0;
        config.quaternion_w.f_val = 1.0;
        config.accuracy.f_val     = 0.0;

        rslt = bhy_head_orientation_param_set_hmc_quat_cal_cor_cfg(&config, &bhy);
        print_api_error(rslt, &bhy);
        printf("\n");
        printf("Head Misalignment Quaternion Calibration Correction set successfully\r\n");

        rslt = bhy_head_orientation_param_get_hmc_quat_cal_cor_cfg(&config, &bhy);
        print_api_error(rslt, &bhy);

        printf("\n");
        printf("Head Misalignment Quaternion Correction Configuration\r\n");

        printf("quaternion_x : %f\r\n", config.quaternion_x.f_val);
        printf("quaternion_y : %f\r\n", config.quaternion_y.f_val);
        printf("quaternion_z : %f\r\n", config.quaternion_z.f_val);
        printf("quaternion_w : %f\r\n", config.quaternion_w.f_val);

        ho_quat_head_corr_state[0] = 1;

        rslt = bhy_head_orientation_param_set_quat_init_head_corr(ho_quat_head_corr_state, &bhy);
        print_api_error(rslt, &bhy);

        rslt = bhy_head_orientation_param_get_quat_init_head_corr(ho_quat_head_corr_state, &bhy);
        print_api_error(rslt, &bhy);

        printf("\n");
        printf(
            "Quaternion Initial Heading Correction Status : %s\r\n",
            (ho_quat_head_corr_state[0] == BHY_HEAD_ORIENTATION_PARAM_QUAT_INITIAL_HEAD_CORR_ENABLE)
                ? "Enabled"
                : "Disabled");

        rslt = bhy_head_orientation_param_get_ho_version(&ho_version, &bhy);
        print_api_error(rslt, &bhy);

        printf("\n");
        printf("IMU/NDOF Head Orientation version: %u.%u.%u\r\n\r\n",
               ho_version.major,
               ho_version.minor,
               ho_version.patch);

        ho_eul_head_corr_state[0] = 1;
        rslt = bhy_head_orientation_param_set_eul_init_head_corr(ho_eul_head_corr_state, &bhy);
        print_api_error(rslt, &bhy);

        rslt = bhy_head_orientation_param_get_eul_init_head_corr(ho_eul_head_corr_state, &bhy);
        print_api_error(rslt, &bhy);

        printf(
            "Euler Initial Heading Correction Status : %s\r\n",
            (ho_eul_head_corr_state[0] == BHY_HEAD_ORIENTATION_PARAM_EUL_INITIAL_HEAD_CORR_ENABLE)
                ? "Enabled"
                : "Disabled");
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

static void
upload_firmware (uint8_t boot_stat, struct bhy_dev *dev)
{
    uint8_t sensor_error;
    int8_t  temp_rslt;
    int8_t  rslt = BHY_OK;

    printf("Loading firmware into RAM.\r\n");
    rslt = bhy_upload_firmware_to_ram(bhy_firmware_image, sizeof(bhy_firmware_image), dev);

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
