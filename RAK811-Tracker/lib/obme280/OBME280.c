/*
 ****************************************************************************
* Copyright (C) 2015 - 2016 Bosch Sensortec GmbH
*
* bme280.c
* Date: 2016/07/04
* Revision: 2.0.5(Pressure and Temperature compensation code revision is 1.1
*               and Humidity compensation code revision is 1.0)
*
* Usage: Sensor Driver file for BME280 sensor
* Inspiration from
*  https://github.com/BoschSensortec/BME280_driver
*  https://github.com/sparkfun/SparkFun_BME280_Arduino_Library
*
****************************************************************************
* License:
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*   Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
*
*   Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer in the
*   documentation and/or other materials provided with the distribution.
*
*   Neither the name of the copyright holder nor the names of the
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
* OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
* OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
*
* The information provided is believed to be accurate and reliable.
* The copyright holder assumes no responsibility
* for the consequences of use
* of such information nor for any infringement of patents or
* other rights of third parties which may result from its use.
* No license is granted by implication or otherwise under any patent or
* patent rights of the copyright holder.
**************************************************************************/
 
 
/*
 * esp32 port by Olof Astrand <olof.astrand@gmail.com>
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 *
*/

/** 
    @brief esp-idf code to read temperature and pressure from Bosch BME280 based device
    You must set up inc devices pins e.t.c before using this

*/

#include <stdio.h>
#include "stdio.h" 

#include "gpio.h"
#include "i2c.h"
#include "OBME280.h"
#include "board.h"

typedef	signed char  s8;/**< used for signed 8bit */
typedef	signed short int s16;/**< used for signed 16bit */
typedef	signed int s32;/**< used for signed 32bit */
typedef	signed long long int s64;/**< used for signed 64bit */

/*unsigned integer types*/
typedef	unsigned char u8;/**< used for unsigned 8bit */
typedef	unsigned short int u16;/**< used for unsigned 16bit */
typedef	unsigned int u32;/**< used for unsigned 32bit */
typedef	unsigned long long int u64;/**< used for unsigned 64bit */

// t_fine carries fine temperature
int32_t t_fine;

#define WRITE_BIT  0 /*!< I2C master write */
#define READ_BIT   1  /*!< I2C master read */
#define ACK_CHECK_EN   0x1     /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS  0x0     /*!< I2C master will not check ack from slave */
#define ACK_VAL    0x0         /*!< I2C ack value */
#define NACK_VAL   0x1         /*!< I2C nack value */

#define  BME720_ADDRESS    0x77
#define I2C_MASTER_NUM   I2C_NUM_0   /*!< I2C port number for master dev */

calibrationDataType *calib_data=NULL;

SensorSettings settings;

I2c_t BME280I2C;

/**
 *   https://www.bosch-sensortec.com/bst/products/all_products/bme280
 * @brief  read all registers from a bme280 device
 * _________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write 1 byte + ack  | stop |
 * --------|---------------------------|---------------------|------|
 * 2. wait more than 24 ms
 * 3. read data
 * ______________________________________________________________________________________
 * | start | slave_addr + rd_bit + ack | read 1 byte + ack  ... read 1 byte + nack | stop |
 * --------|---------------------------|---------------------|--------------------|------|
 */

