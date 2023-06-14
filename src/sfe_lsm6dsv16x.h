#include "sfe_bus.h"
#include "sfe_lsm_shim.h"

/*
 * Link to example code:
 * https://github.com/STMicroelectronics/STMems_Standard_C_drivers/tree/master/lsm6dsv16x_STdC/examples
 */

#define LSM6DSV16X_ADDRESS_LOW 0x6A     // Default
#define LSM6DSV16X_ADDRESS_PRIMARY 0x6A // Default
#define LSM6DSV16X_ADDRESS_HIGH 0x6B
#define LSM6DSV16X_ADDRESS_SECONDARY 0x6B

typedef enum
{
    LSM_PIN_ONE = 0x01,
    LSM_PIN_TWO
} sfe_lsm_pin_t;

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

    bool setAccelFullScale(lsm6dsv16x_xl_full_scale_t scale);
    bool getAccelFullScale(lsm6dsv16x_xl_full_scale_t *scale);
    bool setGyroFullScale(lsm6dsv16x_gy_full_scale_t scale);
    bool getGyroFullScale(lsm6dsv16x_gy_full_scale_t *scale);

    uint8_t getUniqueId();

    // Linear, Angular, and Temp Data retrieval
    bool getRawTemp(int16_t *);
    bool getRawAccel(sfe_lsm_raw_data_t *accelData);
    bool getRawGyro(sfe_lsm_raw_data_t *gyroData);
    bool getRawQvar(int16_t *qvarData);
    bool getAccel(sfe_lsm_data_t *accelData);
    bool getGyro(sfe_lsm_data_t *gyroData);

    // General Settings
    // bool setDeviceConfig(bool enable = true);
    bool deviceReset();
    bool getDeviceReset();
    bool getAutoIncrement();
    bool setAccelMode(lsm6dsv16x_xl_mode_t mode);
    bool setGyroMode(lsm6dsv16x_gy_mode_t mode);
    bool enableAccelHpFilter(bool enable = true);
    bool enableAccelLPS2(bool enable = true);
    bool enableFastSetMode(bool enable = true);
    bool enableGyroLP1Filter(bool enable = true);
    bool setGyroLP1Bandwidth(uint8_t val);
    bool enableAccelLP2Filter(bool enable = true);
    bool setAccelLP2Bandwidth(uint8_t val);
    bool enableBlockDataUpdate(bool enable = true);
    uint8_t getBlockDataUpdate();
    bool setAccelDataRate(lsm6dsv16x_data_rate_t rate);
    bool setGyroDataRate(lsm6dsv16x_data_rate_t rate);
    bool enableTimestamp(bool enable = true);
    bool enableFilterSettling(bool enable = true);
    // bool resetTimestamp();

    // Interrupt Settings
    bool getAllInterrupts(lsm6dsv16x_all_sources_t *source);
    bool setInt2DENActiveLow(bool activeLow = true);
    bool setIntRoute(lsm6dsv16x_pin_int_route_t val, sfe_lsm_pin_t pin);
    bool setIntAccelDataReady(sfe_lsm_pin_t pin, bool enable = true);
    bool setIntGyroDataReady(sfe_lsm_pin_t pin, bool enable = true);
    bool setIntSingleTap(sfe_lsm_pin_t pin, bool enable = true);
    bool setIntDoubleTap(sfe_lsm_pin_t pin, bool enable = true);
    bool setIntWakeup(sfe_lsm_pin_t pin, bool enable = true);
    bool setIntFreeFall(sfe_lsm_pin_t pin, bool enable = true);
    bool setIntSleepChange(sfe_lsm_pin_t pin, bool enable = true);

    bool setDataReadyMode(lsm6dsv16x_data_ready_mode_t);

    // Tap Settings
    bool enableTapInterrupt(bool enable = true);
    bool setTapMode(lsm6dsv16x_tap_mode_t mode);
    bool getTapMode(lsm6dsv16x_tap_mode_t *mode);
    bool setTapDirection(lsm6dsv16x_tap_detection_t directionDetect);
    bool getTapDirection(lsm6dsv16x_tap_detection_t *directionDetect);
    bool setTapThresholds(lsm6dsv16x_tap_thresholds_t thresholds);
    bool getTapThresholds(lsm6dsv16x_tap_thresholds_t *thresholds);
    bool setTapTimeWindows(lsm6dsv16x_tap_time_windows_t window);
    bool getTapTimeWindows(lsm6dsv16x_tap_time_windows_t *window);

    // Qvar Settings
    bool enableAhQvar(bool enable = true);
    bool getQvarMode(lsm6dsv16x_ah_qvar_mode_t *mode);
    bool setQvarImpedance(lsm6dsv16x_ah_qvar_zin_t val);

    // Sensor Hub Settings
    bool setHubODR(lsm6dsv16x_sh_data_rate_t rate);
    bool setHubSensorRead(uint8_t sensor, lsm6dsv16x_sh_cfg_read_t *settings);
    bool setHubSensorWrite(lsm6dsv16x_sh_cfg_write_t *settings);
    bool setNumberHubSensors(lsm6dsv16x_sh_slave_connected_t numSensors);
    bool enableAuxiliaryI2C(bool enable);
    bool readPeripheralSensor(lsm6dsv16x_emb_sh_read_t *shReg, uint8_t len);
    bool getExternalSensorNack(uint8_t sensor);
    bool enableHubWriteOnceMode(bool enable = true);
    bool enableHubPassThrough(bool enable = true);
    bool enableHubPullUps(bool enable = true);
    bool resetSensorHub();

    // Self Test
    bool setAccelSelfTest(lsm6dsv16x_xl_self_test_t val);
    bool setGyroSelfTest(lsm6dsv16x_gy_self_test_t val);

    // FIFO Settings
    bool setFifoWatermark(uint8_t val);
    bool setFifoMode(lsm6dsv16x_fifo_mode_t mode);
    bool setAccelFifoBatchSet(lsm6dsv16x_fifo_xl_batch_t odr);
    bool setGyroFifoBatchSet(lsm6dsv16x_fifo_gy_batch_t odr);
    bool setFifoTimestampDec(lsm6dsv16x_fifo_timestamp_batch_t decimation);

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
    bool accelScaleSet = false;
    bool gyroScaleSet = false;
    lsm6dsv16x_xl_full_scale_t fullScaleAccel; // Powered down by default
    lsm6dsv16x_gy_full_scale_t fullScaleGyro;  // Powered down by default
};
