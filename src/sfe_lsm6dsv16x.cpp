#include "sfe_lsm6dsv16x.h"

/// @brief Resets all registers. 
/// @param dataRate 
/// @return true on successful execution.

/// @brief Sets the struct that interfaces with STMicroelectronic's C Library.
/// @return true on successful execution.
bool QwDevLSM6DSV16X::init(void)
{
    //  do we have a bus yet? is the device connected?
    if (!_sfeBus->ping(_i2cAddress))
        return false;
	
		initCtx((void*)this, &sfe_dev); 			

		// I2C ready, now check that we're using the correct sensor before moving on. 
		if (getUniqueId() != LSM6DSV16X_ID)
			return false; 


    return true;
}


/// @brief Checks that the bus is connected with the LSM6DSV16X by checking
/// it's unique ID. 
/// @return True on successful execution.
bool QwDevLSM6DSV16X::isConnected()
{
		if (getUniqueId() != LSM6DSV16X_ID)
			return false; 
		else
			return true;
}


/// @brief Sets the pointer to the data bus for read and writes.
/// @param theBus
/// @param i2cAddress
void QwDevLSM6DSV16X::setCommunicationBus(sfe_LSM6DSV16X::QwIDeviceBus &theBus, uint8_t i2cAddress)
{
    _sfeBus = &theBus;
		_i2cAddress = i2cAddress; 
}

/// @brief Sets the pointer to the data bus for read and writes.
/// @param theBus
void QwDevLSM6DSV16X::setCommunicationBus(sfe_LSM6DSV16X::QwIDeviceBus &theBus)
{
    _sfeBus = &theBus;
}

/// @brief Writes to the data to the given register using the set data bas. 
/// @param  offset
/// @param  data 
/// @param  length
/// @return The successful (0) or unsuccessful (-1) write to the given register.
int32_t QwDevLSM6DSV16X::writeRegisterRegion(uint8_t offset, uint8_t *data, uint16_t length)
{
    return _sfeBus->writeRegisterRegion(_i2cAddress, offset, data, length);
}


/// @brief Reads data from the specified register using the set data bas. 
/// @param  offset
/// @param  data 
/// @param  length
/// @return The successful (0) or unsuccessful (-1) read of the given register.
int32_t QwDevLSM6DSV16X::readRegisterRegion(uint8_t offset, uint8_t *data, uint16_t length)
{
    return _sfeBus->readRegisterRegion(_i2cAddress, offset, data, length);
}

/// @brief Sets the scale of the acceleromter's readings 2g - 16g.
/// @param val
/// @return True upon successful write or false otherwise.
bool QwDevLSM6DSV16X::setAccelFullScale(uint8_t val)
{
	//0 = 2g, 1 = 4g, 2 = 8g, 3 = 16g
	if( val > 3 )
		return false;
	
	int32_t retVal = (lsm6dsv16x_xl_full_scale_set(&sfe_dev, 
																								(lsm6dsv16x_xl_full_scale_t) val));
	
	fullScaleAccel = val; 

	if( retVal != 0 )
		return false; 

	return true; 
}

/// @brief Sets the scale of the gyroscopes's readings 125, 250, 500, 1000, 2000, or 4000 degrees per second
/// @param val
/// @return True upon successful write or false otherwise.
bool QwDevLSM6DSV16X::setGyroFullScale(uint8_t val)
{
	//0,1,2,4,8,12
	if( val > 12 )
		return false;
	
	int32_t retVal = lsm6dsv16x_gy_full_scale_set(&sfe_dev,
																							  (lsm6dsv16x_gy_full_scale_t)val);
	
	fullScaleGyro = val; 

	if( retVal != 0 )
		return false; 

	return true; 
}

/// @brief Retrieves the scale of the accelerometer's readings 
/// @return Returns the gyroscope's scale setting. 
uint8_t QwDevLSM6DSV16X::getAccelFullScale()
{

	lsm6dsv16x_xl_full_scale_t val; 
	int32_t retVal = lsm6dsv16x_xl_full_scale_get(&sfe_dev, &val);

	if( retVal != 0 )
		return -1; 
	
	return (uint8_t)val; 
}

