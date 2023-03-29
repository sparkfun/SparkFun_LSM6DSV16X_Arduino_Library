#include "sfe_bus.h"
#include "sfe_lsm_shim.h"


/*
 * Link to example code:
 * https://github.com/STMicroelectronics/STMems_Standard_C_drivers/tree/master/lsm6dsv16x_STdC/examples
 */

#define LSM6DSV16X_ADDRESS_LOW 0x6A //Default
#define LSM6DSV16X_ADDRESS_HIGH 0x6B

struct sfe_lsm_raw_data_t
{
	int16_t xData;	
	int16_t yData;	
	int16_t zData;
};

struct sfe_lsm_data_t
{
	float xData;	
	float yData;	
	float zData;
};


struct sfe_hub_sensor_settings_t
{
	uint8_t address;
	uint8_t subAddress;
	uint8_t lenData;
};


class QwDevLSM6DSV16X
{
	public: 

		QwDevLSM6DSV16X() : _i2cAddress{0}, _cs{0} {};
			
    bool init();


    bool isConnected(); // Checks if sensor ack's the I2C request

    int32_t writeRegisterRegion(uint8_t reg, uint8_t *data, uint16_t length = 1);
    int32_t readRegisterRegion(uint8_t reg, uint8_t *data, uint16_t length = 1);

		void setCommunicationBus(sfe_LSM6DSV16X::QwIDeviceBus &theBus, uint8_t i2cAddress);
		void setCommunicationBus(sfe_LSM6DSV16X::QwIDeviceBus &theBus);

		bool setAccelFullScale(uint8_t val);
		bool setGyroFullScale(uint8_t val);
		uint8_t getAccelFullScale();

		uint8_t getUniqueId();

		// Linear, Angular, and Temp Data retrieval 
		int16_t getTemp();
		bool getRawAccel(sfe_lsm_raw_data_t* accelData);
		bool getRawGyro(sfe_lsm_raw_data_t* gyroData);
		bool getRawQvar(int16_t* qvarData);
		bool getAccel(sfe_lsm_data_t* accelData);
		bool getGyro(sfe_lsm_data_t* gyroData);

		// General Settings
		//bool setDeviceConfig(bool enable = true);
		bool deviceReset();
		bool getDeviceReset();
		bool setAccelMode(uint8_t mode); 
		bool setGyroMode(uint8_t mode); 
		bool enableAccelHpFilter(bool enable = true);
		bool enableAccelLPS2(bool enable = true);
		bool enableFastSetMode(bool enable = true);
		bool enableGyroLP1Filter(bool enable = true);
		bool setGyroLP1Bandwidth(uint8_t val);
		bool enableAccelLP2Filter(bool enable = true);
		bool setAccelLP2Bandwidth(uint8_t val);
		bool enableBlockDataUpdate(bool enable = true);
		uint8_t getBlockDataUpdate();
		bool setAccelDataRate(uint8_t rate);
		bool setGyroDataRate(uint8_t rate);
		bool enableTimestamp(bool enable = true);
		bool enableFilterSettling(bool enable = true);
		//bool resetTimestamp();

		// Interrupt Settings
		bool setInt2DENPolarity(bool activeLow = true);
		bool setInt1Route(uint8_t val);
		bool setInt1AccelDataReady(bool enable = true);
		bool setInt2AccelDataReady(bool enable = true);
		bool setInt1GyroDataReady(bool enable = true);
		bool setInt2GyroDataReady(bool enable = true);
		bool setDataReadyMode(bool enable = true);

		// Qvar Settings

		bool enableAhQvar(bool enable = true);
		uint8_t getQvarMode();
		
		// Sensor Hub Settings
		bool setHubODR(uint8_t rate);
		bool setHubSensorRead(uint8_t sensor, sfe_hub_sensor_settings_t* settings);
		bool setHubSensorWrite(sfe_hub_sensor_settings_t* settings);
		bool setNumberHubSensors(uint8_t numSensors);
		bool enableSensorI2C(bool enable);
		bool readPeripheralSensor(uint8_t* shReg, uint8_t len);
		bool getExternalSensorNack(uint8_t sensor);
		bool enableHubWriteOnceMode(bool enable = true);
		bool enableHubPassThrough(bool enable = true);
		bool enableHubPullUps(bool enable = true);
		bool resetSensorHub();

		// Self Test
		bool setAccelSelfTest(uint8_t val);
		bool setGyroSelfTest(uint8_t val);

		// FIFO Settings
		bool setFifoWatermark(uint16_t val);
		bool setFifoMode(uint8_t val);
		bool setAccelFifoBatchSet(uint8_t val);
		bool setGyroFifoBatchSet(uint8_t val);
		bool setFifoTimestampDec(uint8_t val);
			
		// Status
		bool checkStatus();
		bool checkAccelStatus();
		bool checkGyroStatus();
		bool checkTempStatus();
		bool checkQvar();

		// Conversions
		float convert2gToMg(int16_t data);
		float convert4gToMg(int16_t data);
		float convert8gToMg(int16_t data);
		float convert16gToMg(int16_t data);
		float convert125dpsToMdps(int16_t data);
		float convert250dpsToMdps(int16_t data);
		float convert500dpsToMdps(int16_t data);
		float convert1000dpsToMdps(int16_t data);
		float convert2000dpsToMdps(int16_t data);
		float convert4000dpsToMdps(int16_t data);
		float convertToCelsius(int16_t data);

	private: 
		
		sfe_LSM6DSV16X::QwIDeviceBus *_sfeBus; 
		uint8_t _i2cAddress;
		uint8_t _cs;
		stmdev_ctx_t sfe_dev; 
		int8_t fullScaleAccel = -1; //Powered down by default
		uint8_t fullScaleGyro = 0;  //Powered down by default
};

