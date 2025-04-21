#############
Developers guide
#############

This document specifies the rules and mechanisms used by this project.

*************
Project structure
*************

This project is a modular embedded sensor framework with hardware-agnostic drivers in src/, MCU-specific integrations in template_configs/, and unit tests with mocks in tests/. Build and documentation configs are at the root.

.. code-block:: text

    src/                Core drivers and modules for sensors, I2C, timers, and error handling
    template_configs/   Platform-specific integration (e.g., STM32 HAL bindings and configuration)
    tests/              Unit tests and mocks for validating modules in isolation

===========
Platform porting
===========

All modules are coupled with underlying HAL methods by using a **manikin_platform.h** and **manikin_software_conf.h** header file.
The **platform.h** contains all function links, like this:

.. code-block:: c

    #define MANIKIN_I2C_HAL_INIT(i2c_inst, baud) i2c_hal_init(i2c_inst, baud)

And the **conf.h** contains all typedef links, such as peripheral handle definitions:

.. code-block:: c

    typedef I2C_TypeDef *manikin_i2c_inst_t;


All software modules and drivers within this library/framework are written to be platform-agnostic. This means the code is based on ISO C99 and avoids any platform-specific dependencies. For hardware-specific functionality, such as peripheral access, the library uses a HAL (Hardware Abstraction Layer) wrapper, which the user connects to the appropriate MCU HAL.

    While this abstraction layer may appear complex, it is essential for effective testing of the library.
    Without it, each individual MCU HAL implementation would need to be mocked and tested separately.
    This would significantly increase complexity, potentially requiring thousands of unit tests and extensive man-hours to ensure proper maintenance of the library.
    Using this abstraction we can directly control the in & outputs of the software modules in this library.

    Which makes it also possible to run this library on PC native first before testing on target.


*************
Error handling
*************
Error-handling is achieved in multiple ways:

* With status-codes following the manikin_status_t enum:

    .. code-block:: c

        manikin_status_t
        manikin_i2c_init (manikin_i2c_inst_t i2c_inst, const manikin_i2c_speed_t i2c_baud)
        {
            ... Some more code
            return MANIKIN_STATUS_OK;
        }
    Using error-codes for especially public facing API functions is useful and enforced in the coding style applied in this project.
    It allows to check whether the function failed and take action accordingly.

* Error-handling with asserts and error-handlers:

    .. code-block:: c

        static manikin_status_t
        ads7138_check_params (const manikin_sensor_ctx_t *sensor_ctx)
        {
            MANIKIN_ASSERT(HASH_ADS7138, (sensor_ctx != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
            MANIKIN_ASSERT(HASH_ADS7138, (sensor_ctx->i2c != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
            return MANIKIN_STATUS_OK;
        }
    Using MANIKIN_ASSERT() checks if condition fails, if it fails it returns the Error code MANIKIN_STATUS_ERR_NULL_PARAM and calls the user-specified error-handler defined in platform.h.
    Due to it being an inlined macro, the function from which it is defined in, returns. Allowing single-line centralized error handling by routing validation through a configurable macro. It ensures clean, consistent checks while allowing each platform to define its own response (e.g., logging, reset, debug break). This improves portability, code reuse, and debugging without cluttering sensor driver logic.

*************
Code style
*************

The style guide used by this project is the `barr-c coding standard`_. This standard is MISRA compliant, and suggests
rules that improve code-reliability.


===========
Exceptions on Barr-C
===========

* This library uses 100 characters line limit instead of 80 characters

    The original 80-character limit was overly restrictive and often hurt readability, as the linter would break nearly every line unnecessarily.

* Each source file shall not terminate with a comment marking the end of file followed by a blank line

    The /*** end of file ***/ comment is neglected, as it does not offer much benefits. It only takes more manual labor to enforce this rule.
    Instead only the blank line is enforced.

* Yoda conditionals are not used (constant on left side of comparison operand)

    Some argue that using Yoda conditionals enforce safe coding as it makes sure that you can't mistake the comparison ==
    for an assignment =. Though most compilers especially with the compile options, that are set within this project. Forbid
    this and throw an error if this case occurs. Besides it being really confusing seeing an if-statement which has this defined
    this way.

* No comments indicating end function at the ending } bracket of function declaration

    Modern IDE's solve this issue, besides the compiler not even wanting to compile if you forgot this bracket.
    Every comment which is not strictly necessary introduces an additional maintenance cost.

===========
Toolchain settings
===========
* This library is written in C99, with tests in C++17.

    C99 was chosen to reduce complexity. While C++ isn't inherently complex, its many styles and features can complicate code reviews. Especially in embedded contexts, where issues like dynamic memory and ambiguous compiler errors (e.g., templates) are more common.
    **Tests are written in C++** due to the C++-based test framework. However it is highly suggested to stick with basic C++ (STL allowed) and write C++ as you would write C. When unsure, use any existing c++ code as reference. The C++17 standard was chosen, as it offers most flexibility while not being too new.

* This library uses CMake with it's minimum version set at VERSION 3.21

    By far CMake still has most compatibility in C/C++ land. For the Manikin it is important, as it allows for easier CI
    integration. Which is crucial when doing code-reviews, as you get quick insights in whether the code is worth merging or reviewing.

* The Library uses GCC for both compilation of target firmware (gcc-arm-none-eabi v14.2) and tests (gcc x86-64 v14.2.1)

    GCC is the most prevalent compiler for ARM microcontrollers, which is the main target for this library. Likewise GCC
    is still one of most used compilers on the desktop.



These settings are enforced by CMake and should be used when contributing to this library:

* Library compile options:

  - **Linux / macOS / Embedded (GCC/Clang)**:

    ``-Wall -Wextra -Wpedantic``

    ``-Wshadow -Wundef -Wconversion -Wsign-conversion``

    ``-Wcast-align -Wcast-qual -Wstrict-prototypes``

    ``-Wdouble-promotion -Wformat=2 -Wformat-security``

    ``-fno-common -fstack-protector-strong``

  - **Windows (MSVC)**:

    ``/Wall``

        This enables all warnings, including those that are normally disabled by default. The compiler will alert you to potentially unsafe coding practices such as:

        - Integer overflows and underflows
        - Signed/unsigned mismatches
        - Dead code
        - Implicit type conversions
        - Dangerous casts
        - Format string issues

        These warnings help catch bugs early and encourage safer, more portable code.





.. _barr-c coding standard: https://barrgroup.com/sites/default/files/barr_c_coding_standard_2018.pdf