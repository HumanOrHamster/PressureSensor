#include <Arduino.h>
#include <FirstOrderFilter.h>
#include <LPS.h>
#include <Wire.h>



LPS ps;
FirstOrderFilter fofAltitude; // first order filtered Altitude

void setup() {
        // put your setup code here, to run once:
        Serial.begin(115200);  // Open serial port
        Wire.begin();
        ps.init();

        // set the base pressure used for computing delta altitude
        ps.setBasePressure();

        // get delta altitude to initialize first order filter
        float initAltitude = ps.getDeltaAltitudeFromBase(ps.basePressure);

        // filter parameters
        float cutOffFreq = .5;  // Hz
        float Ts = 0.1; // sample Time sec

        fofAltitude.initFilter(cutOffFreq,Ts,initAltitude);


}

void loop() {
        // get pressure
        float pressure = ps.getPresureHpa();

        // 1023 CALIBRATED AT HB
        float presAltitudeMeters = ps.getMetersFromPressure(pressure,1023);

        // if we assuming that we want delta feet from the current base pressure

        //float presAltitudeMeters = ps.getDeltaAltitudeFromBase(pressure);

        // apply first order filter
        presAltitudeMeters = fofAltitude.computeOutput(presAltitudeMeters);

        /*
           Serial.print("pressure (hpa): ");
           Serial.print(pressure);
           Serial.print(" , filtered delta altitude (meters) : ");
           Serial.print(presAltitudeMeters);

           // get temperature
           float tempC = ps.getTempF(-18);

           Serial.print(", tempurature (F): ");
           Serial.println(tempC);
         */
        Serial.println(presAltitudeMeters);

        // float get temperature

        delay(100);
}
