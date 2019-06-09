#include <stdio.h>
#include <math.h>
#include "drv_i2c.h"
#include "drv_bme280.h"

//#define BME280_DEBUG

#ifdef BME280_DEBUG
#define debug(fmt, ...) printf("%s" fmt "\n", "BME280: ", ## __VA_ARGS__);
#else
#define debug(fmt, ...)
#endif

/**
 * BME280 registers
 */
#define BME280_REG_TEMP_XLSB   0xFC /* bits: 7-4 */
#define BME280_REG_TEMP_LSB    0xFB
#define BME280_REG_TEMP_MSB    0xFA
#define BME280_REG_TEMP        (BME280_REG_TEMP_MSB)
#define BME280_REG_PRESS_XLSB  0xF9 /* bits: 7-4 */
#define BME280_REG_PRESS_LSB   0xF8
#define BME280_REG_PRESS_MSB   0xF7
#define BME280_REG_PRESSURE    (BME280_REG_PRESS_MSB)
#define BME280_REG_CONFIG      0xF5 /* bits: 7-5 t_sb; 4-2 filter; 0 spi3w_en */
#define BME280_REG_CTRL        0xF4 /* bits: 7-5 osrs_t; 4-2 osrs_p; 1-0 mode */
#define BME280_REG_STATUS      0xF3 /* bits: 3 measuring; 0 im_update */
#define BME280_REG_CTRL_HUM    0xF2 /* bits: 2-0 osrs_h; */
#define BME280_REG_RESET       0xE0
#define BME280_REG_ID          0xD0
#define BME280_REG_CALIB       0x88
#define BME280_REG_HUM_CALIB   0x88


#define BME280_RESET_VALUE     0xB6

//Register names:
#define BME280_DIG_T1_LSB_REG			0x88
#define BME280_DIG_T1_MSB_REG			0x89
#define BME280_DIG_T2_LSB_REG			0x8A
#define BME280_DIG_T2_MSB_REG			0x8B
#define BME280_DIG_T3_LSB_REG			0x8C
#define BME280_DIG_T3_MSB_REG			0x8D
#define BME280_DIG_P1_LSB_REG			0x8E
#define BME280_DIG_P1_MSB_REG			0x8F
#define BME280_DIG_P2_LSB_REG			0x90
#define BME280_DIG_P2_MSB_REG			0x91
#define BME280_DIG_P3_LSB_REG			0x92
#define BME280_DIG_P3_MSB_REG			0x93
#define BME280_DIG_P4_LSB_REG			0x94
#define BME280_DIG_P4_MSB_REG			0x95
#define BME280_DIG_P5_LSB_REG			0x96
#define BME280_DIG_P5_MSB_REG			0x97
#define BME280_DIG_P6_LSB_REG			0x98
#define BME280_DIG_P6_MSB_REG			0x99
#define BME280_DIG_P7_LSB_REG			0x9A
#define BME280_DIG_P7_MSB_REG			0x9B
#define BME280_DIG_P8_LSB_REG			0x9C
#define BME280_DIG_P8_MSB_REG			0x9D
#define BME280_DIG_P9_LSB_REG			0x9E
#define BME280_DIG_P9_MSB_REG			0x9F
#define BME280_DIG_H1_REG				0xA1
#define BME280_CHIP_ID_REG				0xD0 //Chip ID
#define BME280_RST_REG					0xE0 //Softreset Reg
#define BME280_DIG_H2_LSB_REG			0xE1
#define BME280_DIG_H2_MSB_REG			0xE2
#define BME280_DIG_H3_REG				0xE3
#define BME280_DIG_H4_MSB_REG			0xE4
#define BME280_DIG_H4_LSB_REG			0xE5
#define BME280_DIG_H5_MSB_REG			0xE6
#define BME280_DIG_H6_REG				0xE7
#define BME280_CTRL_HUMIDITY_REG		0xF2 //Ctrl Humidity Reg
#define BME280_STAT_REG					0xF3 //Status Reg
#define BME280_CTRL_MEAS_REG			0xF4 //Ctrl Measure Reg
#define BME280_CONFIG_REG				0xF5 //Configuration Reg
#define BME280_PRESSURE_MSB_REG			0xF7 //Pressure MSB
#define BME280_PRESSURE_LSB_REG			0xF8 //Pressure LSB
#define BME280_PRESSURE_XLSB_REG		0xF9 //Pressure XLSB
#define BME280_TEMPERATURE_MSB_REG		0xFA //Temperature MSB
#define BME280_TEMPERATURE_LSB_REG		0xFB //Temperature LSB
#define BME280_TEMPERATURE_XLSB_REG		0xFC //Temperature XLSB
#define BME280_HUMIDITY_MSB_REG			0xFD //Humidity MSB
#define BME280_HUMIDITY_LSB_REG			0xFE //Humidity LSB


