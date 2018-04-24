
#ifndef __BME280_H__
#define __BME280_H__

#include "stdint.h"



// This driver only uses the i2c interface

#define BME280_I2C_ADDRESS1                  (0x76)
#define BME280_I2C_ADDRESS2                  (0x77)

typedef struct SensorSettings
{
		
	uint8_t runMode;
	uint8_t tStandby;
	uint8_t filter;
	uint8_t tempOverSample;
	uint8_t pressOverSample;
	uint8_t humidOverSample;

} SensorSettings;


// You must call begin first, it will read all registers and store 
// calibration data
int i2c_bme280_begin();

// This will free the calibration data, if called you will have to call begin again
void i2c_bme280_end();

// This will put sensor to sleep mode after reading data. 
void i2c_bme280_force_readings( void );


// You must call this first!!
float i2c_bme280_read_temp();

float i2c_bme280_read_temp_F();

// Relative humidity
float i2c_bme280_read_rh();

// Pressure
float i2c_bme280_read_pressure();


uint8_t i2c_bme280_read_register(uint8_t reg);

int i2c_bme280_write_register(uint8_t reg,uint8_t value);

uint8_t i2c_bme280_get_power_mode();


/*calibration parameters */
#define	BME280_CALIB_DATA_SIZE	(26)

#define	BME280_PRESSURE_TEMPERATURE_CALIB_DATA_LENGTH	(26)

#define	BME280_HUMIDITY_CALIB_DATA_LENGTH	(7)

#define	BME280_INIT_VALUE	(0)

#define BME280_SHIFT_BIT_POSITION_BY_04_BITS	(4)
#define BME280_SHIFT_BIT_POSITION_BY_08_BITS	(8)

#define	BME280_MASK_DIG_H4	(0x0F)

///// Registers 
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
#define BME280_HUMIDITY_LSB_REG	        0xFE //Humidity LSB


#define BME280_HUMIDITY_CALIB_DIG_H2_LSB_REG (0xE1)
#define BME280_TEMPERATURE_CALIB_DIG_T1_LSB_REG (0x88)



typedef struct calibrationDataType {

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
    int8_t  dig_H6;
} calibrationDataType;



/****************************************************/
/**\name	ARRAY PARAMETER FOR CALIBRATION     */
/***************************************************/
#define	BME280_TEMPERATURE_CALIB_DIG_T1_LSB		(0)
#define	BME280_TEMPERATURE_CALIB_DIG_T1_MSB		(1)
#define	BME280_TEMPERATURE_CALIB_DIG_T2_LSB		(2)
#define	BME280_TEMPERATURE_CALIB_DIG_T2_MSB		(3)
#define	BME280_TEMPERATURE_CALIB_DIG_T3_LSB		(4)
#define	BME280_TEMPERATURE_CALIB_DIG_T3_MSB		(5)
#define	BME280_PRESSURE_CALIB_DIG_P1_LSB       (6)
#define	BME280_PRESSURE_CALIB_DIG_P1_MSB       (7)
#define	BME280_PRESSURE_CALIB_DIG_P2_LSB       (8)
#define	BME280_PRESSURE_CALIB_DIG_P2_MSB       (9)
#define	BME280_PRESSURE_CALIB_DIG_P3_LSB       (10)
#define	BME280_PRESSURE_CALIB_DIG_P3_MSB       (11)
#define	BME280_PRESSURE_CALIB_DIG_P4_LSB       (12)
#define	BME280_PRESSURE_CALIB_DIG_P4_MSB       (13)
#define	BME280_PRESSURE_CALIB_DIG_P5_LSB       (14)
#define	BME280_PRESSURE_CALIB_DIG_P5_MSB       (15)
#define	BME280_PRESSURE_CALIB_DIG_P6_LSB       (16)
#define	BME280_PRESSURE_CALIB_DIG_P6_MSB       (17)
#define	BME280_PRESSURE_CALIB_DIG_P7_LSB       (18)
#define	BME280_PRESSURE_CALIB_DIG_P7_MSB       (19)
#define	BME280_PRESSURE_CALIB_DIG_P8_LSB       (20)
#define	BME280_PRESSURE_CALIB_DIG_P8_MSB       (21)
#define	BME280_PRESSURE_CALIB_DIG_P9_LSB       (22)
#define	BME280_PRESSURE_CALIB_DIG_P9_MSB       (23)
#define	BME280_HUMIDITY_CALIB_DIG_H1           (25)
#define	BME280_HUMIDITY_CALIB_DIG_H2_LSB		(0)
#define	BME280_HUMIDITY_CALIB_DIG_H2_MSB		(1)
#define	BME280_HUMIDITY_CALIB_DIG_H3			(2)
#define	BME280_HUMIDITY_CALIB_DIG_H4_MSB		(3)
#define	BME280_HUMIDITY_CALIB_DIG_H4_LSB		(4)
#define	BME280_HUMIDITY_CALIB_DIG_H5_MSB		(5)
#define	BME280_HUMIDITY_CALIB_DIG_H6	        (6)



#define BME280_GET_BITSLICE(regvar, bitname)\
    ((regvar & bitname##__MSK) >> bitname##__POS)

/****************************************************/
/**\name	BIT MASK, LENGTH AND POSITION DEFINITIONS
FOR POWER MODE  */
/***************************************************/
#define BME280_CTRL_MEAS_REG_POWER_MODE__POS              (0)
#define BME280_CTRL_MEAS_REG_POWER_MODE__MSK              (0x03)
#define BME280_CTRL_MEAS_REG_POWER_MODE__LEN              (2)
#define BME280_CTRL_MEAS_REG_POWER_MODE__REG              \
   (BME280_CTRL_MEAS_REG)






#endif