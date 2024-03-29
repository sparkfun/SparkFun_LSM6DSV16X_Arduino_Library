#include "sfe_bus.h"
#include <Arduino.h>

#define kMaxTransferBuffer 32
#define SPI_READ 0x80

// What we use for transfer chunk size
const static uint16_t kChunkSize = kMaxTransferBuffer;

//////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor
//

namespace sfe_LSM6DSV16X {

QwI2C::QwI2C(void) : _i2cPort{nullptr}
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// I2C init()
//
// Methods to init/setup this device. The caller can provide a Wire Port, or this class
// will use the default

bool QwI2C::init(TwoWire &wirePort, bool bInit)
{

    // if we don't have a wire port already
    if( !_i2cPort )
    {
        _i2cPort = &wirePort;

        if( bInit )
            _i2cPort->begin();
    }
		
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// I2C init()
//
// Methods to init/setup this device. The caller can provide a Wire Port, or this class
// will use the default
bool QwI2C::init()
{
		if( !_i2cPort )
			return init(Wire);
		else
			return false;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
// ping()
//
// Is a device connected?
bool QwI2C::ping(uint8_t i2c_address)
{

    if( !_i2cPort )
        return false;

    _i2cPort->beginTransmission(i2c_address);
    return _i2cPort->endTransmission() == 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// writeRegisterRegion()
//
// Write a block of data to a device.

int QwI2C::writeRegisterRegion(uint8_t i2c_address, uint8_t offset, const uint8_t *data, uint16_t length)
{

    _i2cPort->beginTransmission(i2c_address);
    _i2cPort->write(offset);
    _i2cPort->write(data, (int)length);

    return _i2cPort->endTransmission() ? -1 : 0; // -1 = error, 0 = success
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// readRegisterRegion()
//
// Reads a block of data from an i2c register on the devices.
//
// For large buffers, the data is chuncked over KMaxI2CBufferLength at a time
//
//
int QwI2C::readRegisterRegion(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t numBytes)
{
    uint8_t nChunk;
    uint16_t nReturned;

    if (!_i2cPort)
        return -1;

    int i;                   // counter in loop

    while (numBytes > 0)
    {
        _i2cPort->beginTransmission(addr);
				_i2cPort->write(reg);
        if (_i2cPort->endTransmission() != 0)
            return -1; // error with the end transmission

        // We're chunking in data - keeping the max chunk to kMaxI2CBufferLength
        nChunk = numBytes > kChunkSize ? kChunkSize : numBytes;

        nReturned = _i2cPort->requestFrom((int)addr, (int)nChunk, (int)true);

        // No data returned, no dice
        if (nReturned == 0)
            return -1; // error

        // Copy the retrieved data chunk to the current index in the data segment
        for (i = 0; i < nReturned; i++){
            *data++ = _i2cPort->read();
				}

        // Decrement the amount of data recieved from the overall data request amount
        numBytes = numBytes - nReturned;

				// Move the register to the number of registers read. 
				reg += nReturned; 

    } // end while

    return 0; // Success
}



//////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor
//

SfeSPI::SfeSPI(void) : _spiPort{nullptr}
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// SPI init()
//
// Methods to init/setup this device. The caller can provide a SPI Port, or this class
// will use the default


bool SfeSPI::init(SPIClass &spiPort, SPISettings& lsmSPISettings, uint8_t cs,  bool bInit)
{

    // if we don't have a SPI port already
    if( !_spiPort )
    {
        _spiPort = &spiPort;

        if( bInit )
            _spiPort->begin();
    }


		// SPI settings are needed for every transaction
		_sfeSPISettings = lsmSPISettings; 

		// The chip select pin can vary from platform to platform and project to project
		// and so it must be given by the user. 
		if( !cs )
			return false; 
		
		_cs = cs;

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// SPI init()
//
// Methods to init/setup this device. The caller can provide a SPI Port, or this class
// will use the default
bool SfeSPI::init(uint8_t cs,  bool bInit)
{

		//If the transaction settings are not provided by the user they are built here.
		SPISettings spiSettings = SPISettings(3000000, MSBFIRST, SPI_MODE3); 

		//In addition of the port is not provided by the user, it defaults to SPI here. 
		return init(SPI, spiSettings, cs, bInit);

}


//////////////////////////////////////////////////////////////////////////////////////////////////
// ping()
//
// Is a device connected? The SPI ping is not relevant but is defined here to keep consistency with
// I2C class i.e. provided for the interface.
//


bool SfeSPI::ping(uint8_t i2c_address)
{
	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// writeRegisterRegion()
//
// Write a block of data to a device.

int SfeSPI::writeRegisterRegion(uint8_t i2c_address, uint8_t offset, const uint8_t *data, uint16_t length)
{

		int i;

		// Apply settings
    _spiPort->beginTransaction(_sfeSPISettings);
		// Signal communication start
		digitalWrite(_cs, LOW);
    _spiPort->transfer(offset);

		for(i = 0; i < length; i++)
		{
			_spiPort->transfer(*data++);
		}

		// End communication
		digitalWrite(_cs, HIGH);
    _spiPort->endTransaction();
		return 0; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// readRegisterRegion()
//
// Reads a block of data from the register on the device.
//
//
//


int SfeSPI::readRegisterRegion(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t numBytes)
{
    if (!_spiPort)
        return -1;

    int i; // counter in loop

		// Apply settings
    _spiPort->beginTransaction(_sfeSPISettings);
		// Signal communication start
		digitalWrite(_cs, LOW);
		// A leading "1" must be added to transfer with register to indicate a "read"
		reg = (reg | SPI_READ);
    _spiPort->transfer(reg);

		for(i = 0; i < numBytes; i++)
		{
			*data++ = _spiPort->transfer(0x00);
		}

		// End transaction
		digitalWrite(_cs, HIGH);
    _spiPort->endTransaction();
		return 0; 

}

}