int i2c_bme280_begin() {
// Settings

   //runMode can be:
   //  0, Sleep mode
   //  1 or 2, Forced mode
   //  3, Normal mode
  settings.runMode = 3;

	//tStandby can be:
	//  0, 0.5ms
	//  1, 62.5ms
	//  2, 125ms
	//  3, 250ms
	//  4, 500ms
	//  5, 1000ms
	//  6, 10ms
	//  7, 20ms
	settings.tStandby = 0;
	
	//filter can be off or number of FIR coefficients to use:
	//  0, filter off
	//  1, coefficients = 2
	//  2, coefficients = 4
	//  3, coefficients = 8
	//  4, coefficients = 16
	settings.filter = 0;
	
	//tempOverSample can be:
	//  0, skipped
	//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
	settings.tempOverSample = 1;

	//pressOverSample can be:
	//  0, skipped
	//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
    settings.pressOverSample = 1;
	
	//humidOverSample can be:
	//  0, skipped
	//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
   settings.humidOverSample = 1;


   I2cInit(&BME280I2C, I2C_SCL,I2C_SDA  );



   //i2c_port_t i2c_num = I2C_MASTER_NUM;
   uint8_t dataToWrite = 0; //Temporary variable
    u8 a_data_u8[BME280_CALIB_DATA_SIZE] = {
	BME280_INIT_VALUE, BME280_INIT_VALUE,
	BME280_INIT_VALUE, BME280_INIT_VALUE, BME280_INIT_VALUE,
	BME280_INIT_VALUE, BME280_INIT_VALUE, BME280_INIT_VALUE,
	BME280_INIT_VALUE, BME280_INIT_VALUE, BME280_INIT_VALUE,
	BME280_INIT_VALUE, BME280_INIT_VALUE, BME280_INIT_VALUE,
	BME280_INIT_VALUE, BME280_INIT_VALUE, BME280_INIT_VALUE,
	BME280_INIT_VALUE, BME280_INIT_VALUE, BME280_INIT_VALUE,
	BME280_INIT_VALUE, BME280_INIT_VALUE, BME280_INIT_VALUE,
    BME280_INIT_VALUE, BME280_INIT_VALUE, BME280_INIT_VALUE};

    calib_data=(calibrationDataType *)malloc(sizeof(calibrationDataType));
    if (calib_data==NULL) return false;

    //i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    //i2c_master_start(cmd);
	I2cWrite(&BME280I2C, ( BME280_I2C_ADDRESS2 << 1 ) | WRITE_BIT,BME280_TEMPERATURE_CALIB_DIG_T1_LSB_REG,0);
    //i2c_master_write_byte(cmd, ( BME280_I2C_ADDRESS2 << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    //i2c_master_write_byte(cmd,  BME280_TEMPERATURE_CALIB_DIG_T1_LSB_REG, ACK_CHECK_EN);
    //i2c_master_stop(cmd);
    //int ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    //i2c_cmd_link_delete(cmd);

    //vTaskDelay(200 / portTICK_RATE_MS);
	HAL_Delay(200);

	I2cReadBuffer(&BME280I2C, BME280_I2C_ADDRESS2 << 1 | READ_BIT,BME280_TEMPERATURE_CALIB_DIG_T1_LSB_REG, a_data_u8,BME280_PRESSURE_TEMPERATURE_CALIB_DATA_LENGTH);
    //cmd = i2c_cmd_link_create();
    //i2c_master_start(cmd);
    //i2c_master_write_byte(cmd, BME280_I2C_ADDRESS2 << 1 | READ_BIT, ACK_CHECK_EN);
    //i2c_master_read(cmd, a_data_u8, BME280_PRESSURE_TEMPERATURE_CALIB_DATA_LENGTH - 1, ACK_VAL);
    //i2c_master_read_byte(cmd, &a_data_u8[BME280_PRESSURE_TEMPERATURE_CALIB_DATA_LENGTH-1], NACK_VAL);
    //i2c_master_stop(cmd);
    //ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    //i2c_cmd_link_delete(cmd);


    calib_data->dig_T1 = (u16)(((
                                        (u16)((u8)a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T1_MSB]))
                                    << BME280_SHIFT_BIT_POSITION_BY_08_BITS) |
                                   a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T1_LSB]);

    calib_data->dig_T2 = (s16)(((
                                    (s16)((s8)a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T2_MSB]))
                                << BME280_SHIFT_BIT_POSITION_BY_08_BITS) |
                               a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T2_LSB]);
    calib_data->dig_T3 = (s16)(((
                                    (s16)((s8)a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T3_MSB]))
                                << BME280_SHIFT_BIT_POSITION_BY_08_BITS) |
                               a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T3_LSB]);
    calib_data->dig_P1 = (u16)(((
                                    (u16)((u8)a_data_u8[BME280_PRESSURE_CALIB_DIG_P1_MSB]))
                                << BME280_SHIFT_BIT_POSITION_BY_08_BITS) |
                               a_data_u8[BME280_PRESSURE_CALIB_DIG_P1_LSB]);
    calib_data->dig_P2 = (s16)(((
                                    (s16)((s8)a_data_u8[BME280_PRESSURE_CALIB_DIG_P2_MSB]))
                                << BME280_SHIFT_BIT_POSITION_BY_08_BITS) |
                               a_data_u8[BME280_PRESSURE_CALIB_DIG_P2_LSB]);
    calib_data->dig_P3 = (s16)(((
                                    (s16)((s8)a_data_u8[BME280_PRESSURE_CALIB_DIG_P3_MSB]))
                                << BME280_SHIFT_BIT_POSITION_BY_08_BITS) |
                               a_data_u8[BME280_PRESSURE_CALIB_DIG_P3_LSB]);
    calib_data->dig_P4 = (s16)(((
                                    (s16)((s8)a_data_u8[BME280_PRESSURE_CALIB_DIG_P4_MSB]))
                                << BME280_SHIFT_BIT_POSITION_BY_08_BITS) |
                               a_data_u8[BME280_PRESSURE_CALIB_DIG_P4_LSB]);
    calib_data->dig_P5 = (s16)(((
                                    (s16)((s8)a_data_u8[BME280_PRESSURE_CALIB_DIG_P5_MSB]))
                                << BME280_SHIFT_BIT_POSITION_BY_08_BITS) |
                               a_data_u8[BME280_PRESSURE_CALIB_DIG_P5_LSB]);
    calib_data->dig_P6 = (s16)(((
                                    (s16)((s8)a_data_u8[BME280_PRESSURE_CALIB_DIG_P6_MSB]))
                                << BME280_SHIFT_BIT_POSITION_BY_08_BITS) |
                               a_data_u8[BME280_PRESSURE_CALIB_DIG_P6_LSB]);
    calib_data->dig_P7 = (s16)(((
                                    (s16)((s8)a_data_u8[BME280_PRESSURE_CALIB_DIG_P7_MSB]))
                                << BME280_SHIFT_BIT_POSITION_BY_08_BITS) |
                               a_data_u8[BME280_PRESSURE_CALIB_DIG_P7_LSB]);
    calib_data->dig_P8 = (s16)(((
                                    (s16)((s8)a_data_u8[BME280_PRESSURE_CALIB_DIG_P8_MSB]))
                                << BME280_SHIFT_BIT_POSITION_BY_08_BITS) |
                               a_data_u8[BME280_PRESSURE_CALIB_DIG_P8_LSB]);
    calib_data->dig_P9 = (s16)(((
                                    (s16)((s8)a_data_u8[BME280_PRESSURE_CALIB_DIG_P9_MSB]))
                                << BME280_SHIFT_BIT_POSITION_BY_08_BITS) |
                               a_data_u8[BME280_PRESSURE_CALIB_DIG_P9_LSB]);
    calib_data->dig_H1 = a_data_u8[BME280_HUMIDITY_CALIB_DIG_H1];
    
