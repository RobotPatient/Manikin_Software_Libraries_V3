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
 * @file    activity_param.c
 * @brief   Activity parameters configuration example for the BHI360
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "bhy.h"
#include "bhy_parse.h"
#include "common.h"
#include "bhy_activity_param.h"

#define HEARABLE_ACTIVITY 1

#if HEARABLE_ACTIVITY
#include "bhi360/Bosch_Shuttle3_BHI360_BMM350C_HeadOrientation.fw.h"
#else
#include "bhi360/Bosch_Shuttle3_BHI360.fw.h"
#endif

static void print_api_error(int8_t rslt, struct bhy_dev *dev);
static void upload_firmware(uint8_t boot_stat, struct bhy_dev *dev);

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
        upload_firmware(boot_status, &bhy);

        rslt = bhy_get_kernel_version(&version, &bhy);
        print_api_error(rslt, &bhy);
        if ((rslt == BHY_OK) && (version != 0))
        {
            printf("Boot successful. Kernel version %u.\r\n", version);
        }

#if HEARABLE_ACTIVITY
        bhy_activity_param_hearable set_hearable_cfg = { 0 };
        bhy_activity_param_hearable get_hearable_cfg = { 0 };

        /* get activity parameters of Hearable */
        rslt = bhy_activity_param_get_hearable_config(&get_hearable_cfg, &bhy);
        print_api_error(rslt, &bhy);

        printf("\n");
        printf("Get hearable activity parameters\r\n");
        printf("seg_size: %u\r\n", get_hearable_cfg.seg_size);
        printf("post_process_en: %u\r\n", get_hearable_cfg.post_process_en);
        printf("min_gdi_thre: %u\r\n", get_hearable_cfg.min_gdi_thre);
        printf("max_gdi_thre: %u\r\n", get_hearable_cfg.max_gdi_thre);
        printf("out_buff_size: %u\r\n", get_hearable_cfg.out_buff_size);
        printf("min_seg_moder_confg: %u\r\n", get_hearable_cfg.min_seg_moder_conf);

        set_hearable_cfg.seg_size           = 1;
        set_hearable_cfg.post_process_en    = 1;
        set_hearable_cfg.min_gdi_thre       = 4095;
        set_hearable_cfg.max_gdi_thre       = 4095;
        set_hearable_cfg.out_buff_size      = 10;
        set_hearable_cfg.min_seg_moder_conf = 10;

        /* set activity parameters for Hearable */
        rslt = bhy_activity_param_set_hearable_config(&set_hearable_cfg, &bhy);
        print_api_error(rslt, &bhy);
        printf("\n");
        printf("Set hearable activity parameters\r\n");
        printf("seg_size: %u\r\n", set_hearable_cfg.seg_size);
        printf("post_process_en: %u\r\n", set_hearable_cfg.post_process_en);
        printf("min_gdi_thre: %u\r\n", set_hearable_cfg.min_gdi_thre);
        printf("max_gdi_thre: %u\r\n", set_hearable_cfg.max_gdi_thre);
        printf("out_buff_size: %u\r\n", set_hearable_cfg.out_buff_size);
        printf("min_seg_moder_confg: %u\r\n", set_hearable_cfg.min_seg_moder_conf);

        /* get activity parameters of Hearable */
        rslt = bhy_activity_param_get_hearable_config(&get_hearable_cfg, &bhy);
        print_api_error(rslt, &bhy);

        printf("\n");
        printf("Get hearable activity parameters\r\n");
        printf("seg_size: %u\r\n", get_hearable_cfg.seg_size);
        printf("post_process_en: %u\r\n", get_hearable_cfg.post_process_en);
        printf("min_gdi_thre: %u\r\n", get_hearable_cfg.min_gdi_thre);
        printf("max_gdi_thre: %u\r\n", get_hearable_cfg.max_gdi_thre);
        printf("out_buff_size: %u\r\n", get_hearable_cfg.out_buff_size);
        printf("min_seg_moder_confg: %u\r\n", get_hearable_cfg.min_seg_moder_conf);
#else
        bhy_activity_param_wearable set_wearable_cfg = { 0 };
        bhy_activity_param_wearable get_wearable_cfg = { 0 };

        /* get activity parameters of Wearable */
        rslt = bhy_activity_param_get_wearable_config(&get_wearable_cfg, &bhy);
        print_api_error(rslt, &bhy);

        printf("\n");
        printf("Get wearable activity parameters\r\n");
        printf("post_process_en: %u\r\n", get_wearable_cfg.post_process_en);
        printf("min_gdi_thre: %u\r\n", get_wearable_cfg.min_gdi_thre);
        printf("max_gdi_thre: %u\r\n", get_wearable_cfg.max_gdi_thre);
        printf("out_buff_size: %u\r\n", get_wearable_cfg.out_buff_size);
        printf("min_seg_moder_confg: %u\r\n", get_wearable_cfg.min_seg_moder_conf);

        set_wearable_cfg.post_process_en    = 1;
        set_wearable_cfg.min_gdi_thre       = 4095;
        set_wearable_cfg.max_gdi_thre       = 4095;
        set_wearable_cfg.out_buff_size      = 10;
        set_wearable_cfg.min_seg_moder_conf = 10;

        /* set activity parameters for Wearable */
        rslt = bhy_activity_param_set_wearable_config(&set_wearable_cfg, &bhy);
        print_api_error(rslt, &bhy);

        printf("\n");
        printf("Set wearable activity parameters\r\n");
        printf("post_process_en: %u\r\n", set_wearable_cfg.post_process_en);
        printf("min_gdi_thre: %u\r\n", set_wearable_cfg.min_gdi_thre);
        printf("max_gdi_thre: %u\r\n", set_wearable_cfg.max_gdi_thre);
        printf("out_buff_size: %u\r\n", set_wearable_cfg.out_buff_size);
        printf("min_seg_moder_confg: %u\r\n", set_wearable_cfg.min_seg_moder_conf);

        /* get activity parameters of Wearable */
        rslt = bhy_activity_param_get_wearable_config(&get_wearable_cfg, &bhy);
        print_api_error(rslt, &bhy);

        printf("\n");
        printf("Get wearable activity parameters\r\n");
        printf("post_process_en: %u\r\n", get_wearable_cfg.post_process_en);
        printf("min_gdi_thre: %u\r\n", get_wearable_cfg.min_gdi_thre);
        printf("max_gdi_thre: %u\r\n", get_wearable_cfg.max_gdi_thre);
        printf("out_buff_size: %u\r\n", get_wearable_cfg.out_buff_size);
        printf("min_seg_moder_confg: %u\r\n", get_wearable_cfg.min_seg_moder_conf);
#endif
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
