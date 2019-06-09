#ifndef _DRV_BME280_H
#define _DRV_BME280_H

#include <stdint.h>
#include <stdbool.h>


/**
 * BME280 or BME280 address is 0x77 if SDO pin is high, and is 0x76 if
 * SDO pin is low.
 */
#define BME280_I2C_ADDRESS_0  0x76
#define BME280_I2C_ADDRESS_1  0x77

#define MODE_SLEEP 0b00
#define MODE_FORCED 0b01
#define MODE_NORMAL 0b11

#define BME280_CHIP_ID  0x60 /* BME280 has chip-id 0x60 */

typedef struct {
	
	//Deprecated settings
	uint8_t runMode;
	uint8_t tStandby;
	uint8_t filter;
	uint8_t tempOverSample;
	uint8_t pressOverSample;
	uint8_t humidOverSample;
    float tempCorrection; // correction of temperature - added to the result
} bme280_param_t;

//Used to hold the calibration constants.  These are used
//by the driver as measurements are being taking
typedef struct {
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;
	
	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;
	
	uint8_t dig_H1;
	int16_t dig_H2;
	uint8_t dig_H3;
	int16_t dig_H4;
	int16_t dig_H5;
	int8_t dig_H6;
	
    uint8_t  id;        /* Chip ID */
} bme280_calib_t;

void bme280_init_params(void);
void read_calib_data(void);
void bme280_setMode(uint8_t mode);
uint8_t bme280_init(void);
uint8_t bme280_getMode(void);
void bme280_setStandbyTime(uint8_t timeSetting);
void bme280_setFilter(uint8_t filterSetting);
void bme280_setTempOverSample(uint8_t overSampleAmount);
void bme280_setPressureOverSample(uint8_t overSampleAmount);
void bme280_setHumidityOverSample(uint8_t overSample);
uint8_t bme280_checkSampleValue(uint8_t userValue);
bool bme280_isMeasuring(void);
void bme280_reset(void);
float bme280_readFloatPressure(void);
void bme280_setReferencePressure(float refPressure);
float bme280_getReferencePressure(void);
float bme280_readFloatAltitudeMeters(void);
float bme280_readFloatAltitudeFeet(void);
float bme280_readFloatHumidity(void);
float bme280_readTempC(void);
float bme280_readTempF(void);
double bme280_dewPointC(void);
double bme280_dewPointF(void);


#endif /* _DRV_BME280_H */