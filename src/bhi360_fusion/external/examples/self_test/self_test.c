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
 * @file    self_test.c
 * @brief   Example for self-test
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "bhy.h"
#include "bhy_parse.h"
#include "common.h"

#include "bhi360/Bosch_Shuttle3_BHI360_BMM350C_BMP580_BME688.fw.h"

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

/*! @brief Prints self test response.
 *
 *  @param[in] self_test_resp : Self test response.
 *  @param[in] dev            : Device reference.
 */
static void print_self_test_resp(struct bhy_self_test_resp *self_test_resp, struct bhy_dev *dev);

enum bhy_intf intf;

int
main (void)
{
    uint8_t                   chip_id = 0;
    uint16_t                  version = 0;
    int8_t                    rslt;
    struct bhy_dev            bhy;
    uint8_t                   hif_ctrl, boot_status, hintr_ctrl;
    struct bhy_self_test_resp self_test_resp;

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
    hintr_ctrl = 0;
    rslt       = bhy_get_host_interrupt_ctrl(&hintr_ctrl, &bhy);
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
    }
    else
    {
        printf("Host interface not ready. Exiting\r\n");

        close_interfaces(intf);

        return 0;
    }

    printf("1. Self test pass case:\r\n");
    printf("Self test in progress\r\n");
    rslt = bhy_perform_self_test(BHY_PHYS_SENSOR_ID_ACCELEROMETER, &self_test_resp, &bhy);
    print_api_error(rslt, &bhy);

    print_self_test_resp(&self_test_resp, &bhy);

    printf("2. Self test failure case: Physical sensor ID not supported\r\n");
    printf("Self test in progress\r\n");

    /*! Clear the self test response */
    memset(&self_test_resp, 0, sizeof(struct bhy_self_test_resp));
    rslt = bhy_perform_self_test(BHY_PHYS_SENSOR_ID_NOT_SUPPORTED, &self_test_resp, &bhy);
    print_api_error(rslt, &bhy);

    print_self_test_resp(&self_test_resp, &bhy);

    close_interfaces(intf);

    return rslt;
}

static void
print_self_test_resp (struct bhy_self_test_resp *self_test_resp, struct bhy_dev *dev)
{

    int8_t rslt = BHY_OK;

    if ((dev == NULL) || (self_test_resp == NULL))
    {
        rslt = BHY_E_NULL_PTR;
    }
    else
    {
        switch (self_test_resp->test_status)
        {
            case 0:
                printf("Test passed\r\n");
                break;
            case 1:
                printf("X axis failed\r\n");
                break;
            case 2:
                printf("Y axis failed\r\n");
                break;
            case 4:
                printf("Z axis failed\r\n");
                break;
            case 7:
                printf("Multiple axis failure / single test failed\r\n");
                break;
            case 8:
                printf("Physical sensor ID doesn't support self test\r\n");
                break;
            case 9:
                printf("Physical sensor ID not supported\r\n");
                break;
            default:
                printf("Undefined self test status %u", self_test_resp->test_status);
                break;
        }
        printf("Self test offset, X: %d, Y: %d, Z %d\r\n",
               self_test_resp->x_offset,
               self_test_resp->y_offset,
               self_test_resp->z_offset);
    }

    print_api_error(rslt, dev);
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