bme280_param_t params;
bme280_calib_t calib;
int32_t t_fine;


void bme280_init_params(void)
{
    
	//These are deprecated params
	params.runMode = 3; //Normal/Run
	params.tStandby = 0; //0.5ms
	params.filter = 0; //Filter off
	params.tempOverSample = 1;
	params.pressOverSample = 1;
	params.humidOverSample = 1;
    params.tempCorrection = 0.0; // correction of temperature - added to the result
}

void read_calib_data(void)
{
    uint8_t data[2];
    //Reading all compensation data, range 0x88:A1, 0xE1:E7

    data[0] = i2c_read(i2c_number, BME280_DIG_T1_MSB_REG);
    data[1] = i2c_read(i2c_number, BME280_DIG_T1_LSB_REG);
	calib.dig_T1 = ((uint16_t)((data[0] << 8) + data[1]));

    data[0] = i2c_read(i2c_number, BME280_DIG_T2_MSB_REG);
    data[1] = i2c_read(i2c_number, BME280_DIG_T2_LSB_REG);
	calib.dig_T2 = ((uint16_t)((data[0] << 8) + data[1]));

    data[0] = i2c_read(i2c_number, BME280_DIG_T3_MSB_REG);
    data[1] = i2c_read(i2c_number, BME280_DIG_T3_LSB_REG);
	calib.dig_T3 = ((uint16_t)((data[0] << 8) + data[1]));


    data[0] = i2c_read(i2c_number, BME280_DIG_P1_MSB_REG);
    data[1] = i2c_read(i2c_number, BME280_DIG_P1_LSB_REG);
	calib.dig_P1 = ((uint16_t)((data[0] << 8) + data[1]));

    data[0] = i2c_read(i2c_number, BME280_DIG_P2_MSB_REG);
    data[1] = i2c_read(i2c_number, BME280_DIG_P2_LSB_REG);
	calib.dig_P2 = ((uint16_t)((data[0] << 8) + data[1]));

    data[0] = i2c_read(i2c_number, BME280_DIG_P3_MSB_REG);
    data[1] = i2c_read(i2c_number, BME280_DIG_P3_LSB_REG);
	calib.dig_P3 = ((uint16_t)((data[0] << 8) + data[1]));

    data[0] = i2c_read(i2c_number, BME280_DIG_P4_MSB_REG);
    data[1] = i2c_read(i2c_number, BME280_DIG_P4_LSB_REG);
	calib.dig_P4 = ((uint16_t)((data[0] << 8) + data[1]));
    
    data[0] = i2c_read(i2c_number, BME280_DIG_P5_MSB_REG);
    data[1] = i2c_read(i2c_number, BME280_DIG_P5_LSB_REG);
	calib.dig_P5 = ((uint16_t)((data[0] << 8) + data[1]));

    data[0] = i2c_read(i2c_number, BME280_DIG_P6_MSB_REG);
    data[1] = i2c_read(i2c_number, BME280_DIG_P6_LSB_REG);
	calib.dig_P6 = ((uint16_t)((data[0] << 8) + data[1]));

    data[0] = i2c_read(i2c_number, BME280_DIG_P7_MSB_REG);
    data[1] = i2c_read(i2c_number, BME280_DIG_P7_LSB_REG);
	calib.dig_P7 = ((uint16_t)((data[0] << 8) + data[1]));
    
    data[0] = i2c_read(i2c_number, BME280_DIG_P8_MSB_REG);
    data[1] = i2c_read(i2c_number, BME280_DIG_P8_LSB_REG);
	calib.dig_P8 = ((uint16_t)((data[0] << 8) + data[1]));

    data[0] = i2c_read(i2c_number, BME280_DIG_P9_MSB_REG);
    data[1] = i2c_read(i2c_number, BME280_DIG_P9_LSB_REG);
	calib.dig_P9 = ((uint16_t)((data[0] << 8) + data[1]));


    data[0] = i2c_read(i2c_number, BME280_DIG_H1_REG);
	calib.dig_H1 = (uint8_t)(data[0]);

    data[0] = i2c_read(i2c_number, BME280_DIG_H2_MSB_REG);
    data[1] = i2c_read(i2c_number, BME280_DIG_H2_LSB_REG);
	calib.dig_H2 = ((uint16_t)((data[0] << 8) + data[1]));
    
    data[0] = i2c_read(i2c_number, BME280_DIG_H3_REG);
	calib.dig_H3 = (uint8_t)(data[0]);

    data[0] = i2c_read(i2c_number, BME280_DIG_H4_MSB_REG);
    data[1] = i2c_read(i2c_number, BME280_DIG_H4_LSB_REG);
	calib.dig_H4 = ((int16_t)((data[0] << 4) + (data[1] & 0x0F)));

    data[0] = i2c_read(i2c_number, BME280_DIG_H5_MSB_REG);
    data[1] = i2c_read(i2c_number, BME280_DIG_H4_LSB_REG);
	calib.dig_H5 = ((int16_t)((data[0] << 4) + ((data[1] >> 4) & 0x0F)));
	
    data[0] = i2c_read(i2c_number, BME280_DIG_H6_REG);
	calib.dig_H6 = (uint8_t)(data[0]);
    
    debug("calib data received:");
    debug("dig_T1=%d", calib.dig_T1);
    debug("dig_T2=%d", calib.dig_T2);
    debug("dig_T3=%d", calib.dig_T3);
    debug("dig_P1=%d", calib.dig_P1);
    debug("dig_P2=%d", calib.dig_P2);
    debug("dig_P3=%d", calib.dig_P3);
    debug("dig_P4=%d", calib.dig_P4);
    debug("dig_P5=%d", calib.dig_P5);
    debug("dig_P6=%d", calib.dig_P6);
    debug("dig_P7=%d", calib.dig_P7);
    debug("dig_P8=%d", calib.dig_P8);
    debug("dig_P9=%d", calib.dig_P9);
    debug("dig_H1=%d", calib.dig_H1);
    debug("dig_H2=%d", calib.dig_H2);
    debug("dig_H3=%d", calib.dig_H3);
    debug("dig_H4=%d", calib.dig_H4);
    debug("dig_H5=%d", calib.dig_H5);
    debug("dig_H6=%d", calib.dig_H6);

}

