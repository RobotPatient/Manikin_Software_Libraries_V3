/**
 * @file            vl53l4cd.c
 * @brief           Driver implementation for STM VL53L4CD ranging ToF sensor
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
 * Author:          Johan Korten
 */

#include "vl53l4cd.h"

#include "common/manikin_bit_manipulation.h"
#include "i2c/i2c.h"
#include "private/vl53l4cd_regs.h"
#include "error_handler/error_handler.h"

#define HASH_VL53L4CD 0xE2B0299Eu

/* Initialize the sensor with these register settings */
static const manikin_sensor_reg_t vl53l4cd_init_regs[] = {
    /* System config */
    {0x2d, 0x12, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x2e, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x2f, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x30, 0x11, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x31, 0x02, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x32, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x33, 0x02, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x34, 0x08, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x35, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x36, 0x08, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x37, 0x10, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x38, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x39, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x3a, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x3b, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x3c, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x3d, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x3e, 0xff, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x3f, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x40, 0x0F, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x41, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x42, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x43, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x44, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x45, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x46, 0x20, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x47, 0x0b, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x48, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x49, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x4a, 0x02, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x4b, 0x14, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x4c, 0x21, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x4d, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x4e, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x4f, 0x05, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x50, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x51, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x52, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x53, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x54, 0xc8, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x55, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x56, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x57, 0x38, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x58, 0xff, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x59, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x5a, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x5b, 0x08, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x5c, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x5d, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x5e, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x5f, 0xcc, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x60, 0x07, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x61, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x62, 0xf1, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x63, 0x05, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x64, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x65, 0xa0, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x66, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x67, 0x80, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x68, 0x08, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x69, 0x38, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x6a, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x6b, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x6c, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x6d, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x6e, 0x0f, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x6f, 0x89, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x70, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x71, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x72, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x73, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x74, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x75, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x76, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x77, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x78, 0x07, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x79, 0x05, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x7a, 0x06, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x7b, 0x06, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x7c, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x7d, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x7e, 0x02, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x7f, 0xc7, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x80, 0xff, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x81, 0x9b, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x82, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x83, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x84, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x85, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x86, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE},
    {0x87, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE}
};


/**
 * @brief Internal function to check the parameters entered into function
 * @param sensor_ctx The sensor settings ptr which should contain i2c bus details
 * @return - MANIKIN_STATUS_OK if all parameters are valid
 *         - MANIKIN_STATUS_ERR_NULL_PARAM if invalid
 */
