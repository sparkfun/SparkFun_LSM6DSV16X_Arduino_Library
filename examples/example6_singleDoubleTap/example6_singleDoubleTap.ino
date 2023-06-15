/*
  example6-single/Double Tap

  This example shows the how to setup the device for tap detection.

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

lsm6dsv16x_all_sources_t lsmSource;

void setup()
{

    Wire.begin();

    Serial.begin(115200);
    while (!Serial)
    {
    }

    Serial.println("LSM6DSV16X Example 6 - Single and Double Tap Interrupts");

    if (!myLSM.begin())
    {
        Serial.println("Did not begin.");
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

    Serial.println("Reset.");
    Serial.println("Applying settings.");
    delay(100);

    // Accelerometer and Gyroscope registers will not be updated
    // until read.
    myLSM.enableBlockDataUpdate();

    myLSM.enableTapInterrupt();

    // If routing to a pin you can clear the follow commented function.
    // myLSM.setIntSingleTap();

    // Direction to detect, we'll do just the z direction which
    lsm6dsv16x_tap_detection_t directionEnable;
    lsm6dsv16x_tap_detection_t getDirectionEnable;
    directionEnable.tap_x_en = 0;
    directionEnable.tap_y_en = 0;
    directionEnable.tap_z_en = 1;
    myLSM.setTapDirection(directionEnable);

    myLSM.getTapDirection(&getDirectionEnable);
    Serial.println("---- Enabled Axes -----");
    Serial.print("X-axis: ");
    Serial.println(getDirectionEnable.tap_x_en);
    Serial.print("Y-axis: ");
    Serial.println(getDirectionEnable.tap_y_en);
    Serial.print("Z-axis: ");
    Serial.println(getDirectionEnable.tap_z_en);

    lsm6dsv16x_tap_thresholds_t tapThreshold;
    lsm6dsv16x_tap_thresholds_t getTapThreshold;
    tapThreshold.x = 0;
    tapThreshold.y = 0;
    tapThreshold.z = 2;

    myLSM.setTapThresholds(tapThreshold);
    myLSM.getTapThresholds(&getTapThreshold);
    Serial.println("---- Thresholds for each Axis -----");
    Serial.print("X-axis: ");
    Serial.println(getTapThreshold.x);
    Serial.print("Y-axis: ");
    Serial.println(getTapThreshold.y);
    Serial.print("Z-axis: ");
    Serial.println(getTapThreshold.z);

    lsm6dsv16x_tap_time_windows_t tapWindows;
    lsm6dsv16x_tap_time_windows_t getTapWindows;
    tapWindows.shock = 1;
    tapWindows.quiet = 1;
    tapWindows.tap_gap = 7;

    myLSM.setTapTimeWindows(tapWindows);
    myLSM.getTapTimeWindows(&getTapWindows);
    Serial.println("---- Settings for Tap Detection-----");
    Serial.print("Shock: ");
    Serial.println(getTapWindows.shock);
    Serial.print("Quiet: ");
    Serial.println(getTapWindows.quiet);
    Serial.print("Tap Gap: ");
    Serial.println(getTapWindows.tap_gap);

    // LSM6DSV16X_ONLY_SINGLE is the other argument for the following function.
    lsm6dsv16x_tap_mode_t getTap;
    myLSM.setTapMode(LSM6DSV16X_BOTH_SINGLE_DOUBLE);
    myLSM.getTapMode(&getTap);
    Serial.print("Tap Mode (0 = Single, 1 = Single/Double): ");
    Serial.println((uint8_t)getTap);

    // Set the output data rate and precision of the accelerometer
    myLSM.setAccelDataRate(LSM6DSV16X_ODR_AT_480Hz);
    myLSM.setAccelFullScale(LSM6DSV16X_8g);

    Serial.println("Ready.");
}

void loop()
{

    // Polling for interrupt bits
    myLSM.getAllInterrupts(&lsmSource);
    if (lsmSource.single_tap || lsmSource.double_tap)
    {
        Serial.println("Tap Detected");
    }

    delay(10);
}