////////////// Humidity calibration

    //cmd = i2c_cmd_link_create();
    //i2c_master_start(cmd);
    //i2c_master_write_byte(cmd, ( BME280_I2C_ADDRESS2 << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    //i2c_master_write_byte(cmd,  BME280_HUMIDITY_CALIB_DIG_H2_LSB_REG, ACK_CHECK_EN);
    //i2c_master_stop(cmd);
    //ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    //i2c_cmd_link_delete(cmd);
    I2cWrite(&BME280I2C, ( BME280_I2C_ADDRESS2 << 1 ) | WRITE_BIT,BME280_HUMIDITY_CALIB_DIG_H2_LSB_REG,0);


    //vTaskDelay(200 / portTICK_RATE_MS);
    HAL_Delay(200);

	     
    //cmd = i2c_cmd_link_create();
    //i2c_master_start(cmd);
    //i2c_master_write_byte(cmd, BME280_I2C_ADDRESS2 << 1 | READ_BIT, ACK_CHECK_EN);
    //i2c_master_read(cmd, a_data_u8, BME280_HUMIDITY_CALIB_DATA_LENGTH - 1, ACK_VAL);
    //i2c_master_read_byte(cmd, &a_data_u8[BME280_HUMIDITY_CALIB_DATA_LENGTH-1], NACK_VAL);
    //i2c_master_stop(cmd);
    //ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    //i2c_cmd_link_delete(cmd);
    I2cReadBuffer(&BME280I2C, BME280_I2C_ADDRESS2 << 1 | READ_BIT,BME280_HUMIDITY_CALIB_DIG_H2_LSB_REG, a_data_u8,BME280_PRESSURE_TEMPERATURE_CALIB_DATA_LENGTH);


    calib_data->dig_H2 = (s16)(((
                                    (s16)((s8)a_data_u8[BME280_HUMIDITY_CALIB_DIG_H2_MSB]))
                                << BME280_SHIFT_BIT_POSITION_BY_08_BITS) |
                               a_data_u8[BME280_HUMIDITY_CALIB_DIG_H2_LSB]);
    calib_data->dig_H3 =
        a_data_u8[BME280_HUMIDITY_CALIB_DIG_H3];
    calib_data->dig_H4 = (s16)(((
                                    (s16)((s8)a_data_u8[BME280_HUMIDITY_CALIB_DIG_H4_MSB]))
                                << BME280_SHIFT_BIT_POSITION_BY_04_BITS) |
                               (((u8)BME280_MASK_DIG_H4) &
                                a_data_u8[BME280_HUMIDITY_CALIB_DIG_H4_LSB]));
    calib_data->dig_H5 = (s16)(((
                                    (s16)((s8)a_data_u8[BME280_HUMIDITY_CALIB_DIG_H5_MSB]))
                                << BME280_SHIFT_BIT_POSITION_BY_04_BITS) |
                               (a_data_u8[BME280_HUMIDITY_CALIB_DIG_H4_LSB] >>
                                BME280_SHIFT_BIT_POSITION_BY_04_BITS));
    calib_data->dig_H6 =(s8)a_data_u8[BME280_HUMIDITY_CALIB_DIG_H6];


    //calib_data=(calibrationDataType *)malloc(sizeof(calibrationDataType));
    //if (calib_data==NULL) return false;


    //Checking all compensation data, range 0x88:A1, 0xE1:E7