//Set the mode bits in the ctrl_meas register
// Mode 00 = Sleep
// 01 and 10 = Forced
// 11 = Normal mode
void bme280_setMode(uint8_t mode)
{
	if(mode > 0b11) mode = 0; //Error check. Default to sleep mode
	uint8_t controlData = i2c_read(i2c_number, BME280_CTRL_MEAS_REG);
	controlData &= ~( (1<<1) | (1<<0) ); //Clear the mode[1:0] bits
	controlData |= mode; //Set
    i2c_write(i2c_number, BME280_CTRL_MEAS_REG, controlData);
}

uint8_t bme280_init(void)
{
    if (slave_address != BME280_I2C_ADDRESS_0 && slave_address != BME280_I2C_ADDRESS_1) {
        debug("Invalid I2C address");
        return false;
    }

    calib.id = i2c_read(i2c_number, BME280_REG_ID);
    debug("calib id = 0x%x", calib.id);

    if (calib.id != BME280_CHIP_ID) {
        debug("Sensor wrong version");
        return false;
    }

    // Soft reset.
    i2c_write(i2c_number, BME280_REG_RESET, BME280_RESET_VALUE);
   
    read_calib_data();

    bme280_setStandbyTime(params.tStandby);
	bme280_setFilter(params.filter);
	bme280_setPressureOverSample(params.pressOverSample); //Default of 1x oversample
	bme280_setHumidityOverSample(params.humidOverSample); //Default of 1x oversample
	bme280_setTempOverSample(params.tempOverSample); //Default of 1x oversample
	
	bme280_setMode(MODE_NORMAL); //Go!
	
	return i2c_read(i2c_number, BME280_CHIP_ID_REG); //Should return 0x60

}


