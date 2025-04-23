# Module Design

This document describes in detail how each of the software modules works.

The following software module types are present in this collection of libraries:

- Sensor Modules
- Flash/Memory Modules
- Communication wrappers

## Sensor Modules

Sensor modules are the most commonly used type of software module in the Manikin CPR system. Each module follows a consistent structure and exposes a standardized public API.

### Class Diagram

The diagram below illustrates the common structure and API of a sensor module:

![Sensor module public api](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/sensor_module_api_overview.iuml)

### Public API

Every sensor module implements the following functions:

```c
manikin_status_t <sensor_name>_init_sensor(manikin_sensor_ctx_t * sensor_ctx);
manikin_status_t <sensor_name>_read_sensor(manikin_sensor_ctx_t * sensor_ctx, uint8_t * read_buf);
manikin_status_t <sensor_name>_deinit_sensor(manikin_sensor_ctx_t * sensor_ctx);
```
> [!NOTE]  
> This api design is based on the requirement that setting up and reading an sensor should not be a hassle. But the simplicity means that there was a concession on configurability. There was at the time within the Manikin project no need for the additional sensor configurability. If requirements change (which will surely happen), it is possible to add an uint32_t param to the sensor_ctx struct e.g. with specific configuration options.


#### `init_sensor`

<table>
  <tr>
    <th>Description</th>
    <th>Diagram</th>
  </tr>
  <tr>
    <td>
      • Validates input parameters.<br>
      • If parameters are null or out of range.<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_ERR_NULL_PARAM</code>.<br>
      • If valid, checks for sensor presence.<br>
      • If the sensor is present:<br>
      &nbsp;&nbsp;&nbsp;– Initializes with default settings (continuous mode).<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_OK</code>.<br>
      • If sensor is not present or init fails:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL</code>.
    </td>
    <td>
      <img src="http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/sensor_module/sensor_module_init.iuml" alt="Sensor module init" />
    </td>
  </tr>
</table>


#### `read_sensor`

<table>
  <tr>
    <th>Description</th>
    <th>Diagram</th>
  </tr>
  <tr>
    <td>
      • Validates input parameters.<br>
      • If parameters are null or out of range.<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_ERR_NULL_PARAM</code>.<br>
      • If valid, checks I2C and sensor status register for faults.<br>
      • If a fault is detected:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_ERR_READ_FAIL</code>.<br>
      • If no faults are present:<br>
      &nbsp;&nbsp;&nbsp;– Reads result register.<br>
      &nbsp;&nbsp;&nbsp;– Writes data to passed-in byte array.<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_OK</code>.
    </td>
    <td>
      <img src="http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/sensor_module/sensor_module_read.iuml" alt="Sensor module init" />
    </td>
  </tr>
</table>

> [!NOTE]   The format and minimum required length of read_buf depend on the specific sensor. Refer to the sensor’s header file for detailed information.


`deinit_sensor`

<table>
  <tr>
    <th>Description</th>
    <th>Diagram</th>
  </tr>
  <tr>
    <td>
      • Validates input parameters.<br>
      • If parameters are null or out of range.<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_ERR_NULL_PARAM</code>.<br>
      • If valid, checks for sensor presence.<br>
      • If the sensor is present:<br>
      &nbsp;&nbsp;&nbsp;– Deinitializes sensor to idle/sleep mode.<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_OK</code>.<br>
      • If sensor is not present:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL</code>.
    </td>
    <td>
      <img src="http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/sensor_module/sensor_module_deinit.iuml" alt="Sensor module deinit" />
    </td>
  </tr>
</table>

### Reasoning for this design choice
This API is implemented in plain C without further abstraction layers to maximize reliability and maintain deterministic behavior.

The software loosely follows the MISRA standard. As a result:

- Runtime polymorphism (e.g., interface-based design)

- Function pointer tables

- Dynamic dispatch mechanisms

...are intentionally avoided to reduce complexity and risk in embedded environments.

## 📄 **Flash/Memory Module**

This module provides a driver interface for memory chips. The driver exposes a consistent API for initialization, status checking, reading, writing, sector erasing, and deinitialization.

### Class Diagram

The diagram below illustrates the structure and public API of the <memory_chip> software module:

![Memory module public API](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/memory_module_api_overview.iuml)

---

### Public API

```c
manikin_status_t         <memory_chip>_init(manikin_spi_memory_ctx_t *mem_ctx);
manikin_status_t         <memory_chip>_status(manikin_spi_memory_ctx_t *mem_ctx);
manikin_memory_result_t  <memory_chip>_write(manikin_spi_memory_ctx_t *mem_ctx, uint8_t *data, uint32_t addr, size_t len);
manikin_memory_result_t  <memory_chip>_read(manikin_spi_memory_ctx_t *mem_ctx, uint8_t *data, uint32_t addr, size_t len);
manikin_memory_result_t  <memory_chip>_erase_sector(manikin_spi_memory_ctx_t *mem_ctx, uint32_t sector_number);
manikin_status_t         <memory_chip>_deinit(manikin_spi_memory_ctx_t *mem_ctx);
```

