# Module Design

This document describes in detail how each of the software modules works.

The following software module types are present in this collection of libraries:

- Sensor Modules
- Flash/Memory Modules
- Communication wrappers

## Sensor Modules

Sensor modules are the most commonly used type of software module in the Manikin CPR system. Each module follows a consistent structure and exposes a standardized public API.

### API design

The diagram below illustrates the common structure and API of a sensor module:

Every sensor module implements the following functions:

![Sensor module public api](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/sensor_module_api_overview.iuml)

> [!NOTE]  
> This api design is based on the requirement that setting up and reading an sensor should not be a hassle. But the simplicity means that there was a concession on configurability. At this time within the Manikin project, there is no need for the additional sensor configurability. If requirements change (which will surely happen), it is possible to add an enum param to the sensor_ctx struct e.g. with specific configuration options for each sensor.

> [!NOTE]
> It’s also important to note that these modules only provide raw sensor data without performing any processing. This design choice was driven by the need for functional decomposition and the goal of keeping the library as generic as possible. Since requirements frequently change, embedding logic directly into the sensor modules would make them harder to maintain, as any update would require changes to the entire module, something we aim to avoid.

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


## 📄 **Flash/Memory Module**

This module provides a driver interface for memory chips. The driver exposes a consistent API for initialization, status checking, reading, writing, sector erasing, and deinitialization.


### API Design


![Memory module public API](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/memory_module_api_overview.iuml)


#### `<memory_chip>_init()`

<table>
  <tr>
    <th>Description</th>
    <th>Diagram</th>
  </tr>
  <tr>
    <td>
      • Validates input parameters.<br>
      • If parameters are null or out of range.<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MEMORY_CHIP_STATUS_ERR_NULL_PARAM</code>.<br>
      • If valid, read JEDEC ID.<br>
      • If the chip present & JEDEC ID is valid:<br>
      &nbsp;&nbsp;&nbsp;– Resets device to known state.<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MEMORY_CHIP_STATUS_OK</code>.<br>
      • If JEDEC ID is invalid or chip not present:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MEMORY_CHIP_STATUS_ERR_READ_FAIL</code>.
    </td>
    <td>
      <img src="http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/memory_module/memory_module_init.iuml" alt="Memory chip init" />
    </td>
  </tr>
</table>

#### `<memory_chip>_status()`
<table>
  <tr>
    <th>Description</th>
    <th>Diagram</th>
  </tr>
  <tr>
    <td>
      • Validates input parameters.<br>
      • If parameters are null or out of range.<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MEMORY_CHIP_STATUS_ERR_NULL_PARAM</code>.<br>
      • If valid, checks for chip presence using JEDEC ID (no reset).<br>
      • If chip is detected and JEDEC ID is valid:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_OK</code>.<br>
      • If chip is not present or ID is invalid:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_ERR_READ_FAIL</code>.
    </td>
    <td>
      <img src="http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/memory_module/memory_module_status.iuml" alt="Memory module status" />
    </td>
  </tr>
</table>



#### `<memory_chip>_write()`

<table>
  <tr>
    <th>Description</th>
    <th>Diagram</th>
  </tr>
  <tr>
    <td>
      • Validates input parameters (<code>data</code>, <code>addr</code>, <code>len</code>).<br>
      • If any parameter is invalid (e.g., null or zero length):<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_MEMORY_RESULT_PARERR</code>.<br>
      • If parameters are valid, checks if chip is responsive.<br>
      • If chip is not responding:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_MEMORY_RESULT_NOTRDY</code>.<br>
      • If chip is responsive, checks for write protection.<br>
      • If write protection is enabled:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_MEMORY_RESULT_WRPRT</code>.<br>
      • If write protection is not enabled:<br>
      &nbsp;&nbsp;&nbsp;– Writes data to the specified address.<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_MEMORY_RESULT_OK</code>.
    </td>
    <td>
      <img src="http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/memory_module/memory_module_write.iuml" alt="Memory chip write" />
    </td>
  </tr>
</table>


#### `<memory_chip>_read()`

<table>
  <tr>
    <th>Description</th>
    <th>Diagram</th>
  </tr>
  <tr>
    <td>
      • Validates input parameters (<code>data</code>, <code>addr</code>, <code>len</code>).<br>
      • If any parameter is invalid (e.g., null or zero length):<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_MEMORY_RESULT_PARERR</code>.<br>
      • If parameters are valid, checks if chip is responsive.<br>
      • If chip is not responding:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_MEMORY_RESULT_NOTRDY</code>.<br>
      • If chip is responsive, checks for write protection.<br>
      • If write protection is enabled:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_MEMORY_RESULT_WRPRT</code>.<br>
      • If write protection is not enabled:<br>
      &nbsp;&nbsp;&nbsp;– Reads data from the specified address.<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_MEMORY_RESULT_OK</code>.
    </td>
    <td>
      <img src="http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/memory_module/memory_module_read.iuml" alt="Memory chip read" />
    </td>
  </tr>
</table>


---

#### `<memory_chip>_erase_sector()`

<table>
  <tr>
    <th>Description</th>
    <th>Diagram</th>
  </tr>
  <tr>
    <td>
      • Validates the <code>sector_number</code> parameter.<br>
      • If parameter is invalid or out of range:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_MEMORY_RESULT_PARERR</code>.<br>
      • If parameter is valid, checks if chip is responsive.<br>
      • If chip is not responding:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_MEMORY_RESULT_NOTRDY</code>.<br>
      • If chip is responsive, checks for write protection.<br>
      • If write protection is enabled:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_MEMORY_RESULT_WRPRT</code>.<br>
      • If write protection is not enabled:<br>
      &nbsp;&nbsp;&nbsp;– Erases the specified 4KB sector.<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_MEMORY_RESULT_OK</code>.
    </td>
    <td>
      <img src="http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/memory_module/memory_module_erase.iuml" alt="Memory chip erase" />
    </td>
  </tr>
