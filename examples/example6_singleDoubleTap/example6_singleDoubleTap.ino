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

#include <Wire.h>
#include "SparkFun_LSM6DSV16X.h"

SparkFun_LSM6DSV16X myLSM; 

lsm6dsv16x_all_sources_t  lsmSource;
sfe_axis_data_t myAxis; 

void setup(){

	Wire.begin();

	Serial.begin(115200);
	while(!Serial){}

	if( !myLSM.begin() ){
		Serial.println("Did not begin.");
		while(1);
	}

	// Reset the device to default settings. This if helpful is you're doing multiple
	// uploads testing different settings. 
	myLSM.deviceReset();

	// Wait for it to finish reseting
	while( !myLSM.getDeviceReset() ){ 
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
	//myLSM.setIntSingleTap();

	// Direction to detect, the parameters are as follows: X-axis enabled, Y-axis enabled, Z-axis enabled
	myLSM.setTapDirection(false, false, true); 
	// Check 
	myLSM.getTapDirection(&myAxis);
	Serial.println("---- Enabled Axes -----");
	Serial.print("X-axis: ");
	Serial.println(myAxis.x);
	Serial.print("Y-axis: ");
	Serial.println(myAxis.y);
	Serial.print("Z-axis: ");
	Serial.println(myAxis.z);

	myLSM.setTapThresholds(0, 0, 2); 
	myLSM.getTapThresholds(&myAxis);
	Serial.println("---- Thresholds for each Axis -----");
	Serial.print("X-axis: ");
	Serial.println(myAxis.x);
	Serial.print("Y-axis: ");
	Serial.println(myAxis.y);
	Serial.print("Z-axis: ");
	Serial.println(myAxis.z);

	myLSM.setTapTimeWindows(1, 1, 7); 
	myLSM.getTapTimeWindows(&myAxis);
	Serial.println("---- Settings for Tap Detection-----");
	Serial.print("Shock: ");
	Serial.println(myAxis.x);
	Serial.print("Quiet: ");
	Serial.println(myAxis.y);
	Serial.print("Tap Gap: ");
	Serial.println(myAxis.z);

	myLSM.setTapMode(0x01); 
	Serial.print("Tap Mode (0 = Single, 1 = Single/Double): "); 
	Serial.println(myLSM.getTapMode()); 
	
	// Set the output data rate and precision of the accelerometer
	myLSM.setAccelDataRate(LSM6DSV16X_ODR_AT_480Hz);
	myLSM.setAccelFullScale(LSM6DSV16X_8g); 

	Serial.println("Ready.");

}

void loop(){

	// Polling for interrupt bits
	myLSM.getAllInterrupts( &lsmSource );
	if( lsmSource.single_tap || lsmSource.double_tap)
	{
		Serial.println("Tap Detected");
	}

	delay(10);

}