/// @brief Retrieves the the device's ID: 0x6B for the LSM6DSV16X
/// @return Returns the unique ID. 
uint8_t QwDevLSM6DSV16X::getUniqueId()
{

	uint8_t buff = 0;
	int32_t retVal = (lsm6dsv16x_device_id_get(&sfe_dev, &buff));

	if(retVal != 0)
		return 0; 
	
	return buff;
}

/// @brief Retrieves raw temperature values.
/// @return Returns raw temperature value. 
int16_t QwDevLSM6DSV16X::getTemp()
{
	
	int16_t tempVal;	
	int32_t retVal = lsm6dsv16x_temperature_raw_get(&sfe_dev, &tempVal);

	if( retVal != 0 )
		return -1;

	return tempVal;

}


/// @brief Retrieves raw register values for accelerometer data 
/// @param accelData    
/// @return True on successful execution.
bool QwDevLSM6DSV16X::getRawAccel(sfe_lsm_raw_data_t* accelData)
{
	int16_t tempVal[3] = {0};	
	int32_t retVal = lsm6dsv16x_acceleration_raw_get(&sfe_dev, tempVal);

	if( retVal != 0 )
		return false;

	accelData->xData = tempVal[0];
	accelData->yData = tempVal[1];
	accelData->zData = tempVal[2];

	return true;

}

/// @brief Retrieves raw register values for gyroscope data 
/// @param gyroData    
/// @return True on successful execution.
bool QwDevLSM6DSV16X::getRawGyro(sfe_lsm_raw_data_t* gyroData)
{
	
	int16_t tempVal[3] = {0};	
	int32_t retVal = lsm6dsv16x_angular_rate_raw_get(&sfe_dev, tempVal);

	if( retVal != 0 )
		return false;


	gyroData->xData = tempVal[0];
	gyroData->yData = tempVal[1];
	gyroData->zData = tempVal[2];

	return true;

}

/// @brief Takes raw register values and converts them according to the 
/// accelerometer's scale setting.
/// @param accelData   
/// @return True on successful execution.
bool QwDevLSM6DSV16X::getAccel(sfe_lsm_data_t* accelData)
{
	
	int16_t tempVal[3] = {0};	
	uint8_t tempScale = 0;
	int32_t retVal; 

	retVal = lsm6dsv16x_acceleration_raw_get(&sfe_dev, tempVal);

	if( fullScaleAccel == -1 )
	{
		tempScale = getAccelFullScale();		
		fullScaleAccel = (int8_t)tempScale;
	}

	if( retVal != 0 )
		return false;
	
	// "fullScaleAccel" is a private variable that keeps track of the users settings
	// so that the register values can be converted accordingly
	switch( fullScaleAccel ){
		case 0:
			accelData->xData = convert2gToMg(tempVal[0]);
			accelData->yData = convert2gToMg(tempVal[1]);
			accelData->zData = convert2gToMg(tempVal[2]);
			break;
		case 1:
			accelData->xData = convert16gToMg(tempVal[0]);
			accelData->yData = convert16gToMg(tempVal[1]);
			accelData->zData = convert16gToMg(tempVal[2]);
			break;
		case 2:
			accelData->xData = convert4gToMg(tempVal[0]);
			accelData->yData = convert4gToMg(tempVal[1]);
			accelData->zData = convert4gToMg(tempVal[2]);
			break;
		case 3:
			accelData->xData = convert8gToMg(tempVal[0]);
			accelData->yData = convert8gToMg(tempVal[1]);
			accelData->zData = convert8gToMg(tempVal[2]);
			break;
		default:
			return false; //Something has gone wrong
	}

	return true;
}