//Gets the current mode bits in the ctrl_meas register
//Mode 00 = Sleep
// 01 and 10 = Forced
// 11 = Normal mode
uint8_t bme280_getMode(void)
{
	uint8_t controlData = i2c_read(i2c_number, BME280_CTRL_MEAS_REG);
	return(controlData & 0b00000011); //Clear bits 7 through 2
}

//Set the standby bits in the config register
//tStandby can be:
//  0, 0.5ms
//  1, 62.5ms
//  2, 125ms
//  3, 250ms
//  4, 500ms
//  5, 1000ms
//  6, 10ms
//  7, 20ms
void bme280_setStandbyTime(uint8_t timeSetting)
{
	if(timeSetting > 0b111) timeSetting = 0; //Error check. Default to 0.5ms
	
	uint8_t controlData = i2c_read(i2c_number, BME280_CONFIG_REG);
	controlData &= ~( (1<<7) | (1<<6) | (1<<5) ); //Clear the 7/6/5 bits
	controlData |= (timeSetting << 5); //Align with bits 7/6/5
	i2c_write(i2c_number, BME280_CONFIG_REG, controlData);
}


//Set the filter bits in the config register
//filter can be off or number of FIR coefficients to use:
//  0, filter off
//  1, coefficients = 2
//  2, coefficients = 4
//  3, coefficients = 8
//  4, coefficients = 16
void bme280_setFilter(uint8_t filterSetting)
{
	if(filterSetting > 0b111) filterSetting = 0; //Error check. Default to filter off
	
	uint8_t controlData = i2c_read(i2c_number, BME280_CONFIG_REG);
	controlData &= ~( (1<<4) | (1<<3) | (1<<2) ); //Clear the 4/3/2 bits
	controlData |= (filterSetting << 2); //Align with bits 4/3/2
	i2c_write(i2c_number, BME280_CONFIG_REG, controlData);
}

//Set the temperature oversample value
//0 turns off temp sensing
//1 to 16 are valid over sampling values
void bme280_setTempOverSample(uint8_t overSampleAmount)
{
	overSampleAmount = bme280_checkSampleValue(overSampleAmount); //Error check
	
	uint8_t originalMode = bme280_getMode(); //Get the current mode so we can go back to it at the end
	
	bme280_setMode(MODE_SLEEP); //Config will only be writeable in sleep mode, so first go to sleep mode

	//Set the osrs_t bits (7, 6, 5) to overSampleAmount
	uint8_t controlData = i2c_read(i2c_number, BME280_CTRL_MEAS_REG);
	controlData &= ~( (1<<7) | (1<<6) | (1<<5) ); //Clear bits 765
	controlData |= overSampleAmount << 5; //Align overSampleAmount to bits 7/6/5
	i2c_write(i2c_number, BME280_CTRL_MEAS_REG, controlData);
	
	bme280_setMode(originalMode); //Return to the original user's choice
}


//Set the pressure oversample value
//0 turns off pressure sensing
//1 to 16 are valid over sampling values
void bme280_setPressureOverSample(uint8_t overSampleAmount)
{
	overSampleAmount = bme280_checkSampleValue(overSampleAmount); //Error check
	
	uint8_t originalMode = bme280_getMode(); //Get the current mode so we can go back to it at the end
	
	bme280_setMode(MODE_SLEEP); //Config will only be writeable in sleep mode, so first go to sleep mode

	//Set the osrs_p bits (4, 3, 2) to overSampleAmount
	uint8_t controlData = i2c_read(i2c_number, BME280_CTRL_MEAS_REG);
	controlData &= ~( (1<<4) | (1<<3) | (1<<2) ); //Clear bits 432
	controlData |= overSampleAmount << 2; //Align overSampleAmount to bits 4/3/2
	i2c_write(i2c_number, BME280_CTRL_MEAS_REG, controlData);
	
	bme280_setMode(originalMode); //Return to the original user's choice
}

