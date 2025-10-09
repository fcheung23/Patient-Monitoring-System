/*
 * tmp36.h
 *
 *  Created on: Sep 27, 2025
 *      Author: fcheung
 */

#ifndef INC_TMP36_H_
#define INC_TMP36_H_

#include "adc.h"

/**
 * Reads TMP36 temperature sensor, returns value in degrees Celsius times 10 for print precision
 *
 * @param	None
 * @returns	int32_t temperature in degrees Celsius times 10
 */
int32_t read_tmp(void);


#endif /* INC_TMP36_H_ */
