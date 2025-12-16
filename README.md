# Overview
M5Bluepad library features extension classes for easier use of [Bluepad32](https://github.com/ricardoquesada/bluepad32) with [M5Stack](https://m5stack.com/) units.

# Bluetooth remote control with M5Stack Atom-Lite and Atomic Motion Base
[**Atomic Motion Base**](https://docs.m5stack.com/en/atom/Atomic_Motion_Base_v1.2) is kind of Arduino shield for controlling servo motors. It's designed to be used with Atom series controllers from M5Stack.

[**Atom-Lite**](https://docs.m5stack.com/en/core/ATOM%20Lite) is based on ESP32 with Bluetooth Classic support so it can establish wireless connection to most modern gamepads, including XBox Wireless Controller and Sony DualShock/DualSense. 

[**Unit 8Servos**](https://docs.m5stack.com/en/unit/8Servos%20Unit) can be used to connect additional 8x Servos so up to 14x Servos and 2x DC motors can be attached in total.

Most M5Stack units have LEGO-compatible mounts so if used together with GeekServo motors, this combo makes possible building small size yet powerful and extensible remote control machines like cars or robots.

<a href="https://postimg.cc/QBHNP1GQ" target="_blank"><img src="https://i.postimg.cc/QBHNP1GQ/IMG-1619.jpg" alt="IMG-1619"></a>
<a href="https://postimg.cc/NK0Q9mMB" target="_blank"><img src="https://i.postimg.cc/NK0Q9mMB/IMG-1613.jpg" alt="IMG-1613"></a>
<a href="https://postimg.cc/640NTpx6" target="_blank"><img src="https://i.postimg.cc/640NTpx6/IMG-1620.jpg" alt="IMG-1620"></a>
<a href='https://postimg.cc/d7QBRRrG' target='_blank'><img src='https://i.postimg.cc/d7QBRRrG/IMG-1677.jpg' alt='IMG-1677'></a>

[**YouTube demo**](https://www.youtube.com/watch?v=p6Zdq-76Pew)

# M5Stack and Power Functions motors

[**Atomic HDriver Base**](https://docs.m5stack.com/en/atom/Atomic%20H-Driver%20Base) features a  DC-motor driver capable to be powered from a 3S LiPo battery. It's also compatible with Atom controllers, so together with a GeekServo this setup enables building cars with proportional steering.

<a href='https://postimg.cc/kVfRLcym' target='_blank'><img src='https://i.postimg.cc/kVfRLcym/IMG-1678.jpg' alt='IMG-1678'></a>

[More info](https://github.com/pink0D/M5Bluepad/wiki/Attaching-Power-Functions-motor-to-M5Stack-Atomic-HDriver-Base)

# M5Stack and Mould King modules

[**Atom-Lite**](https://docs.m5stack.com/en/core/ATOM%20Lite) and [**Atomic HDriver Base**](https://docs.m5stack.com/en/atom/Atomic%20H-Driver%20Base) can be used together to make an adapter which can control Mould King 4.0 or 6.0 modules with a gamepad. An additional **GeekServo** can be attached to the grove port.

<a href="https://postimg.cc/bGc1fvkZ" target="_blank"><img src="https://i.postimg.cc/bGc1fvkZ/Mould_King_M5_cover.jpg" alt="Mould_King_M5_cover"></a>

[More info](https://github.com/pink0D/M5Bluepad/wiki/Controlling-Mould-King-modules-and-GeekServo-with-ESP32)


# Using BluepadHub with other controllers
While this library was initially created for using with Atom-Lite, it is also compatible with other M5Stack controllers based on 
ESP32-PICO-D4, ESP32-PICO-V3 or ESP32-D0WD chips. 

The main **BluepadHub** class in this library is independent from M5Stack hardware, so it can be used to implement bluetooth remote control with many other boards based on **ESP32** chips supporting **Bluetooth Classic**, like ESP32-PICO-MINI02 or ESP-WROOM-32.

| Supported M5Stack controllers  | Unsupported controllers   |
| :---           |    :----        |   
| :white_check_mark: Atom-Lite   | :x: AtomS3-Lite|
| :white_check_mark: Atom-Matrix | :x: AtomS3|
| :white_check_mark: Atom-Echo   | :x: Atom EchoS3R|
| :white_check_mark: Core2   | :x: CoreS3|
| :white_check_mark: Core Basic   |  |
| :white_check_mark: StickC-Plus2    |  |
| :white_check_mark: All controllers based on ESP32-PICO-D4, ESP-32-PICO-V3 and ESP32-D0WD   | :x: All controllers based on ESP32-S3, ESP32-C3, and ESP32-C6|

# Supported servo and motor control units

Description of some most common setups is available on this [wiki page](https://github.com/pink0D/M5Bluepad/wiki/M5Stack-unit-combos). 

| Unit  | Status   |
| :---           |    :----        |   
| [M5Stack Atomic Motion Base](https://docs.m5stack.com/en/atom/Atomic_Motion_Base_v1.2)   | :white_check_mark: Supported |
| [M5Stack Atomic HDriver Base](https://docs.m5stack.com/en/atom/Atomic%20H-Driver%20Base)  | :white_check_mark: Supported |
| [M5Stack Unit 8Servos](https://docs.m5stack.com/en/unit/8Servos%20Unit)  | :white_check_mark: Supported |
| [M5Stack Unit HBridge](https://docs.m5stack.com/en/unit/HBridge%20v1.1%20Unit)   | Planned |
| **Mould King Power Module 4.0**  | :white_check_mark: Supported |
| **Mould King Power Module 6.0**  | :white_check_mark: Supported |
| **GeekServo**  | :white_check_mark: Supported |
| Other units and devices | Can be impemented with custom code in sketch |

# Using the library

1. Install boards and libraries in Arduino IDE ([wiki](https://github.com/pink0D/M5Bluepad/wiki/Arduino-IDE-configuration))
2. Start with uploading [TestBluepad32](https://github.com/pink0D/M5Bluepad/blob/main/examples/TestBluepad32/TestBluepad32.ino) example to test connection between your gamepad and the controller
3. Continue to explore [AtomicLiteMotion](https://github.com/pink0D/M5Bluepad/blob/main/examples/AtomicLiteMotion/AtomicLiteMotion.ino) sketch and learn how BluepadHub handles gamepad's input to control motors and servos
4. Check out more [examples](https://github.com/pink0D/M5Bluepad/tree/main/examples) for supported M5Stack units

# License & credits
- M5Bluepad is open source and licensed under the MIT License
- Bluepad32, on which the M5Bluepad greatly depends, is also open source, licensed under Apache 2. Bluepad32 depends on the [BTstack](https://github.com/bluekitchen/btstack) made by BlueKitchen GmbH. BTStack is free to use for personal open source projects, but commercial for other uses
- Mould King BLE protocol was originally implemented by **[@J0EK3R](https://github.com/J0EK3R)** and **[@vicocz](https://github.com/vicocz)** in [brickcontroller2](https://github.com/vicocz/brickcontroller2) 

# Contacts
EMail: [pink0D.github@gmail.com](mailto:pink0d.github@gmail.com)
