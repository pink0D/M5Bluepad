#ifndef _M5_UNIT_8SERVO_H_
#define _M5_UNIT_8SERVO_H_

#include <Arduino.h>
#include <Wire.h>

#define M5_UNIT_8SERVO_DEFAULT_ADDR         0x25
#define M5_UNIT_8SERVO_MODE_REG             0x00
#define M5_UNIT_8SERVO_OUTPUT_CTL_REG       0x10
#define M5_UNIT_8SERVO_DIGITAL_INPUT_REG    0x20
#define M5_UNIT_8SERVO_ANALOG_INPUT_8B_REG  0x30
#define M5_UNIT_8SERVO_ANALOG_INPUT_12B_REG 0x40
#define M5_UNIT_8SERVO_SERVO_ANGLE_8B_REG   0x50
#define M5_UNIT_8SERVO_SERVO_PULSE_16B_REG  0x60
#define M5_UNIT_8SERVO_RGB_24B_REG          0x70
#define M5_UNIT_8SERVO_PWM_8B_REG           0x90
#define M5_UNIT_8SERVO_CURRENT_REG          0xA0
#define JUMP_TO_BOOTLOADER_REG              0xFD
#define FIRMWARE_VERSION_REG                0xFE
#define I2C_ADDRESS_REG                     0xFF

#define M5_UNIT_8SERVO_FW_VERSION_REG 0xFE
#define M5_UNIT_8SERVO_ADDRESS_REG    0xFF

typedef enum {
    DIGITAL_INPUT_MODE = 0,
    DIGITAL_OUTPUT_MODE,
    ADC_INPUT_MODE,
    SERVO_CTL_MODE,
    RGB_LED_MODE,
    PWM_MODE
} extio_io_mode_t;

typedef enum { _8bit = 0, _12bit } extio_anolog_read_mode_t;

class M5Unit8Servos {
   private:
    uint8_t _addr;
    TwoWire *_wire;
    uint8_t _sda;
    uint8_t _scl;
    bool writeBytes(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length);
    bool readBytes(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length);

   public:
    bool begin(TwoWire *wire = &Wire, uint8_t sda = 21, uint8_t scl = 22,
               uint8_t addr = M5_UNIT_8SERVO_DEFAULT_ADDR);
    bool setAllPinMode(extio_io_mode_t mode);
    bool setOnePinMode(uint8_t pin, extio_io_mode_t mode);
    bool setDeviceAddr(uint8_t addr);
    bool setServoAngle(uint8_t pin, uint8_t angle);
    bool setServoPulse(uint8_t pin, uint16_t pulse);
    bool setPWM(uint8_t pin, uint8_t angle);
    bool setLEDColor(uint8_t pin, uint32_t color);
    bool setDigitalOutput(uint8_t pin, uint8_t state);
    bool getDigitalInput(uint8_t pin);
    uint16_t getAnalogInput(uint8_t pin, extio_anolog_read_mode_t bit = _8bit);
    float getServoCurrent(void);
    uint8_t getVersion();
    uint8_t getFirmwareVersion(void);
    void jumpBootloader(void);
    uint8_t setI2CAddress(uint8_t addr);
    uint8_t getI2CAddress(void);
};

#endif