#include "M5Unit8Servos.h"

bool M5Unit8Servos::begin(TwoWire *wire, uint8_t sda, uint8_t scl,
                           uint8_t addr) {
    _wire = wire;
    _addr = addr;
    _sda  = sda;
    _scl  = scl;
    _wire->begin(_sda, _scl);
    delay(10);
    _wire->beginTransmission(_addr);
    uint8_t error = _wire->endTransmission();
    if (error == 0) {
        return true;
    } else {
        return false;
    }
}

bool M5Unit8Servos::writeBytes(uint8_t addr, uint8_t reg, uint8_t *buffer,
                                uint8_t length) {
    _wire->beginTransmission(addr);
    _wire->write(reg);
    for (uint8_t i = 0; i < length; i++) {
        _wire->write(*(buffer + i));
    }
    if (_wire->endTransmission() == 0) return true;
    return false;
}

bool M5Unit8Servos::readBytes(uint8_t addr, uint8_t reg, uint8_t *buffer,
                               uint8_t length) {
    uint8_t index = 0;
    _wire->beginTransmission(addr);
    _wire->write(reg);
    _wire->endTransmission(false);
    if (_wire->requestFrom(addr, length)) {
        for (uint8_t i = 0; i < length; i++) {
            buffer[index++] = _wire->read();
        }
        return true;
    }
    return false;
}

bool M5Unit8Servos::setAllPinMode(extio_io_mode_t mode) {
    uint8_t data[8];
    memset(data, mode, 8);

    return writeBytes(_addr, M5_UNIT_8SERVO_MODE_REG, data, 8);
    for (uint8_t i = 0; i < 8; i++) {
        delay(1);
        if (writeBytes(_addr, i, &data[i], 1) == false) return false;
        delay(9);
    }
    return true;
}

bool M5Unit8Servos::setOnePinMode(uint8_t pin, extio_io_mode_t mode) {
    uint8_t data[8];
    memset(data, mode, 8);

    return writeBytes(_addr, pin, data, 1);
    for (uint8_t i = 0; i < 8; i++) {
        delay(1);
        if (writeBytes(_addr, i, &data[i], 1) == false) return false;
        delay(9);
    }
    return true;
}

bool M5Unit8Servos::setDeviceAddr(uint8_t addr) {
    if (writeBytes(_addr, M5_UNIT_8SERVO_ADDRESS_REG, &addr, 1)) {
        _addr = addr;
        return true;
    } else {
        return false;
    }
}

uint8_t M5Unit8Servos::getVersion() {
    uint8_t data = 0;
    readBytes(_addr, M5_UNIT_8SERVO_FW_VERSION_REG, &data, 1);
    return data;
}

bool M5Unit8Servos::setDigitalOutput(uint8_t pin, uint8_t state) {
    uint8_t reg = pin + M5_UNIT_8SERVO_OUTPUT_CTL_REG;
    return writeBytes(_addr, reg, &state, 1);
}

bool M5Unit8Servos::setLEDColor(uint8_t pin, uint32_t color) {
    if (pin > 7) return false;
    uint8_t data[3] = {0};
    data[0]         = (color >> 16) & 0xff;
    data[1]         = (color >> 8) & 0xff;
    data[2]         = color & 0xff;
    uint8_t reg     = pin * 3 + M5_UNIT_8SERVO_RGB_24B_REG;
    return writeBytes(_addr, reg, data, 3);
}

bool M5Unit8Servos::setServoAngle(uint8_t pin, uint8_t angle) {
    uint8_t reg = pin + M5_UNIT_8SERVO_SERVO_ANGLE_8B_REG;
    return writeBytes(_addr, reg, &angle, 1);
}

bool M5Unit8Servos::setPWM(uint8_t pin, uint8_t angle) {
    uint8_t reg = pin + M5_UNIT_8SERVO_PWM_8B_REG;
    return writeBytes(_addr, reg, &angle, 1);
}

bool M5Unit8Servos::setServoPulse(uint8_t pin, uint16_t pulse) {
    uint8_t data[2];
    uint8_t reg = pin * 2 + M5_UNIT_8SERVO_SERVO_PULSE_16B_REG;
    data[1]     = (pulse >> 8) & 0xff;
    data[0]     = pulse & 0xff;
    return writeBytes(_addr, reg, data, 2);
}

bool M5Unit8Servos::getDigitalInput(uint8_t pin) {
    uint8_t data;
    uint8_t reg = pin + M5_UNIT_8SERVO_DIGITAL_INPUT_REG;
    if (readBytes(_addr, reg, &data, 1)) {
        return data;
    }
    return 0;
}

uint16_t M5Unit8Servos::getAnalogInput(uint8_t pin,
                                        extio_anolog_read_mode_t bit) {
    if (bit == _8bit) {
        uint8_t data;
        uint8_t reg = pin + M5_UNIT_8SERVO_ANALOG_INPUT_8B_REG;
        if (readBytes(_addr, reg, &data, 1)) {
            return data;
        }
    } else {
        uint8_t data[2];
        uint8_t reg = pin * 2 + M5_UNIT_8SERVO_ANALOG_INPUT_12B_REG;
        if (readBytes(_addr, reg, data, 2)) {
            return (data[1] << 8) | data[0];
        }
    }
    return 0;
}

float M5Unit8Servos::getServoCurrent(void) {
    uint8_t data[4];
    float c;
    uint8_t *p;

    readBytes(_addr, M5_UNIT_8SERVO_CURRENT_REG, data, 4);
    p = (uint8_t *)&c;
    memcpy(p, data, 4);

    return c;
}

void M5Unit8Servos::jumpBootloader(void) {
    uint8_t value = 1;

    writeBytes(_addr, JUMP_TO_BOOTLOADER_REG, (uint8_t *)&value, 1);
}

uint8_t M5Unit8Servos::setI2CAddress(uint8_t addr) {
    _wire->beginTransmission(_addr);
    _wire->write(I2C_ADDRESS_REG);
    _wire->write(addr);
    _wire->endTransmission();
    _addr = addr;
    return _addr;
}

uint8_t M5Unit8Servos::getI2CAddress(void) {
    _wire->beginTransmission(_addr);
    _wire->write(I2C_ADDRESS_REG);
    _wire->endTransmission();

    uint8_t RegValue;

    _wire->requestFrom(_addr, 1);
    RegValue = Wire.read();
    return RegValue;
}

uint8_t M5Unit8Servos::getFirmwareVersion(void) {
    _wire->beginTransmission(_addr);
    _wire->write(FIRMWARE_VERSION_REG);
    _wire->endTransmission();

    uint8_t RegValue;

    _wire->requestFrom(_addr, 1);
    RegValue = Wire.read();
    return RegValue;
}
