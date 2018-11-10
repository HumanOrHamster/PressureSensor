#include <LPS.h>
#include <Wire.h>

// if quering the i2c line the sensor will return the following id
#define LPS25H_WHOAMI_ID   0xBD

/*According to spec sheet pg 16: If the SA0 pad is connected to
   voltage supply, LSb is ‘1’ (address 1011101b), otherwise if the SA0 pad is connected to
   ground, the LSb value is ‘0’ (address 1011100b).*/
#define SA_LPS25H  0b1011101

void LPS::init(void)
{
        // BARO SETUP
        //0xB0 0b10110000;
        // PDF = 1 Active mode, ODR = 011 12.5 Hz

        writeToReg(SA_LPS25H,CTRL_REG1,0xB0);
        // future optimization so as to not  hardcode in the settings for CTRL_REG1
}

void LPS::writeToReg(int addr, int reg, int value)
{
        Wire.beginTransmission(addr);
        Wire.write(reg);
        Wire.write(value);
        Wire.endTransmission();
}


uint32_t LPS::readRawPressure(void)
{
        Wire.beginTransmission(SA_LPS25H);
        // need to add a 1 bit to the MSB (SUPER STUPID)
        Wire.write(PRESS_OUT_XL | (1 << 7)); //
        Wire.endTransmission();

        // request 3 bytes
        // get presure low, mid, then high
        Wire.requestFrom(SA_LPS25H, 3);

        while (Wire.available() < 3); // wait to get bytes

        // need to cast the type otherwise there is some weird issue
        // where bit shifting will add in extra "1's" for the MSBs and when OR
        // with other bytes this  will give incorrect results.
        // Casting will preserve the  "0's" in the front of the MSBs
        // i.e. 1000 << 4 for uint16 is 0000 0000 1000 0000 instead of 1111 1111 1000 0000

        // merge readings and return
        return Wire.read() | (uint16_t)(Wire.read() << 8) | (uint32_t) Wire.read() << 16;
}

int16_t LPS::readRawTemp(void)
{
        Wire.beginTransmission(SA_LPS25H);
        // need to add a 1 bit to the MSB (SUPER STUPID)
        Wire.write(TEMP_OUT_L | (1 << 7)); //
        Wire.endTransmission();

        // request two bytes, low then high, temperature data
        // is expressed as 2's complements
        Wire.requestFrom(SA_LPS25H, 2);

        while (Wire.available() < 2); // wait to get bytes

        // merge readings and return
        return Wire.read() | Wire.read() << 8;
}

float LPS::getPresureHpa(void)
{
        return (float)readRawPressure() / scaleFactor;
}

float LPS::getMetersFromPressure(float pressureHpa, float standardPressureHpa)
{

        // based on the formula  P/P0 = (T/T0)^(-g/(a*R))
        // where
        // P0 = 101325 Pa
        // T0 = 288.15 K
        // a = -6.5E-3 , lapse rate K/meter
        // R = 287.058 J/kg/K
        // g = 9.806650 m/s^2

        // Solving for pressure altitude:
        // T = T0  + a*h
        // letting gamma = -g/(a*R) = 5.2557859
        // then P/P0 = ((T0+ah)/T0)^(gamma)
        // solving for h:
        // h = (T0/a)*((P/P0)^(1/gamma)-1) (meters)
        // final implementation has the 1 as positive if we bring the sign
        // from the parameter "a" inside the parentheses

        return (44330.7692308)*(1.0-pow((pressureHpa/standardPressureHpa),0.19026650));
}

void LPS::setBasePressure(void)
{
        // get average readings
        float avgPressure = 0.0; // hpa
        int iter = 10;

        // get current pressure and set that as the base pressure (i.e. p0)
        for (int i = 0; i < iter; i++ )
        {
                avgPressure += getPresureHpa();
                // Serial.println(avgPressure);
                delay(100);
        }

        avgPressure /= iter;
        /*
           Serial.println("Setting the basepressure to an average pressure reading ");
           Serial.print("of (hpa) : ");
           Serial.println(avgPressure);
         */

        // set public member basePressure to the averaged pressure readings
        basePressure = avgPressure;
}

float LPS::getDeltaAltitudeFromBase(float pressure)
{
        return getMetersFromPressure(pressure,basePressure);
}

float LPS::getFtFromPressure(float pressureHpa, float standardPressureHpa){
        return (145442.1562689)*(1.0-pow((pressureHpa/standardPressureHpa),0.19026650));
}

float LPS::getTempF(float OptOffset = 0.0)
{
        int16_t rawTemp = readRawTemp();
        return (42.5 + rawTemp/480.0)*(1.8) + 32.0 + OptOffset;
}

float LPS::getTempC(float OptOffset = 0.0 )
{
        int16_t rawTemp = readRawTemp();
        // according to the spec sheet pg 34
        return 42.5 + rawTemp/480.0 + OptOffset;
}
