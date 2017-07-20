#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/util.h>
#include <nuttx/i2c.h>

#include <arch/board/th02.h>

static uint8_t m_addr = TH02_ADDR;
static struct i2c_dev_s  *i2c_dev;

int  enable_TH02 (int bus, uint8_t addr) {
	
    dbg("Enabling TH02 i2c device \n");
	
    i2c_dev = up_i2cinitialize(bus);
    if (!i2c_dev)
    {
        dbg("ERROR: failed to init i2c \n");
        return -ENODEV;
    }
	
    m_addr = addr;
	
    if (I2C_SETADDRESS(i2c_dev, m_addr, 7) != 0) {
        dbg("ERROR: failed to init i2c address %d \n", m_addr);
        return -ENODEV;		
    };
    I2C_SETFREQUENCY(i2c_dev, 400000);

    return 0;
}

void disable_TH02() {
	
    dbg("Disabling TH02 i2c device \n");
	
    up_i2cuninitialize(i2c_dev);
}

uint8_t reg_read(uint8_t reg)
{
     uint8_t reg_val[1];
     int ret;

     I2C_SETADDRESS(i2c_dev, m_addr, 7);

     ret = I2C_WRITEREAD(i2c_dev, (uint8_t *)&reg, sizeof(reg),
              reg_val, sizeof(reg_val));
     if (ret)
         return ret;

     ret = *((uint8_t *)reg_val);
      
     return ret;
}


int reg_write(uint8_t reg, uint8_t val)
{
    uint8_t buf[2];

    I2C_SETADDRESS(i2c_dev, m_addr, 7);

    buf[0] = reg;
    buf[1] = val;

    return I2C_WRITE(i2c_dev, (uint8_t *)buf, 3);
}


float getTemperature() {

    dbg("TEMPERATURE 1\n\n");
    uint16_t temperature = 0;

    /* Start a new temperature conversion */
    if (reg_write(TH02_REG_CONFIG, TH02_CMD_MEASURE_TEMP)) {
        dbg("TEMPERATURE ERROR\n");
        return 0.0;
    }

    dbg("TEMPERATURE 2\n\n");

    /* Wait until conversion is done */
    while (getStatus() == 0x00);
	
    temperature = reg_read(TH02_REG_DATA_H) << 8;
    temperature |= reg_read(TH02_REG_DATA_L);
    temperature >>= 2;

    dbg("TEMPERATURE 3\n\n");

    dbg("temperature = %5.1f \n", (temperature / 32.0) - 50.0);

    dbg("TEMPERATURE 4\n\n");


    return ((temperature / 32.0) - 50.0);
}

float getHumidity () {

    dbg("HUMIDADE 1\n\n");
    uint16_t humidity = 0;

    /* Start a new humidity conversion */
    if (reg_write(TH02_REG_CONFIG, TH02_CMD_MEASURE_HUMI)) {
        dbg("HUMIDITY ERROR\n");
        return 0.0;
    }

    dbg("HUMIDITY 2\n\n");

    /* Wait until conversion is done */
    while (getStatus() == 0x00);

    humidity = reg_read(TH02_REG_DATA_H) << 8;
    humidity |= reg_read(TH02_REG_DATA_L);
    humidity >>= 4;

    dbg("HUMIDITY 3\n\n");

    dbg("humidity = %5.1f \n", (humidity / 16.0) - 24.0);

    dbg("HUMIDITY 4\n\n");

    return ((humidity / 16.0) - 24.0);
}

int getStatus () {
	
    uint8_t status = reg_read(TH02_REG_STATUS);

    if (status & TH02_STATUS_RDY_MASK) {
        return 0x00;           // NOT ready
    }
    else {
        return 0x01;            // ready
    }
}

