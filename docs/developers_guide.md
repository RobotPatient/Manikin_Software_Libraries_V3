# Developers Guide

This document specifies the rules and mechanisms used by this project.

## Project Structure

This project is a modular embedded sensor framework with:

- Hardware-agnostic drivers in `src/`
- MCU-specific integrations in `template_configs/`
- Unit tests with mocks in `tests/`
- Build and documentation configs at the root

```
src/                Core drivers and modules
template_configs/   Platform-specific integration
tests/              Unit tests and mocks for modules and drivers
```

## Platform Porting

All modules are coupled with underlying HAL methods using `manikin_platform.h` and `manikin_software_conf.h`.

The `platform.h` contains function links like:

```c
#define MANIKIN_I2C_HAL_INIT(i2c_inst, baud) i2c_hal_init(i2c_inst, baud)
```

The `conf.h` contains typedef links, such as peripheral handle definitions:

```c
typedef I2C_TypeDef *manikin_i2c_inst_t;
```

All software modules and drivers are written to be platform-agnostic. The code follows ISO C99 and avoids platform-specific dependencies. For hardware access, a Hardware Abstraction Layer (HAL) wrapper is used, which the user connects to the target MCU HAL. With the `manikin_platform.h` and `manikin_software_conf.h` file.

> While this abstraction layer may appear complex, it's essential for effective testing.  
> Without it, each individual MCU HAL would need its own mocks and tests—massively increasing complexity.  
> This approach allows us to control the inputs and outputs of the software modules and run the library natively on PC before testing on target.

## Error Handling

Error handling is achieved in two main ways:

### 1. Status Codes

Functions return `manikin_status_t` codes:

```c
manikin_status_t
manikin_i2c_init (manikin_i2c_inst_t i2c_inst, const manikin_i2c_speed_t i2c_baud)
{
    // ...
    return MANIKIN_STATUS_OK;
}
```

This pattern is enforced for all public-facing APIs to allow easy error checks and standardized responses.

### 2. Asserts and Error Handlers

```c
static manikin_status_t
ads7138_check_params (const manikin_sensor_ctx_t *sensor_ctx)
{
    MANIKIN_ASSERT(HASH_ADS7138, (sensor_ctx != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_ADS7138, (sensor_ctx->i2c != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}
```

The `MANIKIN_ASSERT()` macro triggers the error-handler if a check fails and returns the specified error code.  
It allows centralized, platform-specific error responses like logging, resets, or breakpoints, keeping sensor drivers clean and reusable.

## Code Style

This project follows the [Barr-C coding standard](https://barrgroup.com/sites/default/files/barr_c_coding_standard_2018.pdf), which is MISRA compliant and improves code reliability.

## Exceptions to Barr-C

- **Line length:** Uses a 100-character limit instead of 80 for better readability.

- **No End-of-file comments:** Skips `/*** end of file ***/`; only a blank line at the end is required.

```c
...
/*** end of file ***/
```

- **Yoda conditionals:** Not used. Modern compilers catch assignment-instead-of-comparison errors.

```c
Yoda:

if(2 != x)

Normal:

if(x != 2)
```

- **No end-function comments:** Avoided; modern IDEs and compilers make these unnecessary.

```c
void test() {

}/*** test() ***/

```

## Toolchain Settings

- **Languages:**  
  - Library: **C99**  
  - Tests: **C++17**  
    - Use minimal C++ (e.g., STL is allowed). Avoid complex patterns and write C++ as if it were C.

- **Build System:**  
  - **CMake (min version 3.21)**  
    - Chosen for compatibility and CI integration.

- **Compilers:**  
  - **Target firmware:** `gcc-arm-none-eabi v14.2`  
  - **Tests (host):** `GCC x86-64 v14.2.1`

### Compiler Flags

#### Linux / macOS / Embedded (GCC/Clang):

```bash
-Wall -Wextra -Wpedantic
-Wshadow -Wundef -Wconversion -Wsign-conversion
-Wcast-align -Wcast-qual -Wstrict-prototypes
-Wdouble-promotion -Wformat=2 -Wformat-security
-fno-common -fstack-protector-strong
```

#### Windows (MSVC):

```bash
/Wall
```


 >  Enables all warnings, including:
 > - Integer overflows/underflows  
 > - Signed/unsigned mismatches  
 > - Dead code  
 > - Implicit type conversions  
 > - Dangerous casts  
 > - Format string vulnerabilities
 > These flags help ensure portable, maintainable, and safe code.


