/*
  example5-basic

  This example shows the basic setup and functions for the QVar sensing pins (AH1/AH2).
    Before running this example you'll need to cut the SDX and SCX jumpers on the bottom
    side of the board. These pins are not broken out on the micro.

    The Qvar pins are analog input pins and can be used for a wide range of applications.
    However, to use them on the SparkFun 6DoF LSM6DSV16X you'll need to add some kind of
    filter depending on your application, to the unpopulated pads. You can see the two
    application notes from STMicroelectronic's at the following link for more information.

    https://www.st.com/en/mems-and-sensors/lsm6dsv16x.html#documentation

  Written by Elias Santistevan @ SparkFun Electronics, May, 2022

    Products:

    SparkFun 6DoF LSM6DSV16X (Qwiic):
        https://www.sparkfun.com/products/21325

    SparkFun Micro 6DoF LSM6DSV16X (Qwiic):
        https://www.sparkfun.com/products/21336

  Repository:
        https://github.com/sparkfun/SparkFun_LSM6DSV16X_Arduino_Library

  SparkFun code, firmware, and software is released under the MIT
    License	(http://opensource.org/licenses/MIT).
*/

#include "SparkFun_LSM6DSV16X.h"
#include <Wire.h>

SparkFun_LSM6DSV16X myLSM;

int16_t qvar = 0;

void setup()
{

    Wire.begin();

    Serial.begin(115200);
    while (!Serial)
    {
    }

    Serial.println("LSM6DSV16X Example 5 - Qvar Sensing");

    if (!myLSM.begin())
    {
        Serial.println("Did not begin, check your wiring and/or I2C address!");
        while (1)
            ;
    }

    // Reset the device to default settings. This if helpful is you're doing multiple
    // uploads testing different settings.
    myLSM.deviceReset();

    // Wait for it to finish reseting
    while (!myLSM.getDeviceReset())
    {
        delay(1);
    }

    Serial.println("Board has been Reset.");
    Serial.println("Applying settings.");
    delay(100);

    // Accelerometer and Gyroscope registers will not be updated
    // until read.
    myLSM.enableBlockDataUpdate();

    // Enable QVar pins.
    myLSM.enableAhQvar();

    // Check if the pin is enabled.
    // Serial.print("Qvar Enabled: ");
    // Serial.println(myLSM.getQvarMode());
    // delay(1000);

    // Set the output data rate and precision of the accelerometer
    myLSM.setAccelDataRate(LSM6DSV16X_ODR_AT_15Hz);
    myLSM.setAccelFullScale(LSM6DSV16X_2g);

    // Set the output data rate and precision of the gyroscope
    myLSM.setGyroDataRate(LSM6DSV16X_ODR_AT_15Hz);
    myLSM.setGyroFullScale(LSM6DSV16X_2000dps);

    // Change the internal impedance of the analog/Qvar pins
    // Check datasheet for the four possible values.
    // uint8_t impedance = 0x02;
    // myLSM.setQvarImpedance(impedance);

    Serial.println("Ready.");
}

void loop()
{

    // Check if data is available.
    if (myLSM.checkQvar())
    {
        myLSM.getRawQvar(&qvar);
        // Serial.print("Qvar: ");
        Serial.println(qvar);
    }

    delay(4);
}