/// @brief Takes raw register values and converts them according to the 
/// gyroscope's scale setting.
/// @param gyroData   
/// @return True on successful execution.
bool QwDevLSM6DSV16X::getGyro(sfe_lsm_data_t* gyroData)
{
	
	int16_t tempVal[3] = {0};	
	int32_t retVal = lsm6dsv16x_angular_rate_raw_get(&sfe_dev, tempVal);

	if( retVal != 0 )
		return false;

	// "fullScaleGyro" is a private variable that keeps track of the users settings
	// so that the register values can be converted accordingly
	switch( fullScaleGyro ){
		case 0:
			gyroData->xData = convert250dpsToMdps(tempVal[0]);
			gyroData->yData = convert250dpsToMdps(tempVal[1]);
			gyroData->zData = convert250dpsToMdps(tempVal[2]);
			break;
		case 1:
			gyroData->xData = convert4000dpsToMdps(tempVal[0]);
			gyroData->yData = convert4000dpsToMdps(tempVal[1]);
			gyroData->zData = convert4000dpsToMdps(tempVal[2]);
			break;
		case 2:
			gyroData->xData = convert125dpsToMdps(tempVal[0]);
			gyroData->yData = convert125dpsToMdps(tempVal[1]);
			gyroData->zData = convert125dpsToMdps(tempVal[2]);
			break;
		case 4:
			gyroData->xData = convert500dpsToMdps(tempVal[0]);
			gyroData->yData = convert500dpsToMdps(tempVal[1]);
			gyroData->zData = convert500dpsToMdps(tempVal[2]);
			break;
		case 8:
			gyroData->xData = convert1000dpsToMdps(tempVal[0]);
			gyroData->yData = convert1000dpsToMdps(tempVal[1]);
			gyroData->zData = convert1000dpsToMdps(tempVal[2]);
			break;
		case 12:
			gyroData->xData = convert2000dpsToMdps(tempVal[0]);
			gyroData->yData = convert2000dpsToMdps(tempVal[1]);
			gyroData->zData = convert2000dpsToMdps(tempVal[2]);
			break;
		default:
			return false; //Something has gone wrong
	}

	return true;
}


/// @brief Converts 2g scale to milli-g's
/// @param data
/// @return Data converted to milli-g's
float QwDevLSM6DSV16X::convert2gToMg(int16_t data)
{
	return(lsm6dsv16x_from_fs2_to_mg(data));
}

/// @brief Converts 4g scale to milli-g's
/// @param data
/// @return Data converted to milli-g's
float QwDevLSM6DSV16X::convert4gToMg(int16_t data)
{
	return(lsm6dsv16x_from_fs4_to_mg(data));
}

/// @brief Converts 8g scale to milli-g's
/// @param data
/// @return Data converted to milli-g's
float QwDevLSM6DSV16X::convert8gToMg(int16_t data)
{
	return(lsm6dsv16x_from_fs8_to_mg(data));
}

/// @brief Converts 16g scale to milli-g's
/// @param data
/// @return Data converted to milli-g's
float QwDevLSM6DSV16X::convert16gToMg(int16_t data)
{
	return(lsm6dsv16x_from_fs16_to_mg(data));
}

/// @brief Converts 125 degrees per second scale to milli-degrees-per-second
/// @param data
/// @return Data converted to milli-degrees-per-second
float QwDevLSM6DSV16X::convert125dpsToMdps(int16_t data)
{
	return(lsm6dsv16x_from_fs125_to_mdps(data));
}

/// @brief Converts 250 degrees per second scale to milli-degrees-per-second
/// @param data
/// @return Data converted to milli-degrees-per-second
float QwDevLSM6DSV16X::convert250dpsToMdps(int16_t data)
{
	return(lsm6dsv16x_from_fs250_to_mdps(data));
}

/// @brief Converts 500 degrees per second scale to milli-degrees-per-second
/// @param data
/// @return Data converted to milli-degrees-per-second
float QwDevLSM6DSV16X::convert500dpsToMdps(int16_t data)
{
	return(lsm6dsv16x_from_fs500_to_mdps(data));
}

/// @brief Converts 1000 degrees per second scale to milli-degrees-per-second
/// @param data
/// @return Data converted to milli-degrees-per-second
float QwDevLSM6DSV16X::convert1000dpsToMdps(int16_t data)
{
	return(lsm6dsv16x_from_fs1000_to_mdps(data));
}

/// @brief Converts 2000 degrees per second scale to milli-degrees-per-second
/// @param data
/// @return Data converted to milli-degrees-per-second
float QwDevLSM6DSV16X::convert2000dpsToMdps(int16_t data)
{
	return(lsm6dsv16x_from_fs2000_to_mdps(data));
}