</table>


---

#### `<memory_chip>_deinit()`

<table>
  <tr>
    <th>Description</th>
    <th>Diagram</th>
  </tr>
  <tr>
    <td>
      • Validates input parameters.<br>
      • If parameters are null or out of range:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_CHIP_STATUS_ERR_NULL_PARAM</code>.<br>
      • If parameters are valid, checks for chip presence.<br>
      • If the chip is present:<br>
      &nbsp;&nbsp;&nbsp;– Resets device and clears internal state.<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_OK</code>.<br>
      • If chip is not present:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_ERR_WRITE_FAIL</code>.
    </td>
    <td>
      <img src="http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/memory_module/memory_module_deinit.iuml" alt="Memory chip deinit" />
    </td>
  </tr>
</table>



## 📄 **I2C Peripheral Module**

This module provides a platform-agnostic driver interface for I2C communication. It wraps low-level hardware access in a consistent API used throughout the Manikin CPR system.

---

### Public API

![I2C module public API](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/i2c_module_api_overview.iuml)


### Function Descriptions

#### `manikin_i2c_init()`
<table>
  <tr>
    <th>Description</th>
    <th>Diagram</th>
  </tr>
  <tr>
    <td>
      • Validates I2C instance and baud-rate parameters.<br>
      • If parameters are invalid:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_ERR_NULL_PARAM</code> or <code>MANIKIN_STATUS_ERR_INVALID_I2C_BAUD</code>.<br>
      • If valid:<br>
      &nbsp;&nbsp;&nbsp;– Initializes I2C with 7-bit addressing and 1 stop-bit.<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_OK</code>.
    </td>
    <td>
      <img src="http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/i2c_module/i2c_module_init.iuml" alt="I2C module init" />
    </td>
  </tr>
</table>

#### `manikin_i2c_check_device_address()`
<table>
  <tr>
    <th>Description</th>
    <th>Diagram</th>
  </tr>
  <tr>
    <td>
      • Checks the presence of an I2C device at a given address.<br>
      • Sends an I2C ping.<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>1</code> if device responds.<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>0</code> if no device is found.
    </td>
    <td>
      <img src="http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/i2c_module/i2c_module_check_device_address.iuml" alt="I2C module check_device_address" />
    </td>
  </tr>
</table>

#### `manikin_i2c_write_reg()` / `write_reg16()`
<table>
  <tr>
    <th>Description</th>
    <th>Diagram</th>
  </tr>
  <tr>
    <td>
      • Validates parameters.<br>
      • If invalid:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_ERR_NULL_PARAM</code>.<br>
      • Writes 8-bit or 16-bit data to I2C register.<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_OK</code> on success.
    </td>
    <td>
      <img src="http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/i2c_module/i2c_module_write_reg.iuml" alt="I2C module write reg" />
    </td>
  </tr>
</table>

#### `manikin_i2c_read_reg()` / `read_reg16()`
<table>
  <tr>
    <th>Description</th>
    <th>Diagram</th>
  </tr>
  <tr>
    <td>
      • Validates parameters.<br>
      • If invalid:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_ERR_NULL_PARAM</code>.<br>
      • Reads 8-bit or 16-bit data from I2C register.<br>
      &nbsp;&nbsp;&nbsp;– Stores data to provided pointer.<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_OK</code>.
    </td>
    <td>
      <img src="http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/i2c_module/i2c_module_read_reg.iuml" alt="I2C module read reg" />
    </td>
  </tr>
</table>

#### `manikin_i2c_read_bytes()`
<table>
  <tr>
    <th>Description</th>
    <th>Diagram</th>
  </tr>
  <tr>
    <td>
      • Reads multiple bytes from an I2C device.<br>
      • Saves data to provided buffer.<br>
      &nbsp;&nbsp;&nbsp;– Returns number of bytes successfully read.
    </td>
    <td>
      <img src="http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/i2c_module/i2c_module_read_bytes.iuml" alt="I2C module read bytes" />
    </td>
  </tr>
</table>

#### `manikin_i2c_write_bytes()`
<table>
  <tr>
    <th>Description</th>
    <th>Diagram</th>
  </tr>
  <tr>
    <td>
      • Writes multiple bytes from a buffer to I2C device.<br>
      &nbsp;&nbsp;&nbsp;– Returns number of bytes successfully written.
    </td>
    <td>
      <img src="http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/i2c_module/i2c_module_write_bytes.iuml" alt="I2C module write bytes" />
    </td>
  </tr>
</table>


#### `manikin_i2c_deinit()`
<table>
  <tr>
    <th>Description</th>
    <th>Diagram</th>
  </tr>
  <tr>
    <td>
      • Validates I2C instance pointer.<br>
      • If invalid:<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_ERR_NULL_PARAM</code>.<br>
      • If valid:<br>
      &nbsp;&nbsp;&nbsp;– Deinitializes I2C peripheral.<br>
      &nbsp;&nbsp;&nbsp;– Returns <code>MANIKIN_STATUS_OK</code>.
    </td>
    <td>
      <img src="http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/RobotPatient/Manikin_Software_Libraries_V3/refs/heads/dev/docs/assets/i2c_module/i2c_module_deinit.iuml" alt="I2C module deinit" />
    </td>
  </tr>
</table>


### Design Rationale

This driver avoids unnecessary abstraction and complexity to preserve deterministic timing, hardware safety, and MISRA compliance.

As with other modules in the Manikin Software Library:
- No dynamic allocation
- No function pointers or runtime interfaces
- No object-oriented abstractions

The design keeps the code minimal, traceable, and suitable for high-reliability systems.