#if 0	
    if (calib_data->dig_T1!=((uint16_t)((i2c_bme280_read_register(BME280_DIG_T1_MSB_REG) << 8) + i2c_bme280_read_register(BME280_DIG_T1_LSB_REG)))) printf("T1 diff\n");
    if (calib_data->dig_T2!= ((int16_t)((i2c_bme280_read_register(BME280_DIG_T2_MSB_REG) << 8) + i2c_bme280_read_register(BME280_DIG_T2_LSB_REG)))) printf("T2 diff\n");
    if (calib_data->dig_T3!= ((int16_t)((i2c_bme280_read_register(BME280_DIG_T3_MSB_REG) << 8) + i2c_bme280_read_register(BME280_DIG_T3_LSB_REG)))) printf("T3 diff\n");

    if (calib_data->dig_P1!= ((uint16_t)((i2c_bme280_read_register(BME280_DIG_P1_MSB_REG) << 8) + i2c_bme280_read_register(BME280_DIG_P1_LSB_REG)))) printf("P1 diff\n");
    if (calib_data->dig_P2!= ((int16_t)((i2c_bme280_read_register(BME280_DIG_P2_MSB_REG) << 8) + i2c_bme280_read_register(BME280_DIG_P2_LSB_REG)))) printf("P2 diff\n");
    if (calib_data->dig_P3!= ((int16_t)((i2c_bme280_read_register(BME280_DIG_P3_MSB_REG) << 8) + i2c_bme280_read_register(BME280_DIG_P3_LSB_REG)))) printf("P3 diff\n");
    if (calib_data->dig_P4!= ((int16_t)((i2c_bme280_read_register(BME280_DIG_P4_MSB_REG) << 8) + i2c_bme280_read_register(BME280_DIG_P4_LSB_REG)))) printf("P4 diff\n");
    if (calib_data->dig_P5!= ((int16_t)((i2c_bme280_read_register(BME280_DIG_P5_MSB_REG) << 8) + i2c_bme280_read_register(BME280_DIG_P5_LSB_REG)))) printf("P5 diff\n");
    if (calib_data->dig_P6!= ((int16_t)((i2c_bme280_read_register(BME280_DIG_P6_MSB_REG) << 8) + i2c_bme280_read_register(BME280_DIG_P6_LSB_REG)))) printf("P6 diff\n");
    if (calib_data->dig_P7!= ((int16_t)((i2c_bme280_read_register(BME280_DIG_P7_MSB_REG) << 8) + i2c_bme280_read_register(BME280_DIG_P7_LSB_REG)))) printf("P7 diff\n");
    if (calib_data->dig_P8!= ((int16_t)((i2c_bme280_read_register(BME280_DIG_P8_MSB_REG) << 8) + i2c_bme280_read_register(BME280_DIG_P8_LSB_REG)))) printf("P8 diff\n");
    if (calib_data->dig_P9!= ((int16_t)((i2c_bme280_read_register(BME280_DIG_P9_MSB_REG) << 8) + i2c_bme280_read_register(BME280_DIG_P9_LSB_REG)))) printf("P9 diff\n");

    if (calib_data->dig_H1!= ((uint8_t)(i2c_bme280_read_register(BME280_DIG_H1_REG)))) printf("H1 diff\n");
    if (calib_data->dig_H2!= ((int16_t)((i2c_bme280_read_register(BME280_DIG_H2_MSB_REG) << 8) + i2c_bme280_read_register(BME280_DIG_H2_LSB_REG)))) printf("H2 diff\n");
    if (calib_data->dig_H3!= ((uint8_t)(i2c_bme280_read_register(BME280_DIG_H3_REG)))) printf("H3 diff\n");
    if (calib_data->dig_H4!= ((int16_t)((i2c_bme280_read_register(BME280_DIG_H4_MSB_REG) << 4) + (i2c_bme280_read_register(BME280_DIG_H4_LSB_REG) & 0x0F)))) printf("H4 diff\n");
    if (calib_data->dig_H5!= ((int16_t)((i2c_bme280_read_register(BME280_DIG_H5_MSB_REG) << 4) + ((i2c_bme280_read_register(BME280_DIG_H4_LSB_REG) >> 4) & 0x0F)))) printf("H5 diff\n");
    if (calib_data->dig_H6!= ((uint8_t)i2c_bme280_read_register(BME280_DIG_H6_REG))) printf("H6 diff\n");