/// @brief Converts 4000 degrees per second scale to milli-degrees-per-second
/// @param data
/// @return Data converted to milli-degrees-per-second
float QwDevLSM6DSV16X::convert4000dpsToMdps(int16_t data)
{
	return(lsm6dsv16x_from_fs4000_to_mdps(data));
}

/// @brief Converts temperature to celsius. 
/// @param data
/// @return Data converted to milli-degrees-per-second
float QwDevLSM6DSV16X::convertToCelsius(int16_t data)
{
	return(lsm6dsv16x_from_lsb_to_celsius(data));
}

//
//
//////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////
// General Settings
//
// 
//
//


//////////////////////////////////////////////////////////////////////////////////
// setDeviceConfig()
//
// 
//  Parameter   Description
//  ---------   -----------------------------
//  enable      Enable the general device configuration - this is a configuration 
//							register in the LSM6DSV16X, not a library implementation. 
// NOT IN LIB
//bool QwDevLSM6DSV16X::setDeviceConfig(bool enable)
//{
//	int32_t retVal;
//
//	retVal = lsm6dsv16x_device_conf_set(&sfe_dev, (uint8_t)enable);
//
//	if( retVal != 0 )
//		return false;
//
//	return true;
//}

/// @brief Resets all registers. 
/// @return true on successful execution.
bool QwDevLSM6DSV16X::deviceReset()
{
	int32_t retVal;

	retVal = lsm6dsv16x_reset_set(&sfe_dev,(lsm6dsv16x_reset_t)1);

	if( retVal != 0 )
		return false;

	return true;
}

/// @brief Checks the bit indicating that the device has finished reseting.
/// @return true on successful execution.
bool QwDevLSM6DSV16X::getDeviceReset()
{

	int32_t retVal;
	uint8_t tempVal;
	retVal = lsm6dsv16x_reset_get(&sfe_dev, (lsm6dsv16x_reset_t)&tempVal);

	if( retVal != 0 )
		return false;

	if( (tempVal) == 0x00 )
		return true; 

	return false; 
	
}

/// @brief Sets the accelerometer's slope filter
/// @param enable
/// @return True on successful operation. 
bool QwDevLSM6DSV16X::enableAccelHpFilter(bool enable)
{
	int32_t retVal;

	retVal = lsm6dsv16x_filt_xl_hp_set(&sfe_dev, (uint8_t)enable);

	if( retVal != 0 )
		return false;

	return true;
}

/// @brief Selects the accelerometer's high resolution between first stage
/// (zero) digital filtering or (one) LPF2 second filtering stage
/// @param second
/// @return True on successful operation
bool QwDevLSM6DSV16X::enableAccelLPS2(bool second)
{
	int32_t retVal;

	retVal = lsm6dsv16x_filt_xl_lp2_set(&sfe_dev, uint8_t(second));

	if( retVal != 0 )
		return false;

	return true;
}

/// @brief Enables fast settling mode for the acclerometer. Set only during 
/// device exit from power down mode.
/// @param enable
/// @return True on successful operation
bool QwDevLSM6DSV16X::enableFastSetMode(bool enable)
{
	int32_t retVal;

	retVal = lsm6dsv16x_filt_xl_fast_settling_set(&sfe_dev, (uint8_t)enable);

	if ( retVal != 0 )
		return false;

	return true;
}

/// @brief Data is not updated until both MSB and LSB have been read from output registers
/// @param enable			
/// @return True on successful operation
bool QwDevLSM6DSV16X::setBlockDataUpdate(bool enable)
{

	int32_t retVal;

	retVal = lsm6dsv16x_block_data_update_set(&sfe_dev, (uint8_t)enable);

	if( retVal != 0 )
		return false;

	return true; 
}


/// @brief Retrieves the bit indicating whether the block data update mode is set. 
/// @return True on successful operation
uint8_t QwDevLSM6DSV16X::getBlockDataUpdate()
{

	uint8_t tempVal;
	int32_t retVal; 

	retVal = lsm6dsv16x_block_data_update_get(&sfe_dev, &tempVal);

	return tempVal; 
}


