#ifndef LPS_h
#define LPS_h

// has the math lib
#include <Arduino.h>


/*
   NOTE TO SELF: Need to add a method to write calibration for pressure and
   temperature to EEPROM.
 */

class LPS
{
// private members here
float scaleFactor = 4096; // conversion factor from pg 7 / 46 of spec sheet


public:
// All parameters are publicly accessible
float basePressure = 1013.25; // 1013.25 is the default value

enum regAddr
{
        WHO_AM_I                = 0x0F, // this is just a generic who am i transmission
        CTRL_REG1               = 0X20,
        PRESS_OUT_XL            = 0X28,
        TEMP_OUT_L              = 0x2B,
};
// public members here

void init(void);
/*description:
   set up baro active along with refresh rate
 */
void writeToReg(int addr, int reg, int value);
/*description:
   write to register utility
   addr: i2c sensor address
   reg: register of the sensor
   value: the 8bit settings for the register
 */
uint32_t readRawPressure(void);
/*description:
   get raw altitude from sensor
 */
int16_t readRawTemp(void);
/*description:
   get raw temperature from sensor
 */
float getPresureHpa(void);
/*description:
   return pressure in Hpa using scalefactor
 */
float getMetersFromPressure(float pressureHpa, float standardPressureHpa = 1013.25);
/*description:
   compute meters from pressure using standard atmosphere model
   pressure at sealevel is defaulted to 1013.25, user change this for calibration
 */
float getFtFromPressure(float pressureHpa, float standardPressureHpa = 1013.25);
/*description:
   compute feet from pressure using standard atmosphere model
    pressure at sealevel is defaulted to 1013.25, user change this for calibration
 */
void setBasePressure(void);
/*description:
   set basePressure member value using average readings.
   This value is used to compute the delta height
   from the reference basePressure.
 */

float getDeltaAltitudeFromBase(float pressure);
/*description:
   compute the delta altitude relative to the basePressure.
   Assume basePressure is already set.
 */

float getTempF(float OptOffset = 0.0);
/*description:
   get temperature readings in Fahrenheit
   optional offset field for calibration
 */
float getTempC(float OptOffset = 0.0);
/*description:
   get temperature readings in Celcius
   optional offset field for calibration
 */

};

#endif