---

### Function Descriptions

#### `<memory_chip>_init()`

Initializes the <memory_chip> chip:
- Checks for chip presence
- Reads the JEDEC ID to verify correct device
- Resets the device if detected

**Returns:**
- `MANIKIN_STATUS_OK` if the chip is present and bus is configured correctly
- `MANIKIN_STATUS_ERR_READ_FAIL` if the chip is not recognized

---

#### `<memory_chip>_status()`

Checks for presence of the flash chip without performing a reset.

**Returns:**
- `MANIKIN_STATUS_OK` if chip is found and responsive
- `MANIKIN_STATUS_ERR_READ_FAIL` otherwise

---

#### `<memory_chip>_write()`

Writes an arbitrary number of bytes to a specified memory address.

**Parameters:**
- `data` – buffer with the data to write
- `addr` – flash memory address to write to
- `len` – number of bytes to write

**Returns:**
- `MANIKIN_MEMORY_RESULT_OK` on success
- `MANIKIN_MEMORY_RESULT_WRPRT` if flash is write-protected
- `MANIKIN_MEMORY_RESULT_NOTRDY` if the chip is unresponsive
- `MANIKIN_MEMORY_RESULT_PARERR` for invalid parameters

---

#### `<memory_chip>_read()`

Reads a block of bytes from a specified memory address.

**Parameters:**
- `data` – destination buffer
- `addr` – flash memory address to read from
- `len` – number of bytes to read

**Returns:** same as `<memory_chip>_write()`

---

#### `<memory_chip>_erase_sector()`

Erases a single 4KB sector.

**Parameters:**
- `sector_number` – sector to erase (sector size is fixed at 4KB)

**Returns:** same as `<memory_chip>_write()`

---

#### `<memory_chip>_deinit()`

Resets the flash chip and clears internal state.

**Returns:** same as `<memory_chip>_init()`

---

### Call Sequence

The following diagram shows a typical call sequence when using the flash memory module:

![memory module call sequence](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/memory_module_call_sequence.iuml)

---

### Design Rationale

This module is implemented in **plain C** to maximize reliability and portability on embedded systems.

In line with the sensor modules, it avoids abstraction mechanisms that are:
- Not MISRA-compliant
- Non-deterministic at runtime
- Error-prone in low-level hardware access

Thus:
- **Function pointer tables**
- **Interfaces with runtime polymorphism**
- **Dynamic dispatch or allocation**

...are not used.

## 📄 **I2C Peripheral Module**

This module provides a platform-agnostic driver interface for I2C communication. It wraps low-level hardware access in a consistent API used throughout the Manikin CPR system.

---

### Class Diagram

The diagram below illustrates the structure and public API of the I2C wrapper module:

![I2C module public API](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/i2c_module_api_overview.iuml)

---

### Public API

```c
manikin_status_t manikin_i2c_init(manikin_i2c_inst_t i2c_inst, manikin_i2c_speed_t i2c_baud);
uint8_t          manikin_i2c_check_device_address(manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr);
manikin_status_t manikin_i2c_write_reg(manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr, uint16_t reg, uint8_t data);
manikin_status_t manikin_i2c_write_reg16(manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr, uint16_t reg, uint16_t data);
manikin_status_t manikin_i2c_read_reg(manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr, uint16_t reg, uint8_t *data);
manikin_status_t manikin_i2c_read_reg16(manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr, uint16_t reg, uint8_t *data);
size_t           manikin_i2c_read_bytes(manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr, uint8_t *data, size_t len);
size_t           manikin_i2c_write_bytes(manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr, const uint8_t *data, size_t len);
manikin_status_t manikin_i2c_deinit(manikin_i2c_inst_t i2c_inst);
```

---

### Function Descriptions

#### `manikin_i2c_init()`
Initializes the I2C peripheral with Manikin default settings (1 stop bit, 7-bit addressing).

#### `manikin_i2c_check_device_address()`
Checks if a device is present at a given I2C address.

#### `manikin_i2c_write_reg()` / `write_reg16()`
Writes an 8-bit or 16-bit value to a register on an I2C device.

#### `manikin_i2c_read_reg()` / `read_reg16()`
Reads an 8-bit or 16-bit value from a register on an I2C device.

#### `manikin_i2c_read_bytes()`
Reads multiple bytes into a buffer from the specified I2C device.

#### `manikin_i2c_write_bytes()`
Writes multiple bytes from a buffer to the specified I2C device.

#### `manikin_i2c_deinit()`
Deinitializes the I2C peripheral.

---

### Call Sequence

The following diagram shows a typical I2C transaction flow:

![I2C module call sequence](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/i2c_module_call_sequence.iuml)

---

### Design Rationale

This driver avoids unnecessary abstraction and complexity to preserve deterministic timing, hardware safety, and MISRA compliance.

As with other modules in the Manikin Software Library:
- No dynamic allocation
- No function pointers or runtime interfaces
- No object-oriented abstractions

The design keeps the code minimal, traceable, and suitable for high-reliability systems.

---
