/*
  example1-basic

  This example shows the basic settings and functions for retrieving accelerometer
	and gyroscopic data. 
	Please refer to the header file for more possible settings, found here:
	..\SparkFun_6DoF_ISM330DHCX_Arduino_Library\src\sfe_ism330dhcx_defs.h

  Written by Elias Santistevan @ SparkFun Electronics, August 2022

	Product:

		https://www.sparkfun.com/products/19764

  Repository:

		https://github.com/sparkfun/SparkFun_6DoF_ISM330DHCX_Arduino_Library

  SparkFun code, firmware, and software is released under the MIT 
	License	(http://opensource.org/licenses/MIT).
*/

#include <Wire.h>
#include "SparkFun_LSM6DSV16X.h"

SparkFun_LSM6DSV16X myLSM; 

int16_t qvar = 0; 

void setup(){

	Wire.begin();


	Serial.begin(115200);

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
	
	myLSM.enableBlockDataUpdate();

	myLSM.enableAhQvar();
	Serial.print("Qvar Enabled: ");
	Serial.println(myLSM.getQvarMode());
	delay(1000);

	myLSM.setAccelMode(LSM6DSV16X_XL_HIGH_PERFORMANCE_MD);
	myLSM.setGyroMode(LSM6DSV16X_GY_HIGH_PERFORMANCE_MD);
	
	// Set the output data rate and precision of the accelerometer
	myLSM.setAccelDataRate(LSM6DSV16X_ODR_AT_15Hz);
	myLSM.setAccelFullScale(LSM6DSV16X_2g); 

	// Set the output data rate and precision of the gyroscope
	myLSM.setGyroDataRate(LSM6DSV16X_ODR_AT_15Hz);
	myLSM.setGyroFullScale(LSM6DSV16X_2000dps); 

//	// Enable filter settling.
//	myLSM.enableFilterSettling();
//
//	// Turn on the accelerometer's filter and apply settings. 
//	myLSM.enableAccelLP2Filter();
//	myLSM.setAccelLP2Bandwidth(LSM6DSV16X_XL_STRONG);
//
	// Enable Qvar channels.



}

void loop(){

	// Check if data is available. 
	if( myLSM.checkQvar() )
	{
		myLSM.getRawQvar(&qvar);
		//Serial.print("Qvar: ");
		Serial.println(qvar);
	}

	delay(4);
}

