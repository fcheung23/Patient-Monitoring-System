/*
 * adxl345.h
 *
 *  Created on: Jul 23, 2025
 *      Author: fcheung
 */

#ifndef ADXL345_H_
#define ADXL345_H_

#include "i2c.h"

#define DEVID_R				(0x00)
#define DEVICE_ADDR			(0x53)
#define DATA_FORMAT_R		(0x31)
#define POWER_CTL_R			(0x2D)
#define DATA_START_ADDR		(0x32)

#define FOUR_G				(0x01)
#define RESET				(0x00)
#define SET_MEASURE_B		(0x08)

/**
 * Configures adxl345 settings
 *
 * @param	None
 * @returns	None
 */
void adxl_init(void);


/**
 * I2C reads adxl345 values into data_rec[] buffer
 *
 * @param	None
 * @returns	None
 */
void adxl_read_values(uint8_t reg);


/**
 * Reads adxl345 accelerometer, returns vector magnitude times 10 for print precision
 *
 * @param	None
 * @returns	int32_t vector magnitude times 10
 */
int32_t read_accel(void);


#endif /* ADXL345_H_ */
