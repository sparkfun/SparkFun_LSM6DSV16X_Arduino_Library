/*
  example2-interrupt

  This example shows the basic settings and functions for retrieving accelerometer
	data. In addition we're setting the data ready signal to interrupt pin one in an
	active high configuration and show additional ways in which the interrupts
	can be configured. 

  Written by Elias Santistevan @ SparkFun Electronics, May 2022

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

// Structs for X,Y,Z data
SparkFun_LSM6DSV16X myLSM; 

// Structs for X,Y,Z data
sfe_lsm_data_t accelData; 


// Interrupt pin
byte interrupt_pin = D1; 
byte pin_one = 1; 
byte pin_two = 1; 

void setup(){

	// Set the interrupt to INPUT
	pinMode(interrupt_pin, INPUT);
	Serial.begin(115200);

	Wire.begin();

	if( !myLSM.begin() ){
		Serial.println("Did not begin.");
		while(1);
	}

	// Reset the device to default settings. This is helpful if you're doing multiple
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
	
	// Set the output data rate and precision of the accelerometer
	myLSM.setAccelDataRate(LSM6DSV16X_ODR_AT_7Hz5);
	myLSM.setAccelFullScale(LSM6DSV16X_16g); 

	// Turn on the accelerometer's filter and apply settings. 
	myLSM.enableAccelLP2Filter();
	myLSM.setAccelLP2Bandwidth(LSM6DSV16X_XL_STRONG);

	// Set the accelerometer's status i.e. the data ready to interrupt one. 
	// Commented out just below is the function to send the data ready
	// to interrupt two. 

	myLSM.setIntAccelDataReady();
	//myLSM.setIntAccelDataReady(pin_two);


	// We can just as easily set the gyroscope's data read signal to either interrupt

	//myLSM.setIntGyroDataReady();
	//myLSM.setIntGyroDataReady(pin_two);


	// Uncommenting the function call below will change interrupt TWO
	// active LOW instead of HIGH.

	//myLSM.setInt2DENPolarity();

	// This function call will modify which "events" trigger an interrupt. No 
	// argument has been given, please refer to the datasheet for more 
	// information.

	// myLSM.setIntRoute(lsm6dsv16x_pin_int_route_t val);

	// This function changes the latching behaviour of the interrupts to pulsed.
	// myLSM.setDataReadyMode();
}

void loop(){

	if( digitalRead(interrupt_pin) == HIGH ){
		myLSM.getAccel(&accelData);
		Serial.print("Accelerometer: ");
		Serial.print("X: ");
		Serial.print(accelData.xData);
		Serial.print(" ");
		Serial.print("Y: ");
		Serial.print(accelData.yData);
		Serial.print(" ");
		Serial.print("Z: ");
		Serial.print(accelData.zData);
		Serial.println(" ");
	}

	delay(100);
}

