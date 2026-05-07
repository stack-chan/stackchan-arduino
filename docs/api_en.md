# Stackchan Arduino Library API Documentation

## Overview
The Stackchan Arduino Library is a library for controlling Stack-chan on M5Stack devices. This library provides features such as servo motor control, configuration file loading, and Bluetooth settings.

---

## Class List

### 1. `StackchanSystemConfig`
A class that loads configuration files and manages system settings.

#### Methods
- **`loadConfig(fs::FS& fs, const char *app_yaml_filename, uint32_t app_yaml_filesize, const char* secret_yaml_filename, uint32_t secret_yaml_filesize, const char* basic_yaml_filename, uint32_t basic_yaml_filesize)`**
  - Loads configuration files and initializes system settings.
  - Arguments:
    - `fs`: File system object.
    - `app_yaml_filename`: Path to the application configuration file.
    - `app_yaml_filesize`: Size of the application configuration file.
    - `secret_yaml_filename`: Path to the personal information configuration file.
    - `secret_yaml_filesize`: Size of the personal information configuration file.
    - `basic_yaml_filename`: Path to the basic configuration file.
    - `basic_yaml_filesize`: Size of the basic configuration file.

- **`printAllParameters()`**
  - Outputs all configuration parameters to the log.

- **`getServoInfo(uint8_t servo_axis_no)`**
  - Gets information for the specified servo axis.

- **`getWiFiSetting()`**
  - Gets the WiFi settings.

- **`getAPISetting()`**
  - Gets the API key settings.

---

### 2. `StackchanSERVO`
A class that controls servo motors.

#### Methods
- **`begin(stackchan_servo_initial_param_s init_params)`**
  - Initializes the servo motors.

- **`moveX(int x, uint32_t millis_for_move = 0)`**
  - Moves the X-axis servo to the specified angle.
  - Arguments:
    - `x`: Angle to move to.
    - `millis_for_move`: Time to spend on the movement, in milliseconds.

- **`moveY(int y, uint32_t millis_for_move = 0, bool wait_for_completion = true)`**
  - Moves the Y-axis servo to the specified angle.
  - If `wait_for_completion` is set to `false`, the function returns without waiting for the movement to complete.

- **`moveXY(int x, int y, uint32_t millis_for_move)`**
  - Moves the X-axis and Y-axis servos simultaneously.

- **`motion(Motion motion_no)`**
  - Runs a preset motion.
  - Arguments:
    - `motion_no`: Type of motion to run, such as `greet` or `laugh`.

---

### 3. `StackchanExConfig`
A class that extends `StackchanSystemConfig` and manages application-specific settings.

#### Methods
- **`loadExtendConfig(fs::FS& fs, const char *yaml_filename, uint32_t yaml_size)`**
  - Loads an extended configuration file.

- **`setExtendSettings(DynamicJsonDocument doc)`**
  - Applies extended settings.

- **`printExtParameters()`**
  - Outputs extended setting parameters to the log.

---

## Enumerations

### `Motion`
Defines preset servo motor motions.
- `nomove`: No movement.
- `greet`: Greeting.
- `laugh`: Laugh.
- `nod`: Nod.
- `refuse`: Refuse.
- `test`: For testing.

### `ServoType`
Defines servo motor types.
- `PWM`: PWM servo, such as SG90.
- `SCS`: Serial servo, such as Feetech SCS0009.
- `DYN_XL330`: Dynamixel XL330.
- `RT_DYN_XL330`: RT version of Dynamixel XL330.

---

## Configuration Files

### SC_BasicConfig.yaml
Basic configuration file. Defines servo pin numbers, initial positions, movement ranges, speed, and other settings.

### SC_SecConfig.yaml
Personal information configuration file. Defines the WiFi SSID and password, API keys, and other settings.

### SC_ExtConfig.yaml
Extended configuration file. Describes application-specific settings.

---

## Usage Example

### Initializing and Moving Servos
```cpp
#include <M5Unified.h>
#include "Stackchan_servo.h"

StackchanSERVO servo;

void setup() {
  M5.begin();
  servo.begin(33, 90, 0, 32, 90, 0, ServoType::PWM); // Initialize servos
  servo.moveX(45, 1000); // Move the X-axis to 45 degrees over 1 second
  servo.moveY(60, 1000); // Move the Y-axis to 60 degrees over 1 second
  servo.moveXY(90, 90, 1000); // Face forward over 1 second
}

void loop() {
  // Repeated processing
}
```

---