#endif

        //Set the oversampling control words.
	//config will only be writeable in sleep mode, so first insure that.
	i2c_bme280_write_register(BME280_CTRL_MEAS_REG, 0x00);
	
	//Set the config word
	dataToWrite = ( settings.tStandby << 0x5) & 0xE0;
	dataToWrite |= ( settings.filter << 0x02) & 0x1C;
	i2c_bme280_write_register(BME280_CONFIG_REG, dataToWrite);
	
	//Set ctrl_hum first, then ctrl_meas to activate ctrl_hum
	dataToWrite = settings.humidOverSample & 0x07; //all other bits can be ignored
	i2c_bme280_write_register(BME280_CTRL_HUMIDITY_REG, dataToWrite);
	
	//set ctrl_meas
	//First, set temp oversampling
	dataToWrite = (settings.tempOverSample << 0x5) & 0xE0;
	//Next, pressure oversampling
	dataToWrite |= (settings.pressOverSample << 0x02) & 0x1C;
	//Last, set mode
	dataToWrite |= (settings.runMode) & 0x03;
	//Load the byte
    i2c_bme280_write_register(BME280_CTRL_MEAS_REG, dataToWrite);


    return true;
}

/**
 * @brief  Free buffer used by driver
*/
void i2c_bme280_end() {
    if (calib_data) free(calib_data);
    calib_data=NULL;
}


