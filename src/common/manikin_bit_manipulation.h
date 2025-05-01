/**
 * @file            manikin_bit_manipulation.h
 * @brief           Header file with common bit operation functions and macro's
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

#ifndef MANIKIN_BIT_MANIPULATION_H
#define MANIKIN_BIT_MANIPULATION_H
#ifdef __cplusplus
extern "C"
{
#endif

#define BITMASK_COMPARE(NUM, MASK) ((NUM) & (MASK))
#define BIT_IS_SET(VAL, BIT_NUM)   (((VAL) >> (BIT_NUM)) & (1))
#define SHIFT_ONE_LEFT_BY_N(N)     ((1) << (N))

#define GET_LOWER_4_BITS_OF_BYTE(x) BITMASK_COMPARE((x), (0xF))
#define GET_UPPER_4_BITS_OF_BYTE(x) BITMASK_COMPARE(((x) >> (4)), (0xF))

// WARNING: Cast in line below
// NOTE: Cast and Bitmask ensures result is 8-bit after masking
#define GET_LOWER_8_BITS_OF_SHORT(x) (uint8_t)(BITMASK_COMPARE((x), (0xFF)))

// WARNING: Cast in line below
// NOTE: Cast and Bitmask ensures result is 8-bit after masking
#define GET_UPPER_8_BITS_OF_SHORT(x) (uint8_t)(BITMASK_COMPARE(((x) >> (8)), (0xFF)))

// WARNING: Cast in line below
// NOTE: Cast and Bitmask ensures result is 8-bit after masking
#define GET_LOWER_8_BITS_OF_24B(x) (uint8_t)(BITMASK_COMPARE((x), (0xFF)))

// WARNING: Cast in line below
// NOTE: Cast and Bitmask ensures result is 8-bit after masking
#define GET_UPPER_8_BITS_OF_24B(x) (uint8_t)(BITMASK_COMPARE(((x) >> (8)), (0xFF)))

// WARNING: Cast in line below
// NOTE: Cast and Bitmask ensures result is 8-bit after masking
#define GET_LAST_8_BITS_OF_24B(x) (uint8_t)(BITMASK_COMPARE(((x) >> (16)), (0xFF)))

// WARNING: Cast in line below
// NOTE: Mask to extract lower 16 bits (no cast needed if used as int)
#define GET_LOWER_16_BITS_OF_LONG(x) (uint16_t)(BITMASK_COMPARE(((x) >> (16)), (0xFFFF)))

// WARNING: Cast in line below
// NOTE: Shift + mask to get upper 16 bits
#define GET_UPPER_16_BITS_OF_LONG(x) (uint16_t)(BITMASK_COMPARE(((x) >> (16)), (0xFFFF)))

// WARNING: Cast in line below
// NOTE: Combine two 8-bit values; cast ensures result is 16-bit
#define CONSTRUCT_SHORT_FROM_BYTES(upper, lower) ((uint16_t)((upper) << (8)) | (lower))

// WARNING: Cast in line below
// NOTE: Truncate 16-bit value to 12-bit; cast ensures result is within 16-bits
#define TRUNCATE_TO_12B_VAL(byte0, byte1) ((uint16_t)(byte0) << 4) | ((byte1) >> 4)

#ifdef __cplusplus
}
#endif
#endif // MANIKIN_BIT_MANIPULATION_H