static manikin_status_t
vl53l4cd_check_params (manikin_sensor_ctx_t *sensor_ctx)
{
    MANIKIN_ASSERT(HASH_VL53L4CD, (sensor_ctx != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_VL53L4CD, (sensor_ctx->i2c != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}


int VL53L4CD_SetRangeTiming(
		manikin_sensor_ctx_t *ctx,
		uint32_t timing_budget_ms,
		uint32_t inter_measurement_ms)
{
	manikin_status_t status = MANIKIN_STATUS_OK;
	uint16_t clock_pll, osc_frequency, ms_byte;
	uint32_t macro_period_us = 0, timing_budget_us = 0, ls_byte, tmp;
	float inter_measurement_factor = (float)1.055;

	status |= manikin_i2c_read_reg16(ctx->i2c, ctx->i2c_addr, 0x0006, (uint8_t*)&osc_frequency);
	if(osc_frequency != (uint16_t)0)
	{
		timing_budget_us = timing_budget_ms*(uint32_t)1000;
		macro_period_us = (uint32_t)((uint32_t)2304 *
		((uint32_t)0x40000000 / (uint32_t)osc_frequency)) >> 6;
	}
	else
	{
		status |= (uint8_t)MANIKIN_STATUS_ERR_NULL_PARAM;
	}

	/* Timing budget check validity */
	if ((timing_budget_ms < (uint32_t)10) 
			|| (timing_budget_ms > (uint32_t)200) || (status != (uint8_t)0))
	{
		status |= MANIKIN_STATUS_ERR_NULL_PARAM;
	}
	/* Sensor runs in continuous mode */
	else if(inter_measurement_ms == (uint32_t)0)
	{
		status |= manikin_i2c_write_reg16(ctx->i2c, ctx->i2c_addr,VL53L4CD_INTERMEASUREMENT_MS, 0);
		timing_budget_us -= (uint32_t)2500;
	}
	/* Sensor runs in autonomous low power mode */
	else if(inter_measurement_ms > timing_budget_ms)
	{
		status |= manikin_i2c_read_reg16(ctx->i2c,
				ctx->i2c_addr ,VL53L4CD_RESULT__OSC_CALIBRATE_VAL, (uint8_t*)&clock_pll);
		clock_pll = clock_pll & (uint16_t)0x3FF;
				inter_measurement_factor = inter_measurement_factor
				  * (float)inter_measurement_ms
				  * (float)clock_pll;
		status |= manikin_i2c_write_reg16(ctx->i2c, ctx->i2c_addr, VL53L4CD_INTERMEASUREMENT_MS,
				(uint32_t)inter_measurement_factor);

		timing_budget_us -= (uint32_t)4300;
		timing_budget_us /= (uint32_t)2;

	}
	/* Invalid case */
	else
	{
		status |= (uint8_t)MANIKIN_STATUS_ERR_NULL_PARAM;
	}

	if(status != (uint8_t)MANIKIN_STATUS_ERR_NULL_PARAM)
	{
				ms_byte = 0;
				timing_budget_us = timing_budget_us << 12;
				tmp = macro_period_us*(uint32_t)16;
				ls_byte = ((timing_budget_us + ((tmp >> 6)>>1)) /(tmp>> 6))
				  - (uint32_t)1;

				while ((ls_byte & 0xFFFFFF00U) > 0U) {
						 ls_byte = ls_byte >> 1;
						 ms_byte++;
				}
				ms_byte = (uint16_t)(ms_byte << 8)
			+ (uint16_t) (ls_byte & (uint32_t)0xFF);
				status |= manikin_i2c_write_reg16(ctx->i2c, ctx->i2c_addr, VL53L4CD_RANGE_CONFIG_A,ms_byte);

				ms_byte = 0;
				tmp = macro_period_us*(uint32_t)12;
				ls_byte = ((timing_budget_us + ((tmp >> 6)>>1)) /(tmp>> 6))
				  - (uint32_t)1;

				while ((ls_byte & 0xFFFFFF00U) > 0U) {
						 ls_byte = ls_byte >> 1;
						 ms_byte++;
				}
				ms_byte = (uint16_t)(ms_byte << 8)
			+ (uint16_t) (ls_byte & (uint32_t)0xFF);
				status |= manikin_i2c_write_reg16(ctx->i2c, ctx->i2c_addr, VL53L4CD_RANGE_CONFIG_B,ms_byte);
	}

	return status;
}


manikin_status_t
vl53l4cd_init_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = vl53l4cd_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), status);

    sensor_ctx->needs_reinit = 0;
    uint8_t  data[2]         = { 0 };
    uint16_t model_id        = 0;

    /* Check sensor fresh-out-of-reset bit, similar to VL6180X approach */
    status = manikin_i2c_read_reg(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL53L4CD_FIRMWARE_SYSTEM_STATUS, &data[0]);
    MANIKIN_ASSERT(HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), status);

    /* Check if sensor is responding by reading the model ID */
    status = manikin_i2c_read_reg16(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL53L4CD_IDENTIFICATION_MODEL_ID, data);
    MANIKIN_ASSERT(HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), status);

    /* Check model ID is correct (0xEACC) */
    model_id = (uint16_t)((data[0] << 8) | data[1]);
    // MANIKIN_NON_CRIT_ASSERT(
    //     HASH_VL53L4CD, (model_id == VL53L4CD_MODEL_ID_VALUE), MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL);

    /* Write configuration registers */
    for (size_t i = 0; i < sizeof(vl53l4cd_init_regs) / sizeof(manikin_sensor_reg_t); i++)
    {
        status = manikin_i2c_write_reg(sensor_ctx->i2c,
                                       sensor_ctx->i2c_addr,
                                       vl53l4cd_init_regs[i].reg,
                                       GET_LOWER_8_BITS_OF_SHORT(vl53l4cd_init_regs[i].val));
        MANIKIN_ASSERT(
            HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL);
    }

    /* Start ranging in continuous mode */
    status = manikin_i2c_write_reg(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL53L4CD_SYSTEM_START, VL53L4CD_RANGING_START);
    MANIKIN_ASSERT(
        HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL);
    status = manikin_i2c_write_reg(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL53L4CD_SYSTEM_START, 0x80);
    VL53L4CD_SetRangeTiming(sensor_ctx, 1, 0);
    status = manikin_i2c_write_reg(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL53L4CD_SYSTEM_START, VL53L4CD_RANGING_START);
    return status;
}