void i2c_bme280_force_readings( void )
{
	//set Force mode
	settings.runMode = 0x01;	//Table 25: register settings mode
	uint8_t dataToWrite = 0;
	dataToWrite = (settings.tempOverSample << 0x5) & 0xE0;
	dataToWrite |= (settings.pressOverSample << 0x02) & 0x1C;
	dataToWrite |= (settings.runMode) & 0x03;
	i2c_bme280_write_register(BME280_CTRL_MEAS_REG, dataToWrite);

	//sleep while measurements are happening
	//(we'll use t_measure,max in "9.1 Measurement time" of the datasheet)
	uint32_t t_measure_us = 1250;		// microseconds
	if (settings.tempOverSample) {
		t_measure_us += 2300 * settings.tempOverSample;
	}
	if (settings.pressOverSample) {
		t_measure_us += 2300 * settings.pressOverSample + 575;
	}
	if (settings.humidOverSample) {
		t_measure_us += 2300 * settings.humidOverSample + 575;
	}
    // Should be ms rather than us
    //vTaskDelay(t_measure_us / portTICK_RATE_MS);
	// TODO, Sleep / 1000
	HAL_Delay(t_measure_us/100);

	//delay((t_measure_us / 1000) + 1);	// round up
}


void i2c_bme280_scan(uint8_t reg) {
    uint8_t data=0;
	int times=0;
    while (times++<127) {
		I2cRead(&BME280I2C,times << 1  | READ_BIT ,reg,&data);
        printf("result(%d) %x\r\n",times,data);
		
		GpioToggle(&Led2);
	}
}

uint8_t i2c_bme280_read_register(uint8_t reg)
{
	uint8_t data=0;
	/*
    i2c_port_t i2c_num = I2C_MASTER_NUM;
  

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, BME280_I2C_ADDRESS2 << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd,  reg, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    int ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_FAIL) {
        return ret;
    }
    vTaskDelay(30 / portTICK_RATE_MS);

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, BME280_I2C_ADDRESS2 << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, &data, NACK_VAL);

    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
*/
    I2cRead(&BME280I2C,BME280_I2C_ADDRESS2 << 1 | READ_BIT,reg,&data);

    return data;
}

int i2c_bme280_write_register(uint8_t reg,uint8_t value)
{

    I2cWrite(&BME280I2C, BME280_I2C_ADDRESS2 << 1 | WRITE_BIT,reg,value);
	/* 
    i2c_port_t i2c_num = I2C_MASTER_NUM;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, BME280_I2C_ADDRESS2 << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd,  reg, ACK_CHECK_EN);
    i2c_master_write_byte(cmd,  value, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    int ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_FAIL) {
        return false;
    }
	*/

    return 0;
}


/**
 * @brief  Read pressure in hPA
*/
float i2c_bme280_read_pressure( void )
{

	// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
	// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
	int32_t adc_P = ((uint32_t)i2c_bme280_read_register(BME280_PRESSURE_MSB_REG) << 12) | ((uint32_t)i2c_bme280_read_register(BME280_PRESSURE_LSB_REG) << 4) | ((i2c_bme280_read_register(BME280_PRESSURE_XLSB_REG) >> 4) & 0x0F);
	
    printf("hPA %d,%d\n",adc_P,adc_P/256);

	int64_t var1, var2, p_acc;
	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)calib_data->dig_P6;
	var2 = var2 + ((var1 * (int64_t)calib_data->dig_P5)<<17);
	var2 = var2 + (((int64_t)calib_data->dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)calib_data->dig_P3)>>8) + ((var1 * (int64_t)calib_data->dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)calib_data->dig_P1)>>33;
	if (var1 == 0)
	{
		return 0.0; // avoid exception caused by division by zero
	}
	p_acc = 1048576 - adc_P;
	p_acc = (((p_acc<<31) - var2)*3125)/var1;
	var1 = (((int64_t)calib_data->dig_P9) * (p_acc>>13) * (p_acc>>13)) >> 25;
	var2 = (((int64_t)calib_data->dig_P8) * p_acc) >> 19;
	p_acc = ((p_acc + var1 + var2) >> 8) + (((int64_t)calib_data->dig_P7)<<4);
	
	return (float)p_acc / 256.0;
	
}

