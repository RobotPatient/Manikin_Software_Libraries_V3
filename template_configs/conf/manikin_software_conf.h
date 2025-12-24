/**
 * @file            manikin_software_conf.h
 * @brief           Platform typedef configuration for Manikin Libraries V3 library
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

 #ifndef MANIKIN_CONF_H
 #define MANIKIN_CONF_H
 #ifdef __cplusplus
 extern "C"
 {
 #endif
 #include <zephyr/device.h>
 #include <zephyr/kernel.h>
 #include <stdint.h>
 
 #define MANIKIN_SOFTWARE_MAX_TIMEOUT   1u
 
     typedef const struct device *manikin_i2c_inst_t;
     typedef const struct device *manikin_spi_inst_t;
     typedef const struct device *manikin_spi_cs_t;
 #ifdef __cplusplus
 }
 #endif
 #endif /* MANIKIN_CONF_H */