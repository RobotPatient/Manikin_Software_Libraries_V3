/**
 * @file            w25qxx128_regs.h
 * @brief           Header file with registers for the Winbond W25QXX 128 Mbit SPI NOR flash chip
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

#ifndef W25QXX128_REGS_H
#define W25QXX128_REGS_H

#define W25QXX_FLASH_SIZE  (16u * 1024u * 1024u)
#define W25QXX_PAGE_SIZE   (256u)
#define W25QXX_SECTOR_SIZE 4096u

#define W25QXX_REG_JEDEC         0x9Fu
#define W25QXX_REG_JEDEC_WR_SIZE 0x01u
#define W25QXX_REG_JEDEC_SIZE    0x03u

#define W25QXX_REG_RESET_MSB  0x66u
#define W25QXX_REG_RESET_LSB  0x99u
#define W25QXX_REG_RESET_SIZE 0x02u

#define W25QXX_REG_STATUS      0x05u
#define W25QXX_REG_STATUS_SIZE 0x01u
#define W25QXX_REG_STATUS_BUSY 0x01u

#define W25QXX_REG_WREN              0x06u
#define W25QXX_REG_WREN_SIZE         0x01u
#define W25QXX_REG_PAGE_PROGRAM      0x02u
#define W25QXX_REG_PAGE_PROGRAM_SIZE 0x04u

#define W25QXX_REG_READ      0x03u
#define W25QXX_REG_READ_SIZE 0x04u

#define W25QXX_REG_SECTOR_ERASE      0x20u
#define W25QXX_REG_SECTOR_ERASE_SIZE 0x04u

#define W25QXX_MAN_ID_IDX           0x00u
#define W25QXX_RESP_MANUFACTURER_ID 0xEFu // Winbond/

#define W25QXX_MEM_TYPE_IDX  0x01u
#define W25QXX_RESP_MEM_TYPE 0x70u // NOR

#define W25QXX_MEM_SIZE_IDX  0x02u
#define W25QXX_RESP_MEM_SIZE 0x18u // 128 Mbit = 16 MB

/* Power management commands */
#define W25QXX_REG_POWER_DOWN      0xB9u
#define W25QXX_REG_POWER_DOWN_SIZE 0x01u

#endif // W25QXX128_REGS_H