manikin_status_t
vl53l4cd_read_sensor (manikin_sensor_ctx_t *sensor_ctx, uint8_t *read_buf)
{
    manikin_status_t status = vl53l4cd_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), status);

    /* Reinitialize if needed */
    if (sensor_ctx->needs_reinit == 1)
    {
        status = vl53l4cd_init_sensor(sensor_ctx);
        MANIKIN_ASSERT(HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), status);
    }

    MANIKIN_ASSERT(HASH_VL53L4CD, (read_buf != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);

    /* Check for data ready */
    uint8_t data_ready = 0;
    status             = manikin_i2c_read_reg(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL53L4CD_SYSTEM_INTERRUPT_CLEAR, &data_ready);
    MANIKIN_ASSERT(HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), status);

    // /* Check if data is ready (bit 0) */
    // if (!(data_ready & 0x01))
    // {
    //     return MANIKIN_STATUS_ERR_READ_FAIL;
    // }

    /* Read distance measurement (16-bit value) */
    uint8_t distance_data[2] = { 0 };
    status                   = manikin_i2c_read_reg16(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL53L4CD_RESULT_FINAL_RANGE_MM, distance_data);
    MANIKIN_ASSERT(HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), status);

    /* Construct 16-bit distance value */
    uint16_t distance = (uint16_t)((distance_data[0] << 8) | distance_data[1]);

    /* To match VL6180X behavior, clip the distance to single byte (0-255mm) */
    if (distance > 255)
    {
        read_buf[0] = 255;
    }
    else
    {
        read_buf[0] = (uint8_t)distance;
    }

    /* Clear the interrupt for next measurement */
    status = manikin_i2c_write_reg(sensor_ctx->i2c,
                                   sensor_ctx->i2c_addr,
                                   VL53L4CD_SYSTEM_INTERRUPT_CLEAR,
                                   VL53L4CD_CLEAR_INTERRUPT);
    MANIKIN_ASSERT(HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), status);

    return MANIKIN_STATUS_OK;
}

manikin_status_t
vl53l4cd_deinit_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = vl53l4cd_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), status);

    /* Stop ranging */
    status = manikin_i2c_write_reg(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL53L4CD_SYSTEM_START, VL53L4CD_RANGING_STOP);
    MANIKIN_ASSERT(
        HASH_VL53L4CD, (status == MANIKIN_STATUS_OK), MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL);

    return MANIKIN_STATUS_OK;
}

manikin_status_t
vl53l4cd_parse_raw_data (const uint8_t *raw_data, vl53l4cd_sample_data_t *data)
{
    MANIKIN_ASSERT(HASH_VL53L4CD, (raw_data != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_VL53L4CD, (data != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    data->distance_mm = raw_data[0];
    return MANIKIN_STATUS_OK;
}