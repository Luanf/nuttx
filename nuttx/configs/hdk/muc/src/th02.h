/*
 */
#pragma once


#define TH02_ADDR                0x40 // device address

#define TH02_REG_STATUS          0x00
#define TH02_REG_DATA_H          0x01
#define TH02_REG_DATA_L          0x02
#define TH02_REG_CONFIG          0x03
#define TH02_REG_ID              0x11

#define TH02_STATUS_RDY_MASK     0x01

#define TH02_CMD_MEASURE_HUMI    0x01
#define TH02_CMD_MEASURE_TEMP    0x11

/**
 * @brief TH02 Temperature & Humidity Sensor library
 * @defgroup th02 libupm-th02
 * @ingroup seeed i2c temp
 */
int enable_TH02 (int bus, uint8_t addr);

/**
 * TH02 object destructor; basically, it closes the I2C connection.
 */
void disable_TH02(void);

/**
 * Gets the temperature value from the sensor.
 */
float getTemperature(void);

/**
 * Gets the humidity value from the sensor.
 */
float getHumidity(void);

/**
 * Gets the sensor status.
 */
int getStatus(void);
