# stackchan-arduino
Stack Chan Library for ArduinoFramework

[日本語](./README.md) | English

# Function
- Read configuration file functionality<br>Configuration files described below can be read to change settings. <br>Include Stackchan_system_config.h.
- Servo control function<br>Currently three types of servos can be switched and controlled by settings. <br>Include Stackchan_servo.h.
  - PWM<br>PWM servos such as SG90
  - SCS<br>Feetech SCS0009 serial servo
  - DYN_XL330<br>Dynamixel XL330-M288-T
- Notice:Unimplemented Stack-chan_Takao_Base status monitoring<br>We plan to implement useful functions for using Stack-chan_Takao_Base.

# Usage
- [Basic](./examples/Basic/) See here how to load parameters.
- [stack-chan_tester](https://github.com/mongonta0716/stack-chan-tester) Usage Servo,etc.

# Configuration files
The settings are commented in detail in each file; please write them in YAML format. <br>* YAML is used because it is convenient to describe comments.
- [SC_BasicConfig.yaml](./data/yaml/SC_BasicConfig.yaml)<br>This file collects the basic parameters necessary to start the stack chan.
- [SC_SecConfig.yaml](./data/yaml/SC_SecConfig.yaml)<br>Configuration file for personal information. (WiFi, APIKey, etc.)
- [SC_ExConfig.yaml](./data/yaml/SC_ExConfig.yaml)<br>This is used when you want to add settings on the application side. The usage is [Folder](./examples/Basic/) source for usage.

# Required Library

For a detailed version, see [library.json](./library.json). If you use ArduinoIDE, please make sure the version is up-to-date or you may not be able to build or use PlatformIO.
- [M5Unified](https://github.com/m5stack/M5Unified)
- [ESP32Servo](https://github.com/madhephaestus/ESP32Servo)
- [ServoEasing](https://github.com/ArminJo/ServoEasing)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
- [Dynamixel2Arduino](https://github.com/ROBOTIS-GIT/Dynamixel2Arduino)
- [YAMLDuino](https://github.com/tobozo/YAMLDuino)
- [SCServo](https://github.com/mongonta0716/SCServo)


# About Stack Chan

Stackchang is an open source project published by [Shishikawasan](https://github.com/stack-chan).

https://github.com/stack-chan/stack-chan

# Credit

- [meganetaaan](https://github.com/meganetaaan)
- [lovyan03](https://github.com/lovyan03/LovyanGFX)
- [robo8080](https://github.com/robo8080)
- [tobozo](https://github.com/tobozo)

# LICENCE

[MIT](./LICENSE)

# Author

[Takao Akaki](https://github.com/mongonta0716)