//Set the humidity oversample value
//0 turns off humidity sensing
//1 to 16 are valid over sampling values
void bme280_setHumidityOverSample(uint8_t overSample)
{
	uint8_t overSampleAmount = bme280_checkSampleValue(overSample); //Error check
	
	uint8_t originalMode = bme280_getMode(); //Get the current mode so we can go back to it at the end
	
	bme280_setMode(MODE_SLEEP); //Config will only be writeable in sleep mode, so first go to sleep mode

	//Set the osrs_h bits (2, 1, 0) to overSampleAmount
	uint8_t controlData = i2c_read(i2c_number, BME280_CTRL_HUMIDITY_REG);
	controlData &= ~( (1<<2) | (1<<1) | (1<<0) ); //Clear bits 2/1/0
	controlData |= overSampleAmount << 0; //Align overSampleAmount to bits 2/1/0
	i2c_write(i2c_number, BME280_CTRL_HUMIDITY_REG, controlData);

	bme280_setMode(originalMode); //Return to the original user's choice
}

//Validates an over sample value
//Allowed values are 0 to 16
//These are used in the humidty, pressure, and temp oversample functions
uint8_t bme280_checkSampleValue(uint8_t userValue)
{
	switch(userValue) 
	{
		case 0: 
			return 0;
			break; //Valid
		case 1: 
			return 1;
			break; //Valid
		case 2: 
			return 2;
			break; //Valid
		case 4: 
			return 3;
			break; //Valid
		case 8: 
			return 4;
			break; //Valid
		case 16: 
			return 5;
			break; //Valid
		default: 
			return 1; //Default to 1x
			break; //Good
	}
}


//Check the measuring bit and return true while device is taking measurement
bool bme280_isMeasuring(void)
{
	uint8_t stat = i2c_read(i2c_number, BME280_STAT_REG);
	return(stat & (1<<3)); //If the measuring bit (3) is set, return true
}

//Strictly resets.  Run .begin() afterwards
void bme280_reset(void)
{
	i2c_write(i2c_number, BME280_RST_REG, 0xB6);
	
}


//  Pressure Section
float bme280_readFloatPressure(void)
{

	// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
	// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
    uint8_t buffer[3];
    //readRegisterRegion(buffer, BME280_PRESSURE_MSB_REG, 3);
    buffer[0] = i2c_read(i2c_number, 0xF7);
    buffer[1] = i2c_read(i2c_number, 0xF8);
    buffer[2] = i2c_read(i2c_number, 0xF9);
	
    int32_t adc_P = ((uint32_t)buffer[0] << 12) | ((uint32_t)buffer[1] << 4) | ((buffer[2] >> 4) & 0x0F);
	
	int64_t var1, var2, p_acc;
	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)calib.dig_P6;
	var2 = var2 + ((var1 * (int64_t)calib.dig_P5)<<17);
	var2 = var2 + (((int64_t)calib.dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)calib.dig_P3)>>8) + ((var1 * (int64_t)calib.dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)calib.dig_P1)>>33;
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p_acc = 1048576 - adc_P;
	p_acc = (((p_acc<<31) - var2)*3125)/var1;
	var1 = (((int64_t)calib.dig_P9) * (p_acc>>13) * (p_acc>>13)) >> 25;
	var2 = (((int64_t)calib.dig_P8) * p_acc) >> 19;
	p_acc = ((p_acc + var1 + var2) >> 8) + (((int64_t)calib.dig_P7)<<4);
	
	return (float)p_acc / 256.0;
	
}

float referencePressure;
//Sets the internal variable _referencePressure so the 
void bme280_setReferencePressure(float refPressure)
{
	referencePressure = refPressure;
}

//Return the local reference pressure
float bme280_getReferencePressure(void)
{
	return(referencePressure);
}