/**
 * @brief  Read temp in C
*/
float i2c_bme280_read_temp( void )
{
	// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
	// t_fine carries fine temperature as global value

	//get the reading (adc_T);
	int32_t adc_T = ((uint32_t)i2c_bme280_read_register(BME280_TEMPERATURE_MSB_REG) << 12) | ((uint32_t)i2c_bme280_read_register(BME280_TEMPERATURE_LSB_REG) << 4) | ((i2c_bme280_read_register(BME280_TEMPERATURE_XLSB_REG) >> 4) & 0x0F);
    printf("temp %d,%d\n",adc_T,adc_T/100);


	//By datasheet, calibrate
	int64_t var1, var2;

	var1 = ((((adc_T>>3) - ((int32_t)calib_data->dig_T1<<1))) * ((int32_t)calib_data->dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((int32_t)calib_data->dig_T1)) * ((adc_T>>4) - ((int32_t)calib_data->dig_T1))) >> 12) *
	((int32_t)calib_data->dig_T3)) >> 14;
	t_fine = var1 + var2;
	float output = (t_fine * 5 + 128) >> 8;

	output = output / 100;
	
	return output;
}

float i2c_bme280_read_temp_F( void )
{
	float output = i2c_bme280_read_temp();
	output = (output * 9) / 5 + 32;

	return output;
}

