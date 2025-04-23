# Software requirements

These are the functional requirements for this library:

| ID  | Requirement Description                                                  | Priority   | Rationale                                                  |
|-----|--------------------------------------------------------------------------|------------|------------------------------------------------------------|
| F1  | The software library shall include a module capable of reading distance data from a VL6180X Time-of-Flight (ToF) sensor. | Must-have | Required to measure compression depth accurately using the selected sensor. |
| F2  |The software library shall include a module capable of reading differential pressure data from an SDP810 sensor. | Must-have | Required to measure ventilations volume accurately using the selected sensor. |
| F3  | The software library shall include a module capable of reading 8 analog input channels from an ADS7138 ADC. | Must-have | Required for acquiring raw sensor data that can be used to estimate finger positioning on the chest. |
| F4  | The software library shall include a module capable of reading data from a BMM350 magnetometer sensor. | Must-have | Provides measurements required for orientation analysis in airway management. |
| F5  | The software library shall include a module capable of reading motion and orientation data from a BHI360 smart sensor. | Must-have | Enables acquisition of 6-DoF motion data required for airway obstruction detection and orientation tracking. |
| F6  | The software library shall include a module capable of storing all incoming raw sensor data streams in external flash memory (W25QXX128). | Must-have | Ensures complete data logging for offline analysis, debugging, and traceability.         |
| F7  | The software library shall include mechanisms to detect malfunctions in sensor communication (e.g., I²C bus errors) and handle them in a way that preserves data integrity to a defined level. | Must-have | Ensures system reliability and robustness by preventing corrupted or incomplete data from impacting downstream processing. |
| F8  | The software library shall implement I²C bus recovery mechanisms to restore communication in case of a bus hang or error condition. | Must-have | Prevents system lock-up and ensures continued data acquisition by recovering from I²C communication failures. |
| F9  | The software library shall be designed to support the integration of additional sensors and software functionalities in a modular and extensible manner. | Must-have | Facilitates future development, reuse, and customization. |

## Non-functional requirements
| ID  | Requirement Description                                                                                                     | Rationale                                                                                                           | Priority     |
|-----|-----------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------|--------------|
| T1  | The code shall follow a functional-safety-oriented coding standard appropriate for the system.                            | Prior research by Gert van Lagen and the project's preliminary study identified specific methods to ensure functional safety. Adhering to these standards ensures code readability and consistency. | Must-have    |
| T2  | The code shall be continuously tested using appropriate development methods, such as CI testing, unit testing, and static code analysis. | These methods are recommended to ensure software quality.                  | Must-have    |
| T3  | Documentation for the software shall follow conventions that make it understandable for a second-year ESE student.         | The product owner noted that students often struggle to understand existing software. Clear architecture and documentation enable faster development. | Should-have |