float bme280_readFloatAltitudeMeters(void)
{
	float heightOutput = 0;
	
	//heightOutput = ((float)-45846.2)*(pow(((float)readFloatPressure()/(float)_referencePressure), 0.190263) - (float)1);
	heightOutput = ((float)-44330.77)*(pow(((float)bme280_readFloatPressure()/(float)referencePressure), 0.190263) - (float)1); //Corrected, see issue 30
	return heightOutput;
	
}

float bme280_readFloatAltitudeFeet(void)
{
	float heightOutput = 0;
	
	heightOutput = bme280_readFloatAltitudeMeters() * 3.28084;
	return heightOutput;
	
}


//  Humidity Section

float bme280_readFloatHumidity(void)
{
	
	// Returns humidity in %RH as unsigned 32 bit integer in Q22. 10 format (22 integer and 10 fractional bits).
	// Output value of “47445” represents 47445/1024 = 46. 333 %RH
    uint8_t buffer[2];
	//readRegisterRegion(buffer, BME280_HUMIDITY_MSB_REG, 2);
    buffer[0] = i2c_read(i2c_number, 0xFD);
    buffer[1] = i2c_read(i2c_number, 0xFE);
    int32_t adc_H = ((uint32_t)buffer[0] << 8) | ((uint32_t)buffer[1]);
	
	int32_t var1;
	var1 = (t_fine - ((int32_t)76800));
	var1 = (((((adc_H << 14) - (((int32_t)calib.dig_H4) << 20) - (((int32_t)calib.dig_H5) * var1)) +
	((int32_t)16384)) >> 15) * (((((((var1 * ((int32_t)calib.dig_H6)) >> 10) * (((var1 * ((int32_t)calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
	((int32_t)calib.dig_H2) + 8192) >> 14));
	var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)calib.dig_H1)) >> 4));
	var1 = (var1 < 0 ? 0 : var1);
	var1 = (var1 > 419430400 ? 419430400 : var1);

	return (float)(var1>>12) / 1024.0;
}


//  Temperature Section


float bme280_readTempC(void)
{
	// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
	// t_fine carries fine temperature as global value

	//get the reading (adc_T);
    uint8_t buffer[3];
	//readRegisterRegion(buffer, BME280_TEMPERATURE_MSB_REG, 3);
    buffer[0] = i2c_read(i2c_number, 0xFA);
    buffer[1] = i2c_read(i2c_number, 0xFB);
    buffer[2] = i2c_read(i2c_number, 0xFC);
    int32_t adc_T = ((uint32_t)buffer[0] << 12) | ((uint32_t)buffer[1] << 4) | ((buffer[2] >> 4) & 0x0F);

	//By datasheet, calibrate
	int64_t var1, var2;

	var1 = ((((adc_T>>3) - ((int32_t)calib.dig_T1<<1))) * ((int32_t)calib.dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((int32_t)calib.dig_T1)) * ((adc_T>>4) - ((int32_t)calib.dig_T1))) >> 12) *
	((int32_t)calib.dig_T3)) >> 14;
	t_fine = var1 + var2;
	float output = (t_fine * 5 + 128) >> 8;

	output = output / 100 + params.tempCorrection;
	
	return output;
}

float bme280_readTempF(void)
{
	float output = bme280_readTempC();
	output = (output * 9) / 5 + 32;

	return output;
}


//  Dew point Section

// Returns Dew point in DegC
double bme280_dewPointC(void)
{
    double celsius = bme280_readTempC(); 
    double humidity = bme280_readFloatHumidity();
    // (1) Saturation Vapor Pressure = ESGG(T)
    double RATIO = 373.15 / (273.15 + celsius);
    double RHS = -7.90298 * (RATIO - 1);
    RHS += 5.02808 * log10(RATIO);
    RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1/RATIO ))) - 1) ;
    RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
    RHS += log10(1013.246);
            // factor -3 is to adjust units - Vapor Pressure SVP * humidity
    double VP = pow(10, RHS - 3) * humidity;
            // (2) DEWPOINT = F(Vapor Pressure)
    double T = log(VP/0.61078);   // temp var
    return (241.88 * T) / (17.558 - T);
}

// Returns Dew point in DegF

double bme280_dewPointF(void)
{
    double degC = bme280_dewPointC();
    degC = degC*1.8 + 32;
    return degC; //Convert C to F
}