float i2c_bme280_read_rh() 
{
    // Returns humidity in %RH as unsigned 32 bit integer in Q22. 10 format (22 integer and 10 fractional bits).
	// Output value of “47445” represents 47445/1024 = 46. 333 %RH
	int32_t adc_H = ((uint32_t)i2c_bme280_read_register(BME280_HUMIDITY_MSB_REG) << 8) | ((uint32_t)i2c_bme280_read_register(BME280_HUMIDITY_LSB_REG));
	
    printf("rh %d,%d\n",adc_H,adc_H/1024);


	int32_t var1;
	var1 = (t_fine - ((int32_t)76800));
	var1 = (((((adc_H << 14) - (((int32_t)calib_data->dig_H4) << 20) - (((int32_t)calib_data->dig_H5) * var1)) +
	((int32_t)16384)) >> 15) * (((((((var1 * ((int32_t)calib_data->dig_H6)) >> 10) * (((var1 * ((int32_t)calib_data->dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
	((int32_t)calib_data->dig_H2) + 8192) >> 14));
	var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)calib_data->dig_H1)) >> 4));
	var1 = (var1 < 0 ? 0 : var1);
	var1 = (var1 > 419430400 ? 419430400 : var1);

	return (float)(var1>>12) / 1024.0;

}

/*!
 *	@brief This API used to get the
 *	Operational Mode from the sensor in the register 0xF4 bit 0 and 1
 *
 *
 *
 *	@param v_power_mode_u8 : The value of power mode
 *  value           |    mode
 * -----------------|------------------
 *	0x00            | BME280_SLEEP_MODE
 *	0x01 and 0x02   | BME280_FORCED_MODE
 *	0x03            | BME280_NORMAL_MODE
 *
 *
*/
u8 i2c_bme280_get_power_mode()
{
	/* used to return the communication result*/
	u8 v_mode_u8r = BME280_INIT_VALUE;
	/* check the p_bme280 structure pointer as NULL*/
    v_mode_u8r = i2c_bme280_read_register(BME280_CTRL_MEAS_REG);
	v_mode_u8r = BME280_GET_BITSLICE(v_mode_u8r,BME280_CTRL_MEAS_REG_POWER_MODE);
	return v_mode_u8r;
}

/*!
 *	@brief This API used to set the
 *	Operational Mode from the sensor in the register 0xF4 bit 0 and 1
 *
 *
 *
 *	@param v_power_mode_u8 : The value of power mode
 *  value           |    mode
 * -----------------|------------------
 *	0x00            | BME280_SLEEP_MODE
 *	0x01 and 0x02   | BME280_FORCED_MODE
 *	0x03            | BME280_NORMAL_MODE
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
bool bme280_set_power_mode(u8 v_power_mode_u8)
{
#if 0
	BME280_RETURN_FUNCTION_TYPE com_rslt = ERROR;
	u8 v_mode_u8r = BME280_INIT_VALUE;
	u8 v_prev_pow_mode_u8 = BME280_INIT_VALUE;
	u8 v_pre_ctrl_hum_value_u8 = BME280_INIT_VALUE;
	u8 v_pre_config_value_u8 = BME280_INIT_VALUE;
	u8 v_data_u8 = BME280_INIT_VALUE;
	/* check the p_bme280 structure pointer as NULL*/
	if (p_bme280 == BME280_NULL) {
		return E_BME280_NULL_PTR;
		} else {
			if (v_power_mode_u8 <= BME280_NORMAL_MODE) {
				v_mode_u8r = p_bme280->ctrl_meas_reg;
				v_mode_u8r =
				BME280_SET_BITSLICE(v_mode_u8r,
				BME280_CTRL_MEAS_REG_POWER_MODE,
				v_power_mode_u8);
				com_rslt = bme280_get_power_mode(
					&v_prev_pow_mode_u8);
				if (v_prev_pow_mode_u8 != BME280_SLEEP_MODE) {
					com_rslt += bme280_set_soft_rst();
					p_bme280->delay_msec(BME280_3MS_DELAY);
					/* write previous value of
					configuration register*/
					v_pre_config_value_u8 =
					p_bme280->config_reg;
					com_rslt = bme280_write_register(
						BME280_CONFIG_REG,
					&v_pre_config_value_u8,
					BME280_GEN_READ_WRITE_DATA_LENGTH);
					/* write previous value of
					humidity oversampling*/
					v_pre_ctrl_hum_value_u8 =
					p_bme280->ctrl_hum_reg;
					com_rslt += bme280_write_register(
					BME280_CTRL_HUMIDITY_REG,
					&v_pre_ctrl_hum_value_u8,
					BME280_GEN_READ_WRITE_DATA_LENGTH);
					/* write previous and updated value of
					control measurement register*/
					com_rslt += bme280_write_register(
					BME280_CTRL_MEAS_REG,
					&v_mode_u8r,
					BME280_GEN_READ_WRITE_DATA_LENGTH);
				} else {
					com_rslt =
					p_bme280->BME280_BUS_WRITE_FUNC(
					p_bme280->dev_addr,
					BME280_CTRL_MEAS_REG_POWER_MODE__REG,
					&v_mode_u8r,
					BME280_GEN_READ_WRITE_DATA_LENGTH);
				}
				/* read the control measurement register value*/
				com_rslt = bme280_read_register(
					BME280_CTRL_MEAS_REG,
				&v_data_u8, BME280_GEN_READ_WRITE_DATA_LENGTH);
				p_bme280->ctrl_meas_reg = v_data_u8;
				/* read the control humidity register value*/
				com_rslt += bme280_read_register(
					BME280_CTRL_HUMIDITY_REG,
				&v_data_u8, BME280_GEN_READ_WRITE_DATA_LENGTH);
				p_bme280->ctrl_hum_reg = v_data_u8;
				/* read the config register value*/
				com_rslt += bme280_read_register(
					BME280_CONFIG_REG,
				&v_data_u8, BME280_GEN_READ_WRITE_DATA_LENGTH);
				p_bme280->config_reg = v_data_u8;
			} else {
			com_rslt = E_BME280_OUT_OF_RANGE;
			}
		}
	return com_rslt;
#endif
return true;
}
