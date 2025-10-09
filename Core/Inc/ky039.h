/*
 * heartbeat_sensor.h
 *
 *  Created on: Oct 2, 2025
 *      Author: fcheung
 */

#ifndef INC_KY039_H_
#define INC_KY039_H_

#include "adc.h"

/**
 * Reads KY-039 heartbeat sensor, returns value in beats per minute
 *
 * @param	None
 * @returns	int32_t beats per minute
 */
int32_t read_heart(void);

#endif /* INC_KY039_H_ */
