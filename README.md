# Manikin_Software_Libraries_V3

Manikin Software Libraries V3 is a modular embedded C library designed for high-reliability systems like the Manikin CPR simulator platform. It provides a unified API layer for hardware modules such as sensors, memory, I2C, SPI, and system control — all optimized for safety-critical environments.

---

## 📚 Overview

This library adheres to MISRA-inspired best practices to ensure deterministic behavior, testability, and maintainability. Features include:

- 🧱 Modular architecture (sensors, memory, I2C, etc.)
- 🛡️ No dynamic memory allocation or runtime polymorphism
- ⚡ Bare-metal friendly: fast, lightweight, and portable
- 🧪 Easily testable APIs with clear separation of concerns

---

## 🧩 Modules

| Module        | Description                             | Docs & Diagrams |
|---------------|-----------------------------------------|-----------------|
| [`sensors/`](./src/sensors) | Driver interface for Manikin sensors         | [Sensor Module Docs](docs/sensor_module.md) |
| [`memory/`](./src/memory)   | SPI NOR flash memory abstraction (W25QXX)    | [Memory Module Docs](docs/memory_module.md) |
| [`i2c/`](./src/i2c)         | Platform-agnostic I2C communication layer    | [I2C Module Docs](docs/i2c_module.md) |
| [`spi/`](./src/spi)         | SPI wrapper (if applicable)                 | *(TBD)* |
| [`system/`](./src/system)   | Reset control, timers, utility functions     | *(TBD)* |

---

## 🛠️ Build & Integration

This library is designed to integrate easily into existing embedded toolchains.

### Requirements
- Standard-compliant C99 compiler
- Minimal HAL dependencies (e.g., STM32 HAL or platform-specific SPI/I2C)
- No RTOS required

### Include in your project:

```c
#include "manikin_types.h"
#include "sensors/<sensor>.h"
#include "memory/w25qxx128.h"
#include "i2c/i2c.h"
```

---

## 📖 Documentation

Visual API and call sequence diagrams are included per module:

> Full documentation is available in the [`docs/`](./docs/) directory.

---

## 🧪 Testing

Each module can be unit-tested independently using mocks for bus interfaces. Tests can be written using:

- FFF / Catch2
- Platform-specific hardware test benches

---

## 🤝 Contributing

We welcome contributions from the community! To contribute:

1. Fork this repo
2. Create a branch: `feature/your-change`
3. Write clear, testable code
4. Submit a pull request

Please follow existing formatting and naming conventions.

---

## 📄 License

Licensed under the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0).  
Copyright © 2025 RobotPatient