/// @brief Sets the acceleromter's output data rate (ODR)
/// @param rate 
/// @return True on successful operation
bool QwDevLSM6DSV16X::setAccelDataRate(uint8_t rate)
{
	if( rate > 12 )
		return false; 

	int32_t retVal; 

	retVal = lsm6dsv16x_xl_data_rate_set(&sfe_dev, (lsm6dsv16x_odr_xl_t)rate);

	if( retVal != 0)
		return false;

	return true; 
}

/// @brief Sets the gyro's output data rate (ODR)
/// @param rate 
/// @return True on successful operation
bool QwDevLSM6DSV16X::setGyroDataRate(uint8_t rate)
{
	if( rate > 12 )
		return false;

	int32_t retVal;

	retVal = lsm6dsv16x_gy_data_rate_set(&sfe_dev,(lsm6dsv16x_odr_g_t)rate);

	if( retVal != 0 )
		return false;

	return true; 
}



/// @brief Enables the timestamp counter
/// @param enable 
/// @return True on successful operation
bool QwDevLSM6DSV16X::enableTimestamp(bool enable)
{
	int32_t retVal;

	retVal = lsm6dsv16x_timestamp_set(&sfe_dev, (uint8_t)enable);

	if( retVal != 0 )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// FIFO Settings
//
// 
//
//
//  
// 

// @brief Sets the FIFO's watermark threshold.
// @param val
// @return True on successful operation
bool QwDevLSM6DSV16X::setFifoWatermark(uint16_t val)
{
	int32_t retVal;

	if( val > 255 ) 
		return false; 

	retVal = lsm6dsv16x_fifo_watermark_set(&sfe_dev, val);

	if( retVal != 0 )
		return false;

	return true;
}

// @brief Fifo mode selection
// @param val
// @return True on successful operation
bool QwDevLSM6DSV16X::setFifoMode(uint8_t val)
{
	int32_t retVal;

	if( val > 7 )
		return false;

	retVal = lsm6dsv16x_fifo_mode_set(&sfe_dev,
                                 (lsm6dsv16x_fifo_mode_t)val);

	if( retVal != 0 )
		return false;

	return true;
}

// @brief FIFO Batch data rate selection for the accelerometer
// @param val
// @return True on successful executuion
bool QwDevLSM6DSV16X::setAccelFifoBatchSet(uint8_t val)
{
	int32_t retVal;

	if( val > 11)
		return false;

	retVal = lsm6dsv16x_fifo_xl_batch_set(&sfe_dev,
                                     (lsm6dsv16x_fifo_xl_batch_t)val);

	if( retVal != 0 )
		return false;

	return true;
}


// @brief FIFO Batch data rate selection for the gyroscope
// @param val
// @return True on successful executuion
bool QwDevLSM6DSV16X::setGyroFifoBatchSet(uint8_t val)
{
	int32_t retVal;
	if( val > 11)
		return false;

	retVal = lsm6dsv16x_fifo_gy_batch_set(&sfe_dev,
                                     (lsm6dsv16x_fifo_gy_batch_t)val);

	if( retVal != 0 )
		return false;

	return true;
}

// @brief Selects decimation for timestamp batching in FIFO
// @param val
// @return True on successful executuion
bool QwDevLSM6DSV16X::setFifoTimestampDec(uint8_t val)
{
	int32_t retVal;

	if( val > 3 )
		return false;

	retVal = lsm6dsv16x_fifo_timestamp_decimation_batch_set(&sfe_dev,
                                                   (lsm6dsv16x_fifo_timestamp_batch_t)val);

	if( retVal != 0 )
		return false;

	return true;
}

//
//
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// Interrupt Settings
//
// 
//
//
//  
// 

/// @brief Sets the active state of the interrupt pin - high or low.  
/// @param openDrain 
/// @return True on successful execution
bool QwDevLSM6DSV16X::setInt2DENPolarity(bool activeLow )
{
	int32_t retVal;

	//0 = active high :  active low, 1  
	retVal = lsm6dsv16x_den_polarity_set(&sfe_dev, (lsm6dsv16x_den_polarity_t)activeLow);

	if( retVal != 0 )
		return false;
	
	if( activeLow )
		// pinmode must be set to push-pull when active low is set.
		// See section 9.14 on pg 51 of datasheet for more information
		retVal = lsm6dsv16x_pin_mode_set(&sfe_dev, (lsm6dsv16x_pp_od_t)0);

	if( retVal != 0 )
		return false;

	return true; 
}

/// @brief Selects the signal that will be routed to int 1
/// @param val
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::setInt1Route(uint8_t val)
{
	int32_t retVal;

	retVal = lsm6dsv16x_pin_int1_route_set(&sfe_dev,
                                        (lsm6dsv16x_int1_ctrl_t)val);

	if( retVal != 0 )
		return false;

	return true;
}

/// @brief Routes the data ready signal for the accelerometer to interrupt one.
/// @param enable
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::setInt1AccelDataReady(bool enable)
{

	int32_t retVal;
	lsm6dsv16x_pin_int_route_t int1_route;

	int1_ctrl.int1_drdy_xl = (uint8_t)enable; 
	
	retVal = lsm6dsv16x_pin_int1_route_set(&sfe_dev, &int1_ctrl);

	if( retVal != 0 )
		return false;

	return true; 
}

/// @brief Routes the data ready signal for the accelerometer to interrupt two.
/// @param enable
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::setInt2AccelDataReady(bool enable)
{

	int32_t retVal;

	lsm6dsv16x_pin_int_route_t int2_route; 

	int2_route.int2_drdy_xl	= (uint8_t)enable;

	retVal = lsm6dsv16x_pin_int2_route_set(&sfe_dev, &int2_route);

	if( retVal != 0 )
		return false;

	return true; 
}

/// @brief Routes the data ready signal for the gyroscope to interrupt one. 
/// @param enable
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::setInt1GyroDataReady(bool enable)
{

	int32_t retVal;

	lsm6dsv16x_pin_int_route_t int1_route; 

	int1_route.int1_drdy_g = (uint8_t)enable;
	
	retVal = lsm6dsv16x_pin_int1_route_set(&sfe_dev, &int1_route);

	if( retVal != 0 )
		return false;

	return true; 
}


/// @brief Routes the data ready signal for the gyroscope to interrupt two. 
/// @param enable
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::setint2GyroDataReady(bool enable)
{

int32_t retVal;

lsm6dsv16x_pin_int_route_t int2_route; 

int2_route.int2_drdy_g = (uint8_t)enable;

retVal = lsm6dsv16x_pin_int2_route_set(&sfe_dev, &int2_route);

if( retVal != 0 )
	return false;

return true; 
}

/// @brief Enables pulsed data ready mode as oppsed to latch - 65us pulse.
/// @param enable
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::setDataReadyMode(bool enable)
{
	int32_t retVal;

	lsm6dsv16x_ctrl4_t ctrl4;
	 
	ctrl4.drdy_pulsed = (uint8_t)enable;

	retVal = lsm6dsv16x_data_ready_mode_set(&sfe_dev, ctrl4);

	if( retVal != 0 )
		return false;

	return true;
}
//
//
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// Sensor Hub Settings
//
// 
//
//
//  
// 
// 

/// @brief Sets the Sensor Hub Output Data Rate
/// @param rate
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::setHubODR(uint8_t rate)
{
	//15Hz - 480Hz
	if( rate > 6 )
		return false;

	int32_t retVal = lsm6dsv16x_sh_data_rate_set(&sfe_dev, (lsm6dsv16x_sh_data_rate_t)rate);

	if( retVal != 0 )
		return false;

	return true; 
}

/// @brief Sets the parameters with which to read from a downstream sensor 
/// @param settings
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::setHubSensorRead(uint8_t sensor, sfe_hub_sensor_settings_t* settings)
{
	int32_t retVal;
	lsm6dsv16x_sh_cfg_read_t tempSett;

	if( sensor > 3 )
		return false;

	tempSett.slv_add = settings->address; 
	tempSett.slv_subadd = settings->subAddress; 
	tempSett.slv_len = settings->lenData; 


	switch( sensor )
	{
		case 0:
			retVal = lsm6dsv16x_sh_slv0_cfg_read(&sfe_dev, &tempSett);
			break;
		case 1:
			retVal = lsm6dsv16x_sh_slv1_cfg_read(&sfe_dev, &tempSett);
			break;
		case 2:
			retVal = lsm6dsv16x_sh_slv2_cfg_read(&sfe_dev, &tempSett);
			break;
		case 3:
			retVal = lsm6dsv16x_sh_slv3_cfg_read(&sfe_dev, &tempSett);
			break;
		default:
			return false;
	}


	if( retVal != 0 )
		return false;

	return true; 
}

/// @brief Sets the parameters with which to write to the downstream sensor.
/// @param settings
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::setHubSensorWrite(sfe_hub_sensor_settings_t* settings)
{
	int32_t retVal;
	lsm6dsv16x_sh_cfg_write_t tempSett;

	tempSett.slv0_add = settings->address; 
	tempSett.slv0_subadd = settings->subAddress; 
	tempSett.slv0_data = settings->lenData; 

	retVal = lsm6dsv16x_sh_cfg_write(&sfe_dev, &tempSett);

	if( retVal != 0 )
		return false;

	return true; 
}

/// @brief Sets the number of sensors connected to the sensor hub.
/// @param numSensor
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::setNumberHubSensors(uint8_t numSensors)
{

	int32_t retVal;
	if( numSensors > 3 ) 
		return false; 

	retVal = lsm6dsv16x_sh_slave_connected_set(&sfe_dev, 
																					 (lsm6dsv16x_sh_slave_connected_t)numSensors);

	if( retVal != 0 )
		return false;

	return true; 

}

/// @brief Enables sensor hub I2C
/// @param enable
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::enableSensorI2C(bool enable)
{
	int32_t retVal;

	retVal = lsm6dsv16x_sh_master_set(&sfe_dev, (uint8_t)enable);

	if( retVal != 0 )
		return false;

	return true;
}

/// @brief Reads froms sensor hub output registers
/// @param shReg
/// @param len
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::readPeripheralSensor(uint8_t* shReg, uint8_t len)
{
	int32_t retVal;

	retVal = lsm6dsv16x_sh_read_data_raw_get(&sfe_dev, (lsm6dsv16x_emb_sh_read_t*)shReg, len);

	if( retVal != 0 )
		return false;

	return true;
}


/// @brief Checks the bit indicating a NACK for the given sensor
/// @param sensor
/// @return Returns true if a NACK on the sensor hub has occurred.
bool QwDevLSM6DSV16X::getExternalSensorNack(uint8_t sensor)
{

	int32_t retVal;
	lsm6dsv16x_all_sources_t tempVal;

	retVal = lsm6dsv16x_all_sources_get(&sfe_dev, &tempVal);

	if( retVal != 0 )
		return false;

	switch( sensor ){
		case 0:
			if( tempVal.sh_slave0_nack == 1 )
				return true;
			break;
		case 1:
			if( tempVal.sh_slave1_nack == 1 )
				return true;
			break;
		case 2:
			if( tempVal.sh_slave2_nack == 1 )
				return true;
			break;
		case 3:
			if( tempVal.sh_slave3_nack == 1 )
				return true;
			break;
		default:
			return false;
	}

	return false; 
}

/// @brief Configures the sensor hub to write only at the FIRST sensor hub cycle. 
/// @param enable
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::enableHubWriteOnceMode(bool enable)
{
	int32_t retVal;

	// 0 = Write each cycle 
	// 1 = Write once
	retVal = lsm6dsv16x_sh_write_mode_set(&sfe_dev,
																			 (lsm6dsv16x_sh_write_mode_t)enable);

	if( retVal != 0 )
		return false;

	return true;
}

/// @brief Enables the main I2C data lines to communicated directly with a sensor
/// connected to the sensor hub I2C lines. 
/// @param enable
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::enableHubPassThrough(bool enable)
{

	int32_t retVal = lsm6dsv16x_sh_pass_through_set(&sfe_dev, (uint8_t)enable);

	if( retVal != 0 )
		return false;

	return true; 
}

/// @brief Enables data from downstream sensors to be stored in the FIFO.
/// @param eanble
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::enableHubFifoBatching(bool enable)
{
	int32_t retVal;

	retVal = lsm6dsv16x_sh_batch_slave_0_set(&sfe_dev, (uint8_t)enable);

	if( retVal != 0 )
		return false;

	return true;
}

/// @brief Enables pull up resistors on the sensor hub I2C lines.
/// @param
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::enableHubPullUps(bool enable)
{

	int32_t retVal;

	retVal = lsm6dsv16x_sh_master_interface_pull_up_set(&sfe_dev, (uint8_t)&enable);

	if( retVal != 0 )
		return false;

	return true; 
}

/// @brief Resets sensor hub registers. 
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::resetSensorHub()
{

	int32_t retVal;  

	// Must be set to one, then zero
	retVal = lsm6dsv16x_sh_reset_set(&sfe_dev, 1);

	delay(1);

	retVal = lsm6dsv16x_sh_reset_set(&sfe_dev, 0);

	if( retVal != 0 )
		return false;

	return true; 
}
//
//
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// Self Test
//
// 
//
//
//  
// 
// 

/// @brief Accelerometer self-test selection 
/// @param val
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::setAccelSelfTest(uint8_t val)
{
	int32_t retVal;

	if( val > 2)
		return false;

	retVal = lsm6dsv16x_xl_self_test_set(&sfe_dev,
                                    (lsm6dsv16x_xl_self_test_t)val);

	if( retVal != 0 )
		return false;

	return true;
}

/// @brief Gyroscope self-test selection 
/// @param val
/// @return Returns true on successful execution
bool QwDevLSM6DSV16X::setGyroSelfTest(uint8_t val)
{
	int32_t retVal;

	// 0, 1, or 3
	if( val > 3)
		return false;

	retVal = lsm6dsv16x_gy_self_test_set(&sfe_dev,
                                    (lsm6dsv16x_gy_self_test_t)val);

	if( retVal != 0 )
		return false;

	return true;
}
//
//
//////////////////////////////////////////////////////////////////////////////////

/// @brief Retrieves the data ready bits for both the accelerometer and gyroscope.
/// @return Returns true on if both bits indicate data. 
bool QwDevLSM6DSV16X::checkStatus()
{
	lsm6dsv16x_data_ready_t tempVal;
	int32_t retVal = lsm6dsv16x_flag_data_ready_get(&sfe_dev, &tempVal);

	if( retVal != 0)
		return false;

	if( (tempVal.drdy_xl == 1) && (tempVal.drdy_gl == 1) )
		return true; 

	return false; 

}

/// @brief Retrieves the data ready bit for the accelerometer
/// @return Returns true if the bit indicates there is data.
bool QwDevLSM6DSV16X::checkAccelStatus()
{
	lsm6dsv16x_data_ready_t tempVal;

	int32_t retVal = lsm6dsv16x_flag_data_ready_get(&sfe_dev, &tempVal);

	if( retVal != 0)
		return false;

	if( tempVal.drdy_xl == 1 )
		return true; 

	return false; 

}

/// @brief Retrieves the data ready bit for the gyroscope
/// @return Returns true if the bit indicates there is data.
bool QwDevLSM6DSV16X::checkGyroStatus()
{
	lsm6dsv16x_data_ready_t tempVal;

	int32_t retVal = lsm6dsv16x_flag_data_ready_get(&sfe_dev, &tempVal);

	if( retVal != 0)
		return false;

	if( tempVal.gda == 1 )
		return true; 

	return false; 

}


/// @brief Retrieves the data ready bit for the temperature
/// @return Returns true if the bit indicates there is data.
bool QwDevLSM6DSV16X::checkTempStatus()
{
	lsm6dsv16x_data_ready_t tempVal;

	int32_t retVal = lsm6dsv16x_flag_data_ready_get(&sfe_dev, &tempVal);

	if( retVal != 0)
		return false;

	if( tempVal.tda == 1 )
		return true; 

	return false; 

}

