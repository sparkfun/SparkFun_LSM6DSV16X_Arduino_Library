/*
  example4-faster_I2C

  This example shows how to retrieve accelerometer and gyroscopic data as fast
  as the I2C bus will allow.

  If you want to run the accel and gyro at 416Hz, you need to use 400kHz I2C.
  See the Wire.setClock(400000); at the start of setup().

  This example also allows you to run the accel and gyro at different rates,
  should you want to.

  Tested on ATmega328P (16MHz). Please see this issue for more detail:
  https://github.com/sparkfun/SparkFun_6DoF_ISM330DHCX_Arduino_Library/issues/7#issuecomment-1295767690

  Written by Paul Clark @ SparkFun Electronics, October 2022
  Based on original work by Elias Santistevan

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

// Structs for X,Y,Z data
sfe_lsm_data_t accelData;
sfe_lsm_data_t gyroData;

void setup()
{

    Wire.begin();

    // Uncomment the next line to use 400kHz I2C. Essential when running the accel and gyro at 416Hz or faster.
    // Wire.setClock(400000);

    Serial.begin(115200);
    while (!Serial)
    {
    }

    Serial.println("LSM6DSV16X Example 4 - Faster I2C Example");

    while (!myLSM.begin())
    {
        Serial.println("ISM did not begin. Please check the wiring...");
        delay(1000);
    }

    // Reset the device to default settings. This if helpful is you're doing multiple
    // uploads testing different settings.
    myLSM.deviceReset();

    // Wait for it to finish reseting
    while (!myLSM.getDeviceReset())
    {
        delay(1);
    }

    Serial.println("Reset.");
    Serial.println("Applying settings.");
    delay(100);

    // Accelerometer and Gyroscope registers will not be updated
    // until read.
    myLSM.enableBlockDataUpdate();

    // Set the output data rate and precision of the accelerometer
    myLSM.setAccelDataRate(LSM6DSV16X_ODR_AT_7Hz5);
    myLSM.setAccelFullScale(LSM6DSV16X_16g);

    // Set the output data rate and precision of the gyroscope
    myLSM.setGyroDataRate(LSM6DSV16X_ODR_AT_15Hz);
    myLSM.setGyroFullScale(LSM6DSV16X_2000dps);

    // Enable filter settling.
    myLSM.enableFilterSettling();

    // Turn on the accelerometer's filter and apply settings.
    myLSM.enableAccelLP2Filter();
    myLSM.setAccelLP2Bandwidth(LSM6DSV16X_XL_STRONG);

    // Turn on the gyroscope's filter and apply settings.
    myLSM.enableGyroLP1Filter();
    myLSM.setGyroLP1Bandwidth(LSM6DSV16X_GY_ULTRA_LIGHT);

    Serial.println("Ready.")
}

void loop()
{
    float totalAccelX = 0; // Sum the accel readings
    float totalAccelY = 0;
    float totalAccelZ = 0;
    float countA = 0;     // Count the readings
    float totalGyroX = 0; // Sum the gyro readings
    float totalGyroY = 0;
    float totalGyroZ = 0;
    float countG = 0; // Count the readings

    Serial.println("Recording accel and gyro data for 10 seconds...");
    Serial.println();
    delay(10); // Wait for the Serial.print to complete

    unsigned long startTime = millis(); // Keep track of time

    // Note: we can't do prints inside the while loop. They slow things down too much...

    while (millis() < (startTime + 10000)) // Loop for 10 seconds
    {
        // Check if accel data is available.
        if (myLSM.checkAccelStatus())
        {
            myLSM.getAccel(&accelData);     // Read the accel data
            totalAccelX += accelData.xData; // Sum it
            totalAccelY += accelData.yData;
            totalAccelZ += accelData.zData;
            countA += 1; // Increment the count
        }

        // Check if gyro data is available.
        if (myLSM.checkGyroStatus())
        {
            myLSM.getGyro(&gyroData);     // Read the gyro data
            totalGyroX += gyroData.xData; // Sum it
            totalGyroY += gyroData.yData;
            totalGyroZ += gyroData.zData;
            countG += 1; // Increment the count
        }
    }

    Serial.print("Read ");
    Serial.print(countA, 0);
    Serial.println(" Accel values.");
    Serial.println("Accel averages are:");
    Serial.println(totalAccelX / countA);
    Serial.println(totalAccelY / countA);
    Serial.println(totalAccelZ / countA);
    Serial.println();
    Serial.print("Read ");
    Serial.print(countG, 0);
    Serial.println(" Gyro values.");
    Serial.println("Gyro averages are:");
    Serial.println(totalGyroX / countG);
    Serial.println(totalGyroY / countG);
    Serial.println(totalGyroZ / countG);
    Serial.println();

    Serial.println("Final readings were:");
    Serial.print("Accel: ");
    Serial.print("X: ");
    Serial.print(accelData.xData);
    Serial.print(" ");
    Serial.print("Y: ");
    Serial.print(accelData.yData);
    Serial.print(" ");
    Serial.print("Z: ");
    Serial.print(accelData.zData);
    Serial.println(" ");
    Serial.print("Gyro: ");
    Serial.print("X: ");
    Serial.print(gyroData.xData);
    Serial.print(" ");
    Serial.print("Y: ");
    Serial.print(gyroData.yData);
    Serial.print(" ");
    Serial.print("Z: ");
    Serial.println(gyroData.zData);
    Serial.println();

    delay(1000);
}